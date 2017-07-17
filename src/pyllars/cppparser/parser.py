import logging
from abc import abstractmethod, ABCMeta
from typing import List

from .lexer import lexer
from .type_lexer import type_lexer

def _class_from_name(name):
    return globals().get(name)


class Element(metaclass=ABCMeta):

    lookup = {}
    tag_lookup = {}

    def __init__(self, name, tag, parent, locator=None, qualifier=None):
        self._name = name
        self._tag = tag
        assert(parent is None or parent is None or isinstance(parent, Element))
        self._parents = [parent] if parent else []
        self._children = {}
        self._locators = locator
        self._qualifiers = qualifier or []
        if isinstance(self._qualifiers, str):
            self._qualifiers = [self._qualifiers]
        if name:
            Element.lookup[self.full_name] = self
        if parent:
            parent.add_child(self)
        Element.tag_lookup[tag] = self
        self._current_child_access = None

    def finalize(self):
        for parent in self._parents:
            parent.add_child(self)

    @property
    def namespace_name(self):
        return self.parent.namespace_name if self.parent else None

    @property
    def guard(self):
        text = self.full_name
        for c in [':', '<', '>', '!', '%', '^', '&', '*',  '[', ']', '\\', '|', '=']:
            text = text.replace(c, '_')
        text =  "__PYLLARS__" + text
        return text

    @property
    def parent(self):
        return self._parents[0] if self._parents else None

    @property
    def tag(self):
        return self._tag

    @property
    def name(self):
        return self._name

    @property
    def parents(self):
        return self._parents

    @property
    def full_name(self):
        return self.name

    @property
    def full_param_name(self):
        # g++ has bug where signatures with const params removes the const-ness when checking template signatures,
        # to template params must not be const (but can be pointer or ref to const types).  QualType therefore
        # overrides this method
        return self.full_name

    @property
    @abstractmethod
    def scope(self):
        pass

    def default_access(self):
        return "public"

    def set_integer_value(self, value):
        pass

    def add_child(self, element):
        assert(element is not None)
        if isinstance(element, AccessSpecDecl):
            self._current_child_access = element.name
        else:
            self._children.setdefault(self._current_child_access or self.default_access(), []).append(element)

    def __repr__(self):
        return self.full_name

    def find(self, type_name):
        if type_name in Element.lookup:
            return Element.lookup[type_name]
        if "::" + type_name in Element.lookup:
            return Element.lookup["::" + type_name]
        for key in self._children:
            for child in self._children[key]:
                if child.name == type_name:
                    return child
        # traverse parents to find type:
        for parent in self._parents:
            if parent.find(type_name):
                return parent.find(type_name)
        return None

    @staticmethod
    def parse(stream):
        def preprocess(line):
            tokens = {}
            depth = None
            lexer.input(line)
            while True:
                token = lexer.token()
                if token is None:
                    break
                if token.type == 'node':
                    depth = token.depth
                else:
                    if token.type in tokens:
                        if isinstance(tokens[token.type], str) or not hasattr(tokens[token.type], "__iter__"):
                            tokens[token.type] = [tokens[token.type]]
                        tokens[token.type].append(token.value)
                    else:
                        tokens[token.type] = token.value
            return depth+1 if depth is not None else None, tokens


        def process_line(tokens, depth, parent):
            node_type = tokens.get('node_type')
            if node_type == 'TranslationUnitDecl':
                return None
            tag = tokens.get('tag')
            name = tokens.get('name') # or "_%s" % str(tag)
            if 'name' in tokens:
                del tokens['name']
            if tag:
                del tokens['tag']
            if node_type is None:
                if 'access' in tokens and 'definition' in tokens and isinstance(parent, CXXRecordDecl):
                    typ = parse_type(tokens['definition'], parent=None)
                    if not typ:
                        print ("ERROR: unable to find type for definition: %s" + tokens['definition'])
                    else:
                        parent.add_base_class(typ, tokens['access'])
                else:
                    print("ERROR: no node type in token set: %s;  parent: %s" % (tokens, parent))
                return None
            del tokens['node_type']
            clazz = _class_from_name(node_type)
            if clazz:
                current_element = clazz.do_parse_tokens(name, tag, parent, **tokens)
            else:
                current_element = UnknownElement(node_type, tag, parent)
            return Branch(depth+1, current_element) if current_element else None

        class Branch:

            def __init__(self, depth, top, parent = None):
                self.depth = depth
                self.top = top
                self.children = []
                self.parent = parent

            def process(self, stream):
                current = self
                prev = None
                for line in stream:
                    line += b" "
                    try:
                        line = line.decode('latin-1').replace('\n', '').replace('\r', '')
                        depth, tokens = preprocess(line)
                        if tokens.get('node_type') == "TranslationUnitDecl":
                            continue
                        assert depth is not None and depth >= 0, depth
                        while current.depth + 1 > depth:
                            current = current.parent
                        if depth > current.depth + 1:
                            current = prev
                        branch = process_line(tokens, depth-1, current.top if current else NamespaceDecl.GLOBAL)
                        if branch is None:
                            continue
                        branch.parent = current
                        current.children.append(branch)
                        prev = branch
                    except:
                        print("Failed to process line %s" % line)
                        import traceback
                        print(traceback.format_exc())

            def __repr__(self):
                return self.str_rep()

            def str_rep(self, indent=""):
                text = "%s%s\n" % (indent, self.top)
                for child in self.children:
                    text += child.str_rep(indent + "  |-")
                return text


        top = Branch(0, NamespaceDecl.GLOBAL)
        top.process(stream)
        return top

    @classmethod
    @abstractmethod
    def parse_tokens(cls, name, tag, parent, **kargs):
        pass

    @classmethod
    def do_parse_tokens(cls, name, tag, parent, **kargs):
        if tag is not None and tag in Element.tag_lookup:
            return Element.tag_lookup[tag]
        else:
            try:
                return cls.parse_tokens(name, tag, parent, **kargs)
            except:
                import traceback
                traceback.print_exc()
                logging.error("Failed to parse class %s" % cls.__name__)

    def children(self, access: str='public') -> List["Element"]:
        if access is None:
            access = self.default_access()
        if access:
            return self._children.get('public') or []
        elif access == 'all':
            children = []
            for access in self._children:
                children += self._children[access] or []
            return children

    def as_function_argument(self, index, typed=False):
        if typed:
            return self._type.full_name + " " + self.name
        return self.name


class ScopedElement(Element):

    @property
    def scope(self):
        if len(self._parents) == 1 and self._parents[-1].name:
            if self._parents[-1].scope:
                return self._parents[-1].scope + "::" + self._parents[-1].name
            else:
                return "::" + self._parents[-1].name
        else:
            return ""

    @property
    def pyllars_module_name(self):
        parent = self.parent
        while parent and not isinstance(parent, NamespaceDecl):
            parent = parent.parent
        return parent.pyllars_module_name

    @property
    def full_name(self):
        if not self._name and self.parent:
            member = None
            for m in self.parent._children:
                if m.tag == self.tag:
                    member = m
                    break
            if not member:
                 return self.scope + "::" + "<<anonumous>>"
            member_name = member.name
            return "decltype(%(class_name)s::%(name)s)" % {'class_name': self.parent.full_name, 'name': member_name}

        return self.scope + "::" + self.name


class UnscopedElement(Element):

    @property
    def scope(self):
        if self._parents and len(self._parents) == 1:
            return self._parents[0].scope
        return ""


class FloatingLiteral(UnscopedElement):

    def __init__(self, name, tag, parent, defnition, locator=None):
        super(FloatingLiteral, self).__init__("<<unnamed>>", tag, parent, locator=locator)
        self._value = name
        self._kind = defnition.replace("'", "")


class UnknownElement(ScopedElement):

    @classmethod
    def parse_tokens(cls, name, tag, parent, **kargs):
        return UnknownElement(name, tag, parent)

    def __repr__(self):
        return "<<Unkown Element:%s>>" % self.full_name


class AccessSpecDecl(UnknownElement):

    @classmethod
    def parse_tokens(cls, name, tag, parent, **kargs):
        return AccessSpecDecl(kargs.get('access'), tag, parent)


class NamespaceDecl(ScopedElement):

    GLOBAL = None  # set later

    def __init__(self, name, tag, parent, locator):
        super(NamespaceDecl, self).__init__(name, tag, parent, locator=locator)

    @classmethod
    def parse_tokens(cls,  name, tag, parent, **kargs):
        return NamespaceDecl(name, tag, parent, **kargs)

    def __repr__(self):
        return "namespace " + self.full_name

    @property
    def namespace_name(self):
        return self.full_name

    @property
    def pyllars_module_name(self):
        if not self.name:
            return 'PyImport_ImportModule("pyllars")'
        else:
            return "pyllars%s::%s_mod" % (self.full_name, self.name)

class BuiltinType(UnscopedElement):

    def __init__(self, name, parent, tag):
        super(BuiltinType, self).__init__(name, tag, parent)

    @classmethod
    def parse_tokens(cls,  name, tag, parent, **kargs):
        return BuiltinType(kargs['definition'].replace("'", ""), parent, tag)

    @property
    def full_name(self):
        return self.name

    @property
    def is_const(self):
        return False

    @property
    def array_size(self):
        return None

class CXXRecord(UnscopedElement):

    @classmethod
    def parse_tokens(cls, name, tag, parent, **kargs):
        name = name or kargs.get('definition').replace("'", "")
        return CXXRecord(name, tag, parent)


class CXXRecordDecl(ScopedElement):

    def __init__(self, name, tag, parent, **kargs):
        super(CXXRecordDecl, self).__init__(name, tag, parent,
                                            locator=kargs.get('locator'))
        self._is_referenced = kargs.get('is_referenced') or False
        self._is_definition = kargs.get('is_definition') or False
        self._kind = kargs.get('structured_type')
        self._base_classes = {}
        self._is_implicit = kargs.get('implicit_explicit') == 'implicit'

    def add_base_class(self, typ, access):
        self._base_classes.setdefault(access, []).append(typ)

    @property
    def public_base_classes(self):
        return self._base_classes.get('public')

    @property
    def is_implicit(self):
        return self._is_implicit

    def default_access(self):
        return "private" if self._kind == "class" else 'public'

    @classmethod
    def parse_tokens(cls, name, tag, parent, **kargs):
        if kargs.get('implicit_explicit') == 'implicit':
            return None
        return CXXRecordDecl(name, tag, parent, **kargs)


class DecoratingType(UnscopedElement):

    def __init__(self, name, tag, parent, qualifier=None, postfix=None,  locator=None):
        super(DecoratingType, self).__init__(name, tag, parent, locator, qualifier=qualifier)
        self._postfix = postfix

    @classmethod
    def parse_tokens(cls, name, tag, parent, **kargs):
        return cls(name, tag, parent, **kargs)

    @property
    def is_const(self):
        return 'const' in (self._qualifiers or [])

    def add_child(self, element):
        super(DecoratingType, self).add_child(element)
        assert(len(self._children.values()) == 1)
        assert(len(list(self._children.values())[0]) == 1)


    @property
    def name(self):
        if not self._children:
            return "%s<undetermined>" % self.__class__.__name__
        assert(len(self._children.values()) == 1)
        assert(len(list(self._children.values())[0]) == 1)
        return  list(self._children.values())[0][0].name + (self._postfix or "") + (" " if self._qualifiers else "") + \
               " ".join(reversed(self._qualifiers))

    @property
    def full_name(self):
        if not self._children:
            return "%s<undetermined>" % self.__class__.__name__
        assert(len(self._children.values()) == 1)
        assert(len(list(self._children.values())[0]) == 1)
        return list(self._children.values())[0][0].full_name + (self._postfix or "") + (" " if self._qualifiers else "") + \
            " ".join(reversed(self._qualifiers))

    @property
    def array_size(self):
        return None


class ArrayType(DecoratingType):

    def __init__(self, name, tag, parent, array_size, base_type, locator=None):
        self._base_type = base_type
        self._array_size = array_size
        super(ArrayType, self).__init__(name, tag, parent, qualifier=None, postfix="[%d]" % array_size, locator=locator)
        self.add_child(base_type)

    @property
    def array_size(self):
        return self._array_size

    @property
    def name(self):
        return self._base_type.name + "[%d]" % self.array_size

    @property
    def full_name(self):
        return self._base_type.full_name +  "[%d]" % self.array_size

class PointerType(DecoratingType):

    def __init__(self, name, tag, parent, definition=None, locator=None):
        super(PointerType, self).__init__(name, tag, parent, qualifier=None, postfix="*", locator=locator)


class ReferenceType(DecoratingType):

    def __init__(self, name, tag, parent, definition=None, locator=None):
        super(ReferenceType, self).__init__(name, tag, parent, qualifier=None, postfix="&", locator=locator)


class QualType(DecoratingType):

    def __init__(self, name, tag, parent, qualifier=None, definition=None, locator=None):
        if qualifier is None:
            raise Exception("Invalid qual type")
        super(QualType, self).__init__(name, tag, parent, qualifier=qualifier, postfix=None, locator=locator)
        assert(not self.children())

    @property
    def full_param_name(self):
        # g++ has bug where signatures with const params removes the const-ness when checking template signatures,
        # to template params must not be const (but can be pointer or ref to const types)
        if 'const' in self._qualifiers or []:
            return self.children()[0].full_name


def parse_type(definition, parent, tokens=None):
    array_size = None
    if not tokens:
        type_lexer.input(definition.replace("'", ''))
        tokens = []
        while True:
            token = type_lexer.token()
            if token is None:
                break
            if token.type == 'array_spec':
                array_size = token.value
            else:
                tokens.append(token)
        typ = parse_type(definition, parent, [t for t in reversed(tokens)])
        if array_size is not None:
            typ = ArrayType(typ.name + "[%d]" % array_size, None, parent, array_size, base_type=typ)
        return typ
    typ = None
    if tokens:
        token = tokens[0]
        token.value = token.value.strip()
        if token.type == 'qualifier':
            qual_typ = QualType("<<qual>>", None, parent, qualifier=[token.value])
            t = parse_type(definition, qual_typ, tokens[1:])
            if not qual_typ.children():
                qual_typ.add_child(t)
            qual_typ._parents = t._parents
            typ = qual_typ
        elif token.type == 'reference':
            ref_type = {'*': PointerType, '&': ReferenceType}[token.value]("<<ref>>", None, parent=parent)
            #parent.add_child(ref_type)
            t = parse_type(definition, ref_type, tokens[1:])
            if not ref_type.children():
                ref_type.add_child(t)
            ref_type._parents = t._parents
            typ = ref_type
        elif token.type == 'name':
            if len(tokens) > 1:
                typ = parse_type(definition, parent, [t for t in reversed(tokens)])
                assert typ is not None
            else:
                typ = parent.find(token.value)
                assert typ is not None
        elif token.type in ['structured_type']:
            assert tokens[1].type == 'name'
            if len(tokens) > 2:
                return parse_type(definition, parent, [t for t in reversed(tokens[2:])] + tokens[:2])
            if parent.find(tokens[1].value):
                typ = parent.find(tokens[1].value)
            else:
                if "::" + tokens[1].value in Element.lookup:
                    return Element.lookup["::" + tokens[1].value]
                typ = CXXRecordDecl(tokens[1].value, None, parent)
        else:
            raise Exception("Unknown token type: %s" % token.type)

        return typ


class TypeAliasDecl(ScopedElement):

    def __init__(self, name, tag, parent, alias_definition, locator=None):
        super(TypeAliasDecl, self).__init__(name, tag, parent, locator)
        short_hand, full_decl = alias_definition
        self._aliased_type = parent.find(short_hand)

    @classmethod
    def parse_tokens(cls, name, tag, parent, **kargs):
        return TypeAliasDecl(name, tag, parent, **kargs)


class VarDecl(ScopedElement):

    def __init__(self, name, tag, parent, definition=None, alias_definition=None, qualifier=None, locator=None):
        super(ScopedElement, self).__init__(name if isinstance(name, str) else name[-1] if name else None, tag, parent,
                                            locator=locator, qualifier=qualifier)
        assert not (definition is None and alias_definition is None)
        assert definition is None or alias_definition is None
        if not definition:
            definition = alias_definition[1]
        self._type = parse_type(definition, parent)

    @property
    def type_(self):
        return self._type

    @classmethod
    def parse_tokens(cls, name, tag, parent, **kargs):
        return cls(name, tag, parent, **kargs)

    @property
    def is_static(self):
        return 'static' in (self._qualifiers or [])


class ParmVarDecl(VarDecl):

    @property
    def full_name(self):
        return self.name

class FunctionElement(ScopedElement):

    def __init__(self, name, tag, parent, definition, locator=None, qualifier=None):
        try:
            super(FunctionElement, self).__init__(name, tag, parent, locator=locator, qualifier=qualifier)
            from .function_lexer import function_lexer
            function_lexer.input(definition)
            tokens = {}
            has_ellipsis = False
            while True:
                token = function_lexer.token()
                if token is None:
                    break
                tokens[token.type] = token
                if token.type == 'parameters':
                    has_ellipsis = token.has_ellipsis
            qualifiers = definition.rsplit(')', maxsplit=1)[-1]
            self._qualifiers += qualifiers.split(' ') if qualifiers.strip() else []
            return_type_name = definition.split('(')[0].strip()
            self._return_type = parse_type(definition.split('(')[0], parent) if return_type_name != 'void' else None
            # params added as ParamVarDecl's, except for ellipsis
            self._has_varargs = has_ellipsis
            self._throws = tokens.get('throws').throws if 'throws' in tokens else []
        except:
            import traceback
            traceback.print_exc()
            raise
    @property
    def is_const(self):
        return 'const' in (self._qualifiers or [])

    @property
    def is_static(self):
        return 'static' in (self._qualifiers or [])

    def add_child(self, element):
        if element.name != "void":
            super(FunctionElement, self).add_child(element)

    @classmethod
    def parse_tokens(cls, name, tag, parent, **kargs):
        return cls(name, tag, parent, kargs['definition'].replace("\\", "").replace("'", ""), qualifier=kargs.get('qualifier'))

    @property
    def return_type(self):
        return self._return_type

    @property
    def throws(self):
        return self._throws

    @property
    def params(self):
        return [c for c in self.children('public') if isinstance(c, ParmVarDecl)]

    def default_access(self):
        return 'public'

    @property
    def has_varargs(self):
        return self._has_varargs


class FunctionDecl(FunctionElement):

    def __init__(self, name, tag, parent, definition, locator=None, qualifier=None):
        super(FunctionDecl, self).__init__(name, tag, parent, definition, locator=locator, qualifier=qualifier)


class FunctionTypeDecl(FunctionElement):

    def as_function_argument(self, name, typed=False):
        return "%s(*%s)(%s %s)" % (self.return_type.full_name,
                                    name,
                                    ", ".join([a.full_name for a in self.params]),
                                    "..." if self.has_varargs else "")


class CXXConstructorDecl(FunctionDecl):
    pass


class CXXDestructorDecl(FunctionDecl):
    pass


class CXXMethodDecl(FunctionDecl):
    pass


class FieldDecl(ScopedElement):

    def __init__(self, name, tag, parent, **kargs):
        super(FieldDecl, self).__init__(name, tag, parent, locator=kargs.get('locator'))
        self._type = parse_type(kargs.get('definition'), parent)
        self._bit_size = None

    @property
    def type_(self):
        return self._type

    def set_integer_value(self, value):
        self._bit_size = int(value)

    @property
    def bit_size(self):
        return self._bit_size

    @classmethod
    def parse_tokens(cls, name, tag, parent, **kargs):
        return FieldDecl(name, tag, parent, **kargs)


class IntegerLiteral(ScopedElement):

    def __init__(self, name, tag, parent, integer_value, locator=None):
        super(IntegerLiteral, self).__init__(name, tag, parent, locator=locator)
        if parent:
            parent.set_integer_value(integer_value)

    @classmethod
    def parse_tokens(cls, name, tag, parent, **kargs):
        return cls(name, tag, parent, kargs.get('number'), kargs.get('locator'))


class RecordType(ScopedElement):

    def __init__(self, name, tag, parent, **kargs):
        super(RecordType, self).__init__(name, tag, parent, locator=kargs.get('locator'))
        tokens = {}
        type_lexer.input(kargs.get('definition').replace("'", ""))
        while True:
            token = type_lexer.token()
            if token is None:
                break
            if token.type in tokens:
                tokens[token.type] = [tokens[token.type], token]
            else:
                tokens[token.type] = token
        self._kind = tokens.get('structured_type').value
        name = tokens.get('name')
        name = name.value if name else None
        if name:
            self._name = name.split("::")[-1]
            self._qualified_name = name
        else:
            self._name = None
            self._qualified_name = None
        self._qualifiers = [t.value for t in tokens.get('qualifier')] if 'qualifier' in tokens else []

    @property
    def qualified_name(self):
        return self._qualified_name

    @classmethod
    def parse_tokens(cls, name, tag, parent, **kargs):
        return RecordType(name, tag, parent, **kargs)


class TemplateDecl(ScopedElement):
    def __init__(self, name, tag, parent, locator=None):
        super(TemplateDecl, self).__init__(name, tag, parent, locator)
        self._template_type_params = {}

    def add_template_type_param(self, element):
        self._template_type_params[element.name] = element

    def find(self, type_name):
        if type_name in self._template_type_params:
            return self._template_type_params[type_name]
        return super(TemplateDecl, self).find(type_name)


class ClassTemplateDecl(TemplateDecl):

    def __init__(self, name, tag, parent, locator=None):
        super(ClassTemplateDecl, self).__init__(name, tag, parent, locator)

    @classmethod
    def parse_tokens(cls, name, tag, parent, **kargs):
        return ClassTemplateDecl(name, tag, parent, **kargs)


class TemplateTypeParmDecl(ScopedElement):

    def __init__(self, name, tag, parent, structured_type=None, locator=None, is_referenced=False):
        super(TemplateTypeParmDecl, self).__init__(name, tag, parent, locator)
        self._kind = structured_type
        self._is_referenced = is_referenced
        parent.add_template_type_param(self)

    @classmethod
    def parse_tokens(cls, name, tag, parent, **kargs):
        return TemplateTypeParmDecl(name, tag, parent, **kargs)

    @property
    def is_type(self):
        return True


class NonTemplateTypeParmDecl(ScopedElement):

    def __init__(self, name, tag, parent, definition, locator=None):
        super(NonTemplateTypeParmDecl, self).__init__(name, tag, parent, locator)
        self._type = parse_type(definition, parent)
        #self._is_referenced = is_referenced

    @classmethod
    def parse_tokens(cls, name, tag, parent, **kargs):
        return NonTemplateTypeParmDecl(name, tag, parent, **kargs)

    @property
    def is_type(self):
        return False


class ClassTemplateSpecializationDecl(ScopedElement):

    def __init__(self, name, tag, parent, structured_type=None, locator=None):
        self._base_name = name
        super(ClassTemplateSpecializationDecl, self).__init__("<<specialized %s @%s>>" %(name, tag), tag, parent, locator)
        self._kind = structured_type
        self._base_template = parent.find(name)

    @classmethod
    def parse_tokens(cls, name, tag, parent, **kargs):
        return ClassTemplateSpecializationDecl(name, tag, parent, **kargs)

    @property
    def name(self):
        children = self._children.get('public') or []
        suffix = "<%s>" % (", ".join([str(c._value) for c in children if c._value]))
        return self._base_name + suffix


class TemplateArgument(ScopedElement):

    def __init__(self, name, tag, parent, number=None, definition=None, locator=None):
        super(TemplateArgument, self).__init__(name, tag, parent, locator)
        self._value = None
        if name == 'type':
            self._value = parent.find(definition)
        elif name == 'expr':
            self._value = None
        elif "integral" == name:
            self._value = int(number)
        else:
            raise Exception("Unknown TemplateArgument type: %s" % name)

    @classmethod
    def parse_tokens(cls, name, tag, parent, **kargs):
        return TemplateArgument(name, tag, parent, **kargs)


class TranslationUnitDecl(ScopedElement):

    @classmethod
    def parse_tokens(cls, name, tag, parent, **kargs):
        return TranslationUnitDecl(name, tag, parent, **kargs)


class TypedefDecl(ScopedElement):

    def __init__(self, name, tag, parent, **kargs):
        super(TypedefDecl, self).__init__(name, tag, parent, kargs.get('locator'))
        self._is_implicit = kargs.get('implicit_explicit') == 'implicit'

    @classmethod
    def parse_tokens(cls, name, tag, parent, **kargs):
        return TypedefDecl(name, tag, parent, **kargs)

def init():
    Element.lookup = {}
    Element.tag_lookup = {}
    NamespaceDecl.GLOBAL = NamespaceDecl("", None, None, None)
    NamespaceDecl.GLOBAL.add_child(BuiltinType("void", NamespaceDecl.GLOBAL, None))
    NamespaceDecl.GLOBAL.add_child(BuiltinType("char", NamespaceDecl.GLOBAL, None))
    NamespaceDecl.GLOBAL.add_child(BuiltinType("signed char", NamespaceDecl.GLOBAL, None))
    NamespaceDecl.GLOBAL.add_child(BuiltinType("unsigned char", NamespaceDecl.GLOBAL, None))
    NamespaceDecl.GLOBAL.add_child(BuiltinType("short", NamespaceDecl.GLOBAL, None))
    NamespaceDecl.GLOBAL.add_child(BuiltinType("signed short", NamespaceDecl.GLOBAL, None))
    NamespaceDecl.GLOBAL.add_child(BuiltinType("unsigned short", NamespaceDecl.GLOBAL, None))
    NamespaceDecl.GLOBAL.add_child(BuiltinType("int", NamespaceDecl.GLOBAL, None))
    NamespaceDecl.GLOBAL.add_child(BuiltinType("signed", NamespaceDecl.GLOBAL, None))
    NamespaceDecl.GLOBAL.add_child(BuiltinType("signed int", NamespaceDecl.GLOBAL, None))
    NamespaceDecl.GLOBAL.add_child(BuiltinType("unsigned int", NamespaceDecl.GLOBAL, None))
    NamespaceDecl.GLOBAL.add_child(BuiltinType("long", NamespaceDecl.GLOBAL, None))
    NamespaceDecl.GLOBAL.add_child(BuiltinType("signed long", NamespaceDecl.GLOBAL, None))
    NamespaceDecl.GLOBAL.add_child(BuiltinType("unsigned long", NamespaceDecl.GLOBAL, None))
    NamespaceDecl.GLOBAL.add_child(BuiltinType("long long", NamespaceDecl.GLOBAL, None))
    NamespaceDecl.GLOBAL.add_child(BuiltinType("signed long long", NamespaceDecl.GLOBAL, None))
    NamespaceDecl.GLOBAL.add_child(BuiltinType("unsigned long long", NamespaceDecl.GLOBAL, None))
    NamespaceDecl.GLOBAL.add_child(BuiltinType("float", NamespaceDecl.GLOBAL, None))
    NamespaceDecl.GLOBAL.add_child(BuiltinType("double", NamespaceDecl.GLOBAL, None))

init()

def parse_file(src_path):
    import subprocess
    cmd = ["clang-check", "-ast-dump", src_path, "--extra-arg=\"-fno-color-diagnostics\""]
    proc = subprocess.Popen(cmd, stdout=subprocess.PIPE)
    return Element.parse(proc.stdout)
