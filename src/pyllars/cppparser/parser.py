import logging
from abc import abstractmethod, ABCMeta
from typing import List

import os

from .lexer import lexer
from .type_lexer import type_lexer

def _class_from_name(name):
    return globals().get(name)


class UnresolvedElement(object):

    def __init__(self, name, defining_scope):
        self._name = name
        self._parent = None
        self._defining_scope = defining_scope
        Element.lookup[name] = self

    @property
    def parent(self):
        return None

    @property
    def name(self):
        return self._name

    @property
    def is_resolved(self):
        return False

    def resolve(self):
        element = Element.from_name(self._name)
        scope = self._defining_scope
        while (element is None or isinstance(element, UnresolvedElement) or not element.is_scopable) and scope:
            element = Element.from_name(scope.full_name + "::" + self._name)
            scope = scope.parent
        if not element or isinstance(element, UnresolvedElement):
            raise Exception("Unresolvable type name: %s" % self._name)
        #elif not element._target_type.is_resolved:
        #    element._target_type = parse_type(element.target_type.name, self._defining_scope)
        return element


class Element(metaclass=ABCMeta):

    lookup = {}
    tag_lookup = {}
    last_parsed_type = None

    @staticmethod
    def reset():
        Element.lookup = {}
        Element.tag_lookup = {}
        Element.last_parsed_type = None
        init()

    def __init__(self, name, tag, parent, locator=None, qualifier=None, **kargs):
        self._name = name
        self._tag = tag
        self._is_implicit = kargs.get('implicit_explicit') == 'implicit'
        assert(parent is None or parent is None or isinstance(parent, Element))
        assert(parent != self)
        while isinstance(parent, DecoratingType):
            parent = parent.parent
        self._parent = parent
        self._top_scope = None
        self._children = {}
        if locator is None or isinstance(locator, str):
            self._locator = locator
        else:
            self._locator = locator[-1]
        self._qualifiers = qualifier or []
        if isinstance(self._qualifiers, str):
            self._qualifiers = [self._qualifiers]
        self._template_type_params = {}
        self._template_arguments = []
        if parent:
            parent.add_child(self)
        if tag:
            Element.tag_lookup[tag] = self
        self._current_child_access = None
        self._anonymous_types = set([])
        self._is_template = parent and parent.is_template
        self._inside_template_macro = (parent._inside_template_macro or parent.is_template_macro) if parent else False
        self._is_anonymous_type = name is None or "enum " in name
        self._dependencies = set([])
        self._include_paths = []
        if name is not None and self.is_resolved:
            full_name = self.scope + ('::' if self.scope else "") +  name
            if full_name in Element.lookup and  kargs.get('is_referenced') is not True:
                if issubclass(type(Element.lookup[full_name]), type(self)):
                    pass
                elif issubclass(type(self), type(Element.lookup[full_name])):
                    Element.lookup[full_name] = self
                else:
                    raise Exception("Duplicate differing type def %s" % full_name)
            if full_name and full_name not in Element.lookup:
                Element.lookup[full_name] = self
    @property
    def is_template(self):
        return self._is_template

    @property
    def is_resolved(self):
        return True

    @property
    def is_anonymous_type(self):
        return self._is_anonymous_type

    def parent_is_tempalte(self):
        # Clang AST defines a CXXRecord within a ClassTemplateDecl that is same name as template that items belong to
        return self.parent and self.parent.parent and self.parent.parent.is_template

    def make_complete(self):
        for child in self.children():
            child.make_complete()

    @property
    def namespace_name(self):
        parent = self.parent
        while parent and not parent.is_namespace:
            parent = parent.parent
        return parent.name if parent else ""

    @property
    def root_python_module(self):
        root = self.parent
        while root and root.parent != NamespaceDecl.GLOBAL and isinstance(root.parent, NamespaceDecl):
            root = root.parent.parent
        if root is None or root == NamespaceDecl.GLOBAL:
            location = self.location[0]
            if not os.path.exists(location or ""):
                return None
            return os.path.basename(location).split('.')[0].replace(' ', "_").replace("-", "_")
        else:
            return root.basic_name

    @staticmethod
    def set_default_location(location: str):
        Element._default_location = location

    @property
    def location(self):
        if not self._locator:
            if self.parent:
                return self.parent.location
            return None, None
        try:
            filename, lineno = self._locator.replace("<", "").split(':', 1)
            if "invalid sloc" in filename:
                filename = None
                lineno = None
            elif filename == 'line':
                filename = self._default_location
                return filename, lineno
            elif not os.path.exists(os.path.abspath(filename)):
                for path in self.include_paths:
                    filename = os.path.join(path, filename)
                    if os.path.exists(filename):
                        break
        except:
            filename = self._locator.replace("<", "").replace(">", "")
            if filename and "invalid sloc" in filename:
                return None, None
            elif filename and filename.split(':')[0] == 'line':
                filename = self._default_location
            lineno = None
        if filename and not os.path.exists(filename):
            filename = None
            lineno = None
        #elif filename:
        #    filename = os.path.abspath(filename)
        if filename is None and self.parent:
            return self.parent.location
        return filename, lineno

    @property
    def guard(self):
        text = self.name
        for c in [':', '<', '>', '!', '%', '^', '&', '*',  '[', ']', '\\', '|', '=', '(', ')', ',', ' ']:
            text = text.replace(c, '_')
        text =  "__PYLLARS__" + text
        return text

    @property
    def is_scopable(self):
        return False

    @property
    def parent(self):
        return self._parent

    @property
    def tag(self):
        return self._tag

    @property
    def basic_name(self):
        if not self._name or "enum " in self._name:
            return ""
        return self._name

    @property
    def name(self):
        return self.basic_name + (self.parent.template_arguments_string() if self.parent else "")

    @property
    def is_const(self):
        return 'const' in (self._qualifiers or [])

    @property
    def full_name(self):
        return self.block_scope + ("::" if self.block_scope else "") + self.name


    @property
    def full_param_name(self):
        return self.full_name

    @property
    def is_implicit(self):
        return self._is_implicit

    @property
    @abstractmethod
    def scope(self):
        pass

    @property
    @abstractmethod
    def block_scope(self):
        pass

    @property
    def is_union(self):
        return False

    @property
    def array_size(self):
        return None

    @property
    def is_namespace(self):
        return False

    @property
    def is_template_macro(self):
        return False

    @property
    def dependencies(self):
        deps = self.dependencies
        for child in self.children():
            deps += child.dependencies
        return deps

    @property
    def top_scope(self):
        if self._top_scope:
            return self._top_scope
        parent = self.parent
        while parent.parent and parent.parent.name:
            parent = parent.parent
        if not parent.name:
            parent = None  # do not count global namespace
        self._top_scope = parent
        return parent

    @property
    def include_paths(self):
        return self._include_paths

    @include_paths.setter
    def include_paths(self, paths):
        self._include_paths = paths

    @classmethod
    def from_name(cls, name):
        if '::' + name in Element.lookup:
            return Element.lookup['::' + name]
        else:
            return Element.lookup.get(name)

    def filter(self, src_paths: List[str], include_paths: List[str]):
        def full_path(path):
            if os.path.isabs(path):
                return path
            # TODO: Better mech for "/usr/include": system includes
            for inc_path in include_paths + ["/usr/include"]:
                if os.path.isfile(os.path.join(inc_path, path)):
                    return os.path.join(inc_path, path)
            return path

        full_src_paths = [full_path(path) for path in src_paths]
        for element in self.children():
            location = element.location[0]
            if location is not None and not full_path(location) in full_src_paths:
                self._children['public'].remove(element)

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
        if not self.name:
            return "<<anonymous-%s" % self.tag
        return self.name

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
        if self.parent and self.parent.find(type_name):
            return self.parent.find(type_name)
        return None

    @staticmethod
    def parse(stream, include_paths: List[str]):
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
                        if token.type == 'tag':
                            continue
                        if isinstance(tokens[token.type], str) or not hasattr(tokens[token.type], "__iter__"):
                            tokens[token.type] = [tokens[token.type]]
                        tokens[token.type].append(token.value)
                    else:
                        tokens[token.type] = token.value
            return depth+1 if depth is not None else None, tokens


        def process_line(location, tokens, depth, parent):
            node_type = tokens.get('node_type')
            if node_type in ['TranslationUnitDecl']:
                return None
            tag = tokens.get('tag')
            name = tokens.get('name') # or "_%s" % str(tag)
            possible_location = tokens.get('locator')
            if possible_location and not isinstance(possible_location, str):
                possible_location = possible_location[-1]
            if possible_location:
                possible_location = possible_location.split(':', 1)[0]
                if possible_location != 'line':
                    location = possible_location
                elif not location:
                    raise Exception("referenced line location without reference set")
            if location and "invalid sloc" in location:
                location = None
            elif location and not os.path.isabs(location):
                for dir in include_paths + ["."]:
                    if os.path.exists(os.path.join(dir, location)):
                        location = os.path.join(dir, location)
                        break
                if not os.path.isabs(location):
                    print("location not found: %s" % location)
                    location = None
            if name and not isinstance(name, str):
                name = ' '.join(name)
            if 'name' in tokens:
                del tokens['name']
            if not name:
                try:
                    if 'definition' in tokens:
                        definition = tokens['definition']
                        if definition.startswith('struct'):
                            name = definition.split(' ')[-1]
                except:
                    pass
            if tag:
                del tokens['tag']
            if node_type is None:
                if 'access' in tokens and 'definition' in tokens and isinstance(parent, CXXRecordDecl):
                    typ = parse_type(None, tokens['definition'], parent, var_name=name)
                    if not typ:
                        print ("ERROR: unable to find type for definition: %s" + tokens['definition'])
                    elif isinstance(type, RecordTypeDefn):
                        parent.add_base_class(typ, tokens['access'])
                else:
                    print("ERROR: no node type in token set: %s;  parent: %s" % (tokens, parent))
                return location, None
            del tokens['node_type']
            clazz = _class_from_name(node_type)
            if clazz:
                current_element = clazz.do_parse_tokens(name, tag, parent, **tokens)
                if current_element:
                    current_element.include_paths = include_paths
                if isinstance(current_element, CXXRecordDecl):
                    Element.last_parsed_type = current_element
            else:
                current_element = UnknownElement(node_type, tag, parent)
            return location, Branch(depth+1, current_element) if current_element else None

        class Branch:

            def __init__(self, depth, top, parent = None):
                self.depth = depth
                self.top = top
                self.children = []
                self.parent = parent

            def process(self, stream):
                import ply
                current = self
                prev = None
                location = None
                with open('parser.log', 'w') as log:
                    for line in stream:
                        line += b" "
                        try:
                            line = line.decode('latin-1').replace('\n', '').replace('\r', '')
                            subtext = True
                            while subtext:
                                index = line.find("(anonymous ")
                                if index < 0:
                                    break
                                end = line[index+1:].find(")") + index + 1
                                subtext = line[index:end+1]
                                if subtext:
                                    line = line.replace(subtext, "")
                            depth, tokens = preprocess(line)
                            if tokens.get('node_type') == "TranslationUnitDecl":
                                continue
                            assert depth is not None and depth >= 0, depth
                            while current.depth + 1 > depth:
                                current = current.parent
                            if depth > current.depth + 1:
                                current = prev
                            location, branch = process_line(location, tokens, depth-1, current.top if current else NamespaceDecl.GLOBAL)
                            if branch is None:
                                continue
                            log.write("%s\n" %line)
                            log.write("   LOC: %s\n" % branch.top.location[0])
                            branch.top.set_default_location(location)
                            branch.parent = current
                            current.children.append(branch)
                            prev = branch
                        except ply.lex.LexError:
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
        for element in top.top.children():
            element.make_complete()
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
            import ply.lex as lex
            try:
                while isinstance(parent, BogusElement):
                    parent = parent.parent
                return cls.parse_tokens(name, tag, parent, **kargs)
            except lex.LexError:
                import traceback
                traceback.print_exc()
                logging.error("Failed to parse class %s" % cls.__name__)
            except Exception:
                raise

    def resolve(self):
        return self

    def children(self, access: str='public') -> List["Element"]:
        if access is None:
            access = self.default_access()
        if access:
            return self._children.get('public') or []
        elif access == 'all':
            children = []
            for access in self._children:
                children += self._children[access].resolve() or []
            return children

    def as_function_argument(self, index, typed=False):
        if typed:
            if isinstance(self.type_, FunctionElement):
                type_name = self.type_.full_name
                name = self.name or "p%s" % index
                return type_name.replace("(*)", "(*%s)" % name)
            return self._type.full_name + " " + (self.name or "p%s" % index)
        return self.name

    @staticmethod
    def template_declaration(template_args):
        def template_parm_name(e):
            if isinstance(e, TemplateTypeParmDecl):
                return "typename %s" % e.name
            else:
                return "%s %s" % (e._type.name, e.name)

        return "" if not template_args else "template < %s >" % (", ".join([template_parm_name(e) for e in template_args]))


    def template_decl(self):
        #if self.parent and self.parent.is_template:
        #    return self.parent.template_decl()
        return self.template_declaration(self._template_arguments)

    def template_arguments_string(self):
        #if not self._template_type_params and self.parent and self.parent.is_template:
        #    return self.parent.template_arguments_string()
        return "" if not self._template_type_params else "<%s>" % (", ".join([e.name for e in self._template_arguments]))

    @property
    def template_arguments(self):
        return self._template_arguments

    def add_template_arg(self, element):
        self._template_arguments.append(element)
        if isinstance(element, TemplateTypeParmDecl):
            self.add_template_type_param(element)

    def add_template_type_param(self, element):
        self._template_type_params[element.name] = element


class ScopedElement(Element):

    def __init__(self, *args, **kargs):
        super(ScopedElement, self).__init__(*args, **kargs)

    @property
    def pyllars_module_name(self):
        if not self.name:
            return "global_mod"
        else:
            return "%s::%s::%s_mod" % (self.pyllars_scope, self.name, self.name)

    @property
    def scope(self):
        if self.is_anonymous_type:
            return ""
        if isinstance(self.parent, ClassTemplateDecl):
            parent = self.parent.parent
            prefix = ""
        else:
            parent = self.parent
            prefix = ""
        if parent and parent.name:
            if parent.scope:
                def qualified_name(name):
                    return name.replace("(", "_lparen_").replace(")", "_rparen_").replace(":", "_")
                name = parent.name if not parent.name.startswith('decltype') \
                    else qualified_name(parent.name)
                return parent.scope + "::" + prefix + name
            else:
                return "::" + parent.name
        else:
            return ""

    @property
    def block_scope(self):
        if self.is_anonymous_type:
            return ""
        if self.parent.is_template_macro:
            parent = self.parent.parent
        else:
            parent = self.parent
        prefix = "" if not parent.parent or not parent.parent.is_template_macro else "template "
        if parent and parent.name:
            block_scope = parent.block_scope
            if block_scope:
                def qualified_name(name):
                    return name.replace("(", "_lparen_").replace(")", "_rparen_").replace(":", "_")
                name = parent.name if not parent.name.startswith('decltype') \
                    else qualified_name(parent.name)
                return block_scope + "::" + prefix + name
            else:
                return "::" + parent.name
        else:
            return ""

    @property
    def pyllars_scope(self):
        parent = self.parent
        basic_names = []
        while parent and parent.name:
            basic_names = [parent.scope_fragment] + basic_names
            parent = parent.parent
        basic_name = "::".join(["pyllars"] + basic_names)
        return basic_name

    @property
    def pyllars_block_scope(self):
        parent = self.parent
        basic_names = []
        while parent and parent.name:
            prefix = "" if not parent.parent or not parent.parent.is_template_macro else "template "
            basic_names = [prefix + parent.scope_fragment] + basic_names
            parent = parent.parent
        basic_name = "::".join(["pyllars"] + basic_names)
        return basic_name

    @property
    def scope_fragment(self):
        return self.name

    @property
    def block_scope_fragment(self):
        return ("template " if self.parent.is_template_macro else "") + self.name

    @property
    def full_name(self):
        if not self._name:
            if self.parent:
                return self.parent.name
            return "::"
        else:
            return self.scope + ("::" if self.scope != "::" else "") + self.name + (self.parent.template_arguments_string() if self.parent else "")


class UnscopedElement(Element):

    @property
    def scope(self):
        if self.is_anonymous_type:
            return ""
        if self.parent:
            return self.parent.scope
        return ""

    @property
    def block_scope(self):
        return self.scope


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
        return "<<Unkown Element:%s>>" % self.name


class AccessSpecDecl(UnknownElement):

    @classmethod
    def parse_tokens(cls, name, tag, parent, **kargs):
        return AccessSpecDecl(kargs.get('access'), tag, parent)


class NamespaceDecl(ScopedElement):

    GLOBAL = None  # set later

    def __init__(self, name, tag, parent, locator, **kargs):
        super(NamespaceDecl, self).__init__(name, tag, parent, locator=locator)

    @classmethod
    def parse_tokens(cls,  name, tag, parent, **kargs):
        return NamespaceDecl(name, tag, parent, **kargs)

    def __repr__(self):
        return "namespace " + self.name

    @property
    def namespace_name(self):
        return self.name

    @property
    def is_namespace(self):
        return True

    @property
    def is_scopable(self):
        return True


class BuiltinType(UnscopedElement):

    DEFINITIONS = {}

    def __init__(self, name, parent, tag):
        super(BuiltinType, self).__init__(name, tag, parent)
        BuiltinType.DEFINITIONS[name] = self

    @classmethod
    def parse_tokens(cls,  name, tag, parent, **kargs):
        return BuiltinType(kargs['definition'].replace("'", ""), None, tag)

    @property
    def full_name(self):
        return self.name

    @property
    def name(self):
        return self._name

    @property
    def is_const(self):
        return False

    @property
    def array_size(self):
        return None

    @property
    def scope(self):
        return ""

    @property
    def block_scope(self):
        return ""

    def to_py_conversion_code(self, value):
        return {
            "char": "PyLong_FromLong((long)%(val)s)" % value,
            "signed char": "PyLong_FromLong((long)%(val)s)" % value,
            "unsigned char": "PyLong_FromUnsignedLong((unsigned long)%(val)s)" % value,
            "short": "PyLong_FromLong((long)%(val)s)" % value,
            "signed short": "PyLong_FromLong((long)%(val)s)" % value,
            "unsigned short": "PyLong_FromUnsignedLong((unsigned long)%(val)s)" % value,
            "int": "PyLong_FromLong((long) %(val)s)" % value,
            "signed": "PyLong_FromLong((long) %(val)s)" % value,
            "unsigned": "PyLong_FromUnsignedLong((unsigned long) %(val)s)" % value,
            "signed int": "PyLong_FromLong((long) %(val)s)" % value,
            "unsigned int": "PyLong_FromUnsignedLong((unsigned long) %(val)s)" % value,
            "long": "PyLong_FromLong(%(val)s)" % value,
            "signed long": "PyLong_FromLong(%(val)s)" % value,
            "unsigned long": "PyLong_FromUnsignedLong(%(val)s)" % value,
            "long long": "PyLong_FromLongLong(%(val)s)" % value,
            "signed long long": "PyLong_FromLongLong(%(val)s)" % value,
            "unsigned long long ": "PyLong_FromUnsignedLongLong(%(val)s)" % value,
            "float" : "PyFloat_FromDouble((double) %(val)s" % value,
            "double": "PyFloat_FromDouble(%(val)s" % value,
            "bool": "%(val)s?Py_True:Py_False",
        }[self.name]


class RecordTypeDefn(ScopedElement):

    def __init__(self, name, tag, parent, **kargs):
        super(RecordTypeDefn, self).__init__(name, tag, parent, **kargs)
        self._base_classes = {}

    def make_complete_by_attr_name(self, attr_name):
        self._name = "decltype(%(scope)s::%(name)s)" % {'scope': self.scope,
                                                        'name': attr_name}
        self._is_anonymous_type = True

    @property
    def full_name(self):
        if self.name.startswith("decltype("):
            return self.name
        else:
            return super(RecordTypeDefn, self).full_name

    @property
    def public_base_classes(self):
        return self._base_classes.get('public')

    def add_base_class(self, typ, access):
        self._base_classes.setdefault(access, []).append(typ)

    @property
    def is_structure(self):
        return True

    @property
    def is_scopable(self):
        return True


class CXXRecordDecl(RecordTypeDefn):

    def __init__(self, name, tag, parent, **kargs):
        super(CXXRecordDecl, self).__init__(name, tag, parent, **kargs)
        self._is_referenced = kargs.get('is_referenced') or False
        self._is_definition = kargs.get('is_definition') or False
        self._kind = kargs.get('structured_type')
        self._struct_type = kargs.get('structured_type')
        self._struct_type = self._struct_type.strip()

    def default_access(self):
        return "public" if self._struct_type != 'class' else "private"

    @property
    def is_union(self):
        return self._struct_type.strip() == 'union'

    @classmethod
    def parse_tokens(cls, name, tag, parent, **kargs):
        if kargs.get('implicit_explicit') == 'implicit':
            return None
        return CXXRecordDecl(name, tag, parent, **kargs)


class DecoratingType(UnscopedElement):

    def __init__(self, name, tag, parent, definition, qualifier=None,  locator=None):
        self._target_type = parse_type(self, definition, parent, var_name=name)
        super(DecoratingType, self).__init__(name, tag,
                                             self._target_type.parent,
                                             locator, qualifier=qualifier)

    @property
    def dependencies(self):
        type_parent = self._target_type.top_scope
        my_parent = self.top_scope
        if type_parent and type_parent != my_parent:
            self._dependencies.append(type_parent)

    @classmethod
    def parse_tokens(cls, name, tag, parent, **kargs):
        if 'definition' in kargs:
            kargs['definition'] = parse_type(None, kargs['definition'], parent, var_name=name)
        return cls(tag, parent, **kargs)

    @property
    def is_const(self):
        return 'const' in (self._qualifiers or [])

    @property
    def is_resolved(self):
        return not isinstance(self._target_type, UnresolvedElement) and self._target_type.is_resolved

    @property
    def parent(self):
        return self._target_type.parent if not self._parent else super(DecoratingType, self).parent

    def make_complete(self):
        pass

    def resolve(self):
        if not self.is_resolved:
            self._target_type = self._target_type.resolve()
            full_name = self.full_name
            if full_name in Element.lookup and type(Element.lookup[full_name]) != type(self):
                raise Exception("Duplicate type def")
            if full_name and full_name not in Element.lookup:
                Element.lookup[full_name] = self
        return self

    @property
    def name(self):
        if not self._target_type:
            return "%s<undetermined>" % self.__class__.__name__
        return super(DecoratingType, self).name

    @property
    def basic_name(self):
        if not self._target_type:
            return "%s<undetermined>" % self.__class__.__name__
        return super(DecoratingType, self).basic_name + (" " if self._qualifiers else "")

    @property
    def full_name(self):
        if not self._target_type:
            return "%s<undetermined>" % self.__class__.__name__
        if not self.is_resolved:
            self.resolve()
        return self._target_type.full_name + " " + self._qualifiers[0]
        #return self._target_type.scope + ('::' if self._target_type.scope else '') + super(DecoratingType, self).name
        # base_name = self._target_type.full_name
        # return base_name + (" " if self._qualifiers else "") + \
        #    " ".join(reversed(self._qualifiers))

    @property
    def scope(self):
        return self._target_type.scope

    @property
    def array_size(self):
        return None


class ArrayType(DecoratingType):

    def __init__(self, tag, parent, definition, array_size, locator=None):
        self._array_size = array_size
        super(ArrayType, self).__init__(definition.name + "[%s]" % array_size, tag, parent, definition=definition, qualifier=None, locator=locator)

    @property
    def array_size(self):
        return self._array_size

    @property
    def full_name(self):
        if not self._target_type:
            return "%s<undetermined>[%s]" % (self.__class__.__name__, self._array_size or "")
        if not self.is_resolved:
            self.resolve()
        return self._target_type.full_name + "[%s]" % (self._array_size or "")

class PointerType(DecoratingType):

    def __init__(self, tag, parent, definition, locator=None):
        super(PointerType, self).__init__(definition.name +'*', tag, parent, definition=definition, qualifier=None, locator=locator)

    @property
    def full_name(self):
        if not self._target_type:
            return "%s<undetermined>*" % self.__class__.__name__
        if not self.is_resolved:
            self.resolve()
        return self._target_type.full_name + "*"


class ReferenceType(DecoratingType):

    def __init__(self, tag, parent, definition, locator=None):
        super(ReferenceType, self).__init__(definition.name +'&', tag, parent,  definition=definition, qualifier=None, locator=locator)

    @property
    def full_name(self):
        if not self._target_type:
            return "%s<undetermined>" % self.__class__.__name__
        if not self.is_resolved:
            self.resolve()
        return self._target_type.full_name + "&"


class QualType(DecoratingType):

    def __init__(self, tag, parent, definition, qualifier=None, locator=None):
        if qualifier is None:
            raise Exception("Invalid qual type")
        #if isinstance(definition, (PointerType, ReferenceType)):
        name = definition.name + " " + qualifier
        #else:
        #    name = qualifier + " " + definition.name
        super(QualType, self).__init__(name, tag, parent, definition=definition, qualifier=qualifier, locator=locator)

    @property
    def full_param_name(self):
        # g++ has bug where signatures with const params removes the const-ness when checking template signatures,
        # to template params must not be const (but can be pointer or ref to const types)
        if 'const' in self._qualifiers or []:
            return self._target_type.full_name
        return super(QualType, self).full_name


def parse_type(element, definition, defining_scope, tag=None, var_name=None):
    if isinstance(definition, Element) or isinstance(definition, UnresolvedElement):
        return definition
    if not isinstance(definition, str):
        for defin in definition:
            try:
                if defin != 'struct':
                    return parse_type(element, defin, defining_scope, tag, var_name=var_name)
            except:
                pass
    array_size = None
    type_lexer.input(definition.replace("'", ''))
    implicit_explicit = None
    struct_type = None
    base_type = None
    base_qualifiers = [] # per-type-name qualifiers (const, volatile,...)
    base_modifiers = []  # post-type modifier (pointer or reference followed by other references or qualifiers)
    struct_type = None
    name = None
    while True:
        token = type_lexer.token()
        if token is None:
            break
        if token.type == 'name':
            name = token.value
            base_type = None
            if struct_type == 'enum' and (not token.value or token.value.endswith('::')):
                base_name = "enum " + defining_scope.tag
                name = name[:-2] + "::" +base_name
                if not Element.from_name(name):
                    base_type = EnumDecl(base_name, tag=defining_scope.tag, parent=defining_scope)
            elif token.value.endswith('::'):
                name = "decltype(%s::%s)" % (defining_scope.full_name, var_name)
            if not base_type:
                base_type = Element.from_name(name) or UnresolvedElement(name, defining_scope)
        elif token.type == 'array_spec':
            array_size = (array_size or [])
            array_size.append(token.value)
        elif token.type == 'implicit_explicit':
            implicit_explicit = token.value
        elif token.type in ['qualifier', 'reference'] :
            if base_type is None:
                base_qualifiers.append(token.value)
            else:
                base_modifiers.append(token.value)
        elif token.type == 'reference':
            assert base_type is not None
            base_modifiers.append(token.value)
        elif token.type == 'structured_type':
            struct_type = token.value
        elif token.type == 'is_enum':
            is_enum = True
        elif token.type == 'function_spec':
            base_type = FunctionTypeDecl(name=definition, tag=tag, parent=defining_scope, definition=token.value)
    if struct_type == 'enum' and not base_type:
        base_type = BuiltinType.DEFINITIONS['int']
    if name is None and struct_type and array_size:
        # anonymous array type (field)
        base_type = parse_type(element, struct_type, defining_scope=defining_scope, tag=tag, var_name=var_name)
    elif name is None and struct_type:
        base_type = AnonymousType(element)

    assert base_type is not None
    if array_size:
        for size in reversed(array_size):
            base_type = ArrayType(tag=None, parent=base_type.parent, definition=base_type, array_size=size)
    typ = base_type
    for qualifier in reversed(base_qualifiers):
        typ = QualType(tag=None, parent=base_type.parent, definition=typ, qualifier=qualifier)
    for modifier in base_modifiers:
        typ = {
            '*': PointerType(tag=None, parent=base_type.parent, definition=typ),
            '&': ReferenceType(tag=None, parent=base_type.parent, definition=typ),
        }.get(modifier) or QualType(tag=None, parent=base_type.parent, definition=typ, qualifier=modifier)
    return typ


class AnonymousType(ScopedElement):

    @classmethod
    def parse_tokens(cls, name, tag, parent, **kargs):
        raise Exception("Not parsable from tokens")

    def __init__(self, element, type_element=None):
        super(AnonymousType, self).__init__(name="decltype(%s::%s)" % (element.block_scope, element.basic_name),
                                            tag=None,
                                            parent=None,
                                            locator=None)

    @property
    def scope(self):
        return ""

    @property
    def block_scope(self):
        return ""

    @property
    def full_name(self):
        return self.name


class TypeAliasDecl(ScopedElement):

    def __init__(self, name, tag, parent, alias_definition=None, locator=None, definition=None):
        super(TypeAliasDecl, self).__init__(name, tag, parent, locator)
        if alias_definition:
            short_hand, full_decl = alias_definition
        else:
            short_hand, full_decl = definition, definition
        self._aliased_type = parent.find(short_hand)

    @property
    def dependencies(self):
        top_parent = self.top_scope
        type_parent = self._aliased_type.top_scope
        if type_parent and top_parent != type_parent:
            self._dependencies.apped(type_parent)


    @classmethod
    def parse_tokens(cls, name, tag, parent, **kargs):
        return TypeAliasDecl(name, tag, parent, **kargs)


class VarDecl(ScopedElement):

    def __init__(self, name, tag, parent, definition=None, alias_definition=None, qualifier=None, locator=None, implicit_explicit=None, is_referenced=None,
                 **kargs):
        super(ScopedElement, self).__init__(name if isinstance(name, str) else name[-1] if name else None, tag, parent,
                                            locator=locator, qualifier=qualifier)
        assert not (definition is None and alias_definition is None)
        assert definition is None or alias_definition is None
        if not definition:
            definition = alias_definition[1]
        self._type = parse_type(self, definition, parent, tag=tag)

    @property
    def dependencies(self):
        if self._type.top_scope and self._type.top_scope != self.top_scope:
            self._dependencies.append(self._type.top_scope)

    @property
    def type_(self):
        if self._type.name in ['enum']:
            self._type = AnonymousType(self, self._type)
        if isinstance(self._type, UnresolvedElement):
            self._type = self._type.resolve()
        return self._type

    @classmethod
    def parse_tokens(cls, name, tag, parent, **kargs):
        if name and name.startswith('used '):
            name = name.split(' ')[1]
        elif name and name.endswith(" cinit"):
            name = name[:-6]
        return cls(name, tag, parent, **kargs)

    @property
    def is_static(self):
        return 'static' in (self._qualifiers or [])


class ParmVarDecl(VarDecl):

    @property
    def full_name(self):
        return self.name


class FunctionElement(ScopedElement):

    def __init__(self, name, tag, parent, definition, locator=None, qualifier=None, **kargs):
        super(FunctionElement, self).__init__(name, tag, parent, locator=locator, qualifier=qualifier, **kargs)
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
        self._return_type = parse_type(self, definition.split('(')[0], parent) if return_type_name != 'void' else None
        # params added as ParamVarDecl's, except for ellipsis
        self._has_varargs = has_ellipsis
        self._throws = tokens.get('throws').throws if 'throws' in tokens else []

    @property
    def dependencies(self):
        if self._return_type and self._return_type.top_sccope and self._return_type.top_scope != self.top_scope:
            self._dependencies.append(self._return_type.top_scope)

    @property
    def is_static(self):
        return 'static' in (self._qualifiers or [])

    def add_child(self, element):
        if element._name != "void":
            super(FunctionElement, self).add_child(element)

    @classmethod
    def parse_tokens(cls, name, tag, parent, **kargs):
        return cls(name, tag, parent, **kargs)

    @property
    def return_type(self):
        return self._return_type

    @property
    def throws(self):
        return self._throws

    @property
    def params(self):
        return [c.resolve() for c in self.children('public') if isinstance(c.resolve(), ParmVarDecl)]

    def default_access(self):
        return 'public'

    @property
    def has_varargs(self):
        return self._has_varargs

    def as_function_argument(self, index, typed=False):
        name = self.name or "p%s" % index
        type_name = self._type.full_name
        return type_name.replace("(*)", "(*%s)" % name)

class BogusElement(ScopedElement):

    def __init__(self, *args, **kargs):
        super(BogusElement, self).__init__(*args)

    @classmethod
    def parse_tokens(cls, name, tag, parent, **kargs):
        return cls(name, tag, parent, **kargs)


class CompoundStmt(BogusElement):
    pass


class ReturnStmt(BogusElement):
    pass


class DeclStmt(BogusElement):
    pass


class ImplicitCastExpr(BogusElement):
    pass


class CXXNewExpr(BogusElement):
    pass


class DeclRefExpr(BogusElement):
    pass

class LinkageSpecDecl(BogusElement):
    pass

class FunctionDecl(FunctionElement):

    def __init__(self, name, tag, parent, definition, locator=None, qualifier=None, **kargs):
        super(FunctionDecl, self).__init__(name, tag, parent, definition, locator=locator, qualifier=qualifier, **kargs)


class FunctionTypeDecl(FunctionElement):

    def as_function_argument(self, name, typed=False):
        return "%s(*%s)(%s %s)" % (self.return_type.full_name,
                                    name,
                                    ", ".join([a.name for a in self.params]),
                                    "..." if self.has_varargs else "")

    def make_complete_by_attr_name(self, attr_name):
        self._name = "anonymous_func_decl_%s" % self.tag
        self._is_anonymous_type = True

    @property
    def full_name(self):
        return self.name


class CXXConstructorDecl(FunctionDecl):

    @property
    def guard(self):
        return super(CXXConstructorDecl, self).guard + "______constructor"


class CXXDestructorDecl(FunctionDecl):
    pass


class CXXMethodDecl(FunctionDecl):
    pass


class FieldDecl(ScopedElement):

    def __init__(self, name, tag, parent, **kargs):
        try:
            super(FieldDecl, self).__init__(name, tag, parent, **kargs)
            definition = kargs.get('definition') or ""
            if not isinstance(definition, str):
                definition = definition[0]
            if kargs.get('alias_definition'):
                # is anonymous type:
                self._type = Element.last_parsed_type
            elif 'struct' == definition or definition.endswith('::'):
                # anonymous type
                self._type = AnonymousType(self)
            else:
                self._type = parse_type(self, definition, parent, tag=tag, var_name=name)
            self._bit_size = None
        except:
            raise

    @property
    def dependencies(self):
        if self._type.top_scope and self._type.top_scope != self.top_scope and self._type.top_scope:
            self._dependencies.append(self._type.top_scope)

    @property
    def type_(self):
        if isinstance(self._type, UnresolvedElement):
            self._type = self._type.resolve()
        return self._type

    def set_integer_value(self, value):
        self._bit_size = int(value)

    @property
    def bit_size(self):
        return self._bit_size

    @property
    def parent_full_name(self):
        if self.parent.name:
            return self.parent.name


    @classmethod
    def parse_tokens(cls, name, tag, parent, **kargs):
        return FieldDecl(name, tag, parent, **kargs)

    def make_complete(self):
        super(FieldDecl, self).make_complete()
        if not self.name and self.parent and self.parent.parent:
            self._parent = self.parent.parent
            for child in self.children():
                self.parent.add_child(child)
                child._parent = self.parent
            self._children = {}
        if self.type_ and not self.type_.name and self.name:
            self.type_.make_complete_by_attr_name(self.name)


class IntegerLiteral(ScopedElement):

    def __init__(self, name, tag, parent, integer_value, locator=None, **kargs):
        super(IntegerLiteral, self).__init__(name, tag, parent, locator=locator, **kargs)
        if parent:
            parent.set_integer_value(integer_value)

    @classmethod
    def parse_tokens(cls, name, tag, parent, **kargs):
        return cls(name, tag, parent, integer_value=kargs.get('number'), **kargs)

    def make_complete(self):
        pass


class RecordType(RecordTypeDefn):

    def __init__(self, name, tag, parent, **kargs):
        super(RecordType, self).__init__(name, tag, parent, **kargs)
        tokens = {}
        if kargs.get('definition'):
            type_lexer.input(kargs.get('definition').replace("'", ""))
            while True:
                token = type_lexer.token()
                if token is None:
                    break
                if token.type in tokens:
                    tokens[token.type] = [tokens[token.type], token]
                else:
                    tokens[token.type] = token
        if tokens.get('structured_type'):
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


class CXXRecord(RecordType):

    @classmethod
    def parse_tokens(cls, name, tag, parent, **kargs):
        name = name or kargs.get('definition').replace("'", "")
        return CXXRecord(name, tag, parent)


class EnumDecl(RecordType):

    def __init__(self, name, tag, parent, **kargs):
        super(EnumDecl, self).__init__(name, tag, parent, **kargs)
        self._structured_type = 'structured_type' in kargs
        self._is_definition = True

    @classmethod
    def parse_tokens(cls, name, tag, parent, **kargs):
        return EnumDecl(name, tag, parent, **kargs)

    @property
    def is_structure(self):
        return self._structured_type


class EnumConstantDecl(VarDecl):

    def __init__(self, name, tag, parent, *args, **kargs):
        self._is_structure = parent.is_structure
        if not parent.name or not parent.is_structure:
            parent = parent.parent  # not an enum class, so must be added to enum's parent
        super(EnumConstantDecl, self).__init__(name, tag, parent, *args, **kargs)

    @property
    def is_structure(self):
        return self._is_structure

class TemplateDecl(ScopedElement):
    def __init__(self, name, tag, parent, locator=None, **kargs):
        super(TemplateDecl, self).__init__(name, tag, parent, locator=locator, **kargs)

    def find(self, type_name):
        if type_name in self._template_type_params:
            return self._template_type_params[type_name]
        return super(TemplateDecl, self).find(type_name)

    @property
    def template_args(self):
        return self._template_arguments

    @property
    def is_template(self):
        return True


class ClassTemplateDecl(TemplateDecl, RecordTypeDefn):

    def __init__(self, name, tag, parent, locator=None, **kargs):
        name = name + "_placeholder"
        super(ClassTemplateDecl, self).__init__(name, tag, parent, locator, **kargs)

    @property
    def basic_name(self):
        return super(ClassTemplateDecl, self).basic_name  # + "_placeholder"

    @property
    def name(self):
        return super(ClassTemplateDecl, self).basic_name  # + "_placeholder" # TODO: REMOVE + self.template_arguments_string()

    @classmethod
    def parse_tokens(cls, name, tag, parent, **kargs):
        return ClassTemplateDecl(name, tag, parent, **kargs)

    #@property
    #def full_name(self):
    #    return self.parent.name

    @property
    def is_template_macro(self):
        return True

    @property
    def guard(self):
        text = self.name + "__" + self.basic_name
        for c in [':', '<', '>', '!', '%', '^', '&', '*',  '[', ']', '\\', '|', '=', '(', ')', ',', ' ']:
            text = text.replace(c, '_')
        text =  "__PYLLARS__" + text
        return text

    @property
    def scope_fragment(self):
        return self.basic_name


class TemplateTypeParmDecl(ScopedElement):

    def __init__(self, name, tag, parent, structured_type=None, locator=None, is_referenced=False, **kargs):
        super(TemplateTypeParmDecl, self).__init__(name, tag, None, locator, **kargs)
        self._kind = structured_type
        self._is_referenced = is_referenced
        parent.add_template_arg(self)

    @classmethod
    def parse_tokens(cls, name, tag, parent, **kargs):
        return TemplateTypeParmDecl(name, tag, parent, **kargs)

    @property
    def is_type(self):
        return True

    @property
    def type_name(self):
        return "typename %s" % self.name

    def type_and_var_name(self, index):
        return self.type_name

    @property
    def full_name(self):
        return self.name

    def var_name(self, index):
        return self.name

    def py_var_name(self, index):
        return "(PyObject*)PythonClassWrapper< %s >::Type" % self.name


class NonTypeTemplateParmDecl(ScopedElement):

    def __init__(self, name, tag, parent: TemplateDecl, definition, locator=None, is_referenced=None,
                 **kargs):
        super(NonTypeTemplateParmDecl, self).__init__(name, tag, None, locator, **kargs)
        self._type = parse_type(self, definition, parent, var_name=name)
        #self._is_referenced = is_referenced
        parent.add_template_arg(self)

    @property
    def dependencies(self):
        if self._type.top_scope and self._type.top_scope != self.top_scope:
            return [self._type.top_scope]

    @classmethod
    def parse_tokens(cls, name: str, tag: str, parent: Element, **kargs):
        return NonTypeTemplateParmDecl(name, tag, parent, **kargs)

    @property
    def is_type(self):
        return False

    @property
    def type_name(self):
        return self._type.name

    def type_and_var_name(self, index):
        return self.type_name + " " + self.var_name(index)

    def pyllars_generic_argument(self , value):
        if isinstance(self._type, BuiltinType):
            return self._type.to_py_conversion_code(value)
        return "(PyObject*) __pyllars_internal::PythonClassWrapper< %(name)s >::Type" % self._type.name

    def var_name(self, index):
        return self.name or "_%s" % index

    def py_var_name(self, index):
        return "keyFrom(%s)" % self.name


class ClassTemplateSpecializationDecl(TemplateDecl, ScopedElement):

    def __init__(self, name, tag, parent, structured_type=None, locator=None, **kargs):
        self._base_name = name
        super(ClassTemplateSpecializationDecl, self).__init__("<<specialized %s @%s>>" %(name, tag), tag, parent, locator, **kargs)
        self._kind = structured_type
        self._base_template = parent.find(name)

    @classmethod
    def parse_tokens(cls, name, tag, parent, **kargs):
        return ClassTemplateSpecializationDecl(name, tag, parent, **kargs)

    @property
    def name(self)->str:
        children = self._children.get('public') or []
        suffix = "<%s>" % (", ".join([str(c._value) for c in children if c._value]))
        return self._base_name + suffix


class TemplateArgument(ScopedElement):

    def __init__(self, name, tag, parent, number=None, definition=None, locator=None, **kargs):
        self._value = None
        if name == 'type':
            self._value = parent.find(definition)
        elif name == 'expr':
            self._value = None
        elif "integral" == name:
            self._value = int(number)
        elif name=='nullptr':
            self._value = name
        else:
            raise Exception("Unknown TemplateArgument type: %s" % name)
        super(TemplateArgument, self).__init__(name, tag, parent, locator, **kargs)

    @classmethod
    def parse_tokens(cls, name, tag, parent, **kargs):
        return TemplateArgument(name, tag, parent, **kargs)


class TranslationUnitDecl(ScopedElement):

    @classmethod
    def parse_tokens(cls, name, tag, parent, **kargs):
        return TranslationUnitDecl(name, tag, parent, **kargs)


class TypedefDecl(ScopedElement):

    def __init__(self, name, tag, parent, **kargs):
        super(TypedefDecl, self).__init__(name, tag, parent, **kargs)

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
    NamespaceDecl.GLOBAL.add_child(BuiltinType("bool", NamespaceDecl.GLOBAL, None))

init()

def parse_files(src_paths: List[str], include_paths: List[str]):
    import subprocess
    import os.path
    Element.reset()
    import tempfile
    tmpd = tempfile.mkdtemp()
    includes = " ".join(['-I%s' % path for path in include_paths])
    try:
        with open(os.path.join(str(tmpd), "compile_commands.json"), mode='w') as f:
            f.write("""
                [
                """)
            for index, src_path in enumerate(src_paths):
                    f.write("""
                {
                   "directory": "%(dir)s",
                   "file": "%(file)s",
                   "command": "g++ -std=c++14 -c %(includes)s %(file)s"
                }
                """ % {'dir': os.path.dirname(src_path),
                       'file':os.path.basename(src_path),
                       'includes': includes})
                    if index != (len(src_paths) - 1):
                        f.write(',')
            f.write("""
            ]
            """)
            f.flush()
        cmd = ["clang-check", "-ast-dump"] + src_paths + ["--extra-arg=\"-fno-color-diagnostics\"", "-p", str(tmpd)]
        proc = subprocess.Popen(cmd, stdout=subprocess.PIPE)
        return Element.parse(proc.stdout, include_paths)
    finally:
        import shutil
        shutil.rmtree(str(tmpd))