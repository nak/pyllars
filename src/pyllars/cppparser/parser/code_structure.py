import os
from abc import ABC, abstractmethod
from typing import Optional, List

from pyllars.cppparser.type_lexer import type_lexer


class Element(ABC):

    lookup = {}
    KEYWORDS = ['const', 'volatile', 'mutable']

    _last_location = None

    def __init__(self, tag: str, parent: "Element", name: Optional[str], qualifiers: List[str] = [],
                 loc1: Optional[str] = None,
                 loc2: Optional[str] = None):
        assert isinstance(parent, Element) or parent is None
        name = name.replace("'", "").strip() if name is not None else None
        self._tag = tag
        self._children = []
        self._inaccessible_children = []
        self._name = name
        self._loc1 = loc1
        self._loc2 = loc2
        fname = loc1.replace("<", "").replace(">", "").split(':')[0] if loc1 else ""
        if os.path.isfile(fname):
            self._loc1 = os.path.abspath(fname)
            Element._last_location = os.path.abspath(fname)
        elif (fname.startswith('line') or fname.startswith('col')) and self._last_location:
            # clang is f*ing lazt and we have to use the output of last given explicit path in tree
            # for location :-(
            self._loc1 = Element._last_location
        self._namespace = None
        self._parent = parent if not isinstance(parent, TypedefDecl) else parent.parent
        self._top_scope = None
        self._current_child_access = self.default_access()
        self._qualifiers = qualifiers
        self._full_name = None
        Element.lookup[tag] = self
        if name:
            full_name = (self.scope + '::' if self.scope else '') + name
            Element.lookup[full_name] = self
            if full_name.startswith('::'):
                Element.lookup[full_name[2:]] = self


    @property
    def is_template(self):
        return False

    @property
    def is_anonymous_type(self):
        return self._name is None

    @property
    def is_const(self):
        return 'const' in self._qualifiers

    @property
    def is_static(self):
        return 'static' in self._qualifiers

    @property
    def is_scoping(self):
        return False

    @property
    def is_typename(self):
        return False

    @property
    def location(self):
        return self._loc1.replace("<", "").replace(">", "").split(":")[0] if self._loc1 else ""

    def set_location(self, location):
        self._loc1 = os.path.abspath(location)
    
    @property
    def name(self):
        """
        :return: name of this element or None if anonymous
        """
        return self._name

    @property
    def safe_name(self):
        return self._name or "anonymous_%s" % self.tag

    @property
    def parent(self):
        return self._parent

    @property
    def tag(self):
        return self._tag
    
    @property
    def is_namespace(self):
        return False
    
    @property
    def is_const(self):
        return 'const' in (self._qualifiers or [])

    @property
    def is_constexpr(self):
        return 'constexpr' in (self._qualifiers or [])

    @property
    def full_name(self):
        """
        :return: fully scoped name of this type/element
        """
        if self._full_name is None:
            if not self.parent or isinstance(self.parent, TranslationUnitDecl):
                self._full_name = self.name or "anonymous_%s" % self.tag
            else:
                self._full_name = self.scope + ("::" if self.scope != '::' else "") + \
                                  (self.name or "anonymous_%s" % self.tag)
        return "::" + self._full_name

    @property
    def namespace(self):
        if not self._namespace:
            parent = self.parent
            while parent and not parent.is_namespace:
                parent = parent.parent
            self._namespace = parent
        return self._namespace

    @property
    def guard(self):
        """
        :return: c pre-compiler #ifdef macro name for this element
        """
        if self.parent and not isinstance(self.parent, TranslationUnitDecl):
            prefix = "__PYLLARS__" + self.parent.guard + "__"
        else:
            prefix = "__PYLLARS__"
        text = prefix + (self.name or "anonymous_%s" % self.tag)
        for c in [':', '<', '>', '!', '%', '^', '&', '*',  '[', ']', '\\', '|', '=', '(', ')', ',', ' ']:
            text = text.replace(c, '_')
        return text

    @property
    def full_param_name(self):
        """
        :return: name to use when specified as type parameter
        """
        return self.full_name

    @property
    def is_implicit(self):
        return 'implicit' in self._qualifiers

    @property
    def is_referenced(self):
        return 'referenced' in self._qualifiers

    @property
    def is_union(self):
        return False

    @property
    def array_size(self):
        return None

    @property
    def is_template_macro(self):
        return False

    @property
    def top_scope(self):
        """
        :return: top-level namespace that this element belongs to
        """
        if self._top_scope:
            return self._top_scope
        top = self.parent
        while top.parent:
            top = top.parent
        if top.is_namespace:
            return top
        return None

    @property
    def pyllars_scope(self):
        return "pyllars::" + (self.scope if self.parent else "")

    @property
    def scope(self):
        parent = self.parent
        basic_names = []
        while parent and not parent.is_scoping:
            parent = parent.parent
        while parent and parent.name:
            basic_names = [parent.name] + basic_names
            parent = parent.parent
        return "::".join(basic_names)

    @classmethod
    def default_access(cls):
        return "public"

    @property
    def has_varargs(self):
        return False

    @property
    def python_cpp_module_name(self):
        return "%s%s::%s_module()" % (self.pyllars_scope, self.name, self.name)

    def append_child(self, element: "Element"):
        assert(element is not None)
        if isinstance(element, AccessSpecDecl):
            self._current_child_access = element.name
        elif self._current_child_access == 'public':
            self._children.append(element)
        else:
            self._inaccessible_children.append(element)

    def __repr__(self):
        if self.is_anonymous_type:
            return "<<anonymous_%s>>" % self.tag
        return self.full_name

    def find(self, type_name):
        # look globally
        for child in self._children:
            if child.name == type_name:
                return child
        if type_name in Element.lookup:
            return Element.lookup[type_name]
        if "::" + type_name in Element.lookup:
            return Element.lookup["::" + type_name]
        if self.parent:
            # traverse parents to find type:
            return self.parent.find(type_name)

    def children(self) -> List["Element"]:
        return self._children

    def as_function_argument(self, index):
        """

        :param index:
        :param typed:
        :return:
        """
        return self.name if self.name else "param%s" % index

    @staticmethod
    def template_declaration(template_args):
        def template_parm_name(e):
            if isinstance(e, TemplateTypeParmDecl):
                return "typename %s" % e.name
            else:
                return "%s %s" % (e._type.name, e.name)

        return "" if not template_args else "template < %s >" % (", ".join([template_parm_name(e) for e in template_args]))

    def template_decl(self):
        return self.template_declaration(self._template_arguments)


class ScopedElement(Element):
    """
    """
    pass


class UnscopedElement(Element):

    @property
    def scope(self):
        return ""

    @property
    def block_scope(self):
        return None


class TranslationUnitDecl(Element):
    """
    """
    def __init__(self, *args: str, tag: str, parent: Optional[Element]):
        super().__init__(tag=tag, parent=parent, name=None, loc1=args[0], loc2=args[1])
        for name in ["void", "char", "signed char", "unsigned char", "short", "unsigned short", "signed short",
                     "int", "signed", "signed int", "unsigned int", "long", "signed long", "unsigned long",
                     "long long", "signed long long", "unsigned long long", "float", "double", "bool"]:
            BuiltinType(name, tag=name, parent=self)  # registers build in type

    @property
    def name(self):
        return None

    @property
    def full_name(self):
        return None

    @property
    def is_definition(self):
        return False


class NamespaceDecl(ScopedElement):

    def __init__(self, *args, tag: str, parent: Optional[Element]):
        super().__init__(tag=tag, parent=parent, name=args[2], loc1=args[0], loc2=args[1])

    @property
    def is_namespace(self):
        return True

    @property
    def is_scoping(self):
        return True

    @property
    def full_name(self):
        if self._full_name:
            return self._full_name
        return "::" + super().full_name

    @property
    def is_definition(self):
        return True


###########
#
###########


class AccessSpecDecl(UnscopedElement):

    def __init__(self, *args, tag: str, parent: Optional[Element]):
        super().__init__(tag=tag, parent=parent, name=args[-1])


##############
# Type-related ans structural definitions
##############


def _parse_type_spec(target_spec: str, element: Element):
    target_spec = target_spec.replace("'", "").replace("__restrict", "").replace("anonymous enum", "anonymous").strip()
    if target_spec in Element.lookup:
        return Element.lookup[target_spec]
    type_lexer.input(target_spec)
    target_type = None
    qualifiers = []
    tag = element.tag
    kind = None


    current_scope = None

    def find(element: Element, name: str):
        if element is None:
            return None
        for child in element.children():
            if child.name == name:
                return child
        raise Exception("Unknown scope: %s" % (current_scope.full_name + '::' + name))

    def scoping_element(element: Element):
        parent = element.parent
        while parent and not parent.is_scoping:
            parent = parent.parent
        return parent

    while True:
        token = type_lexer.token()
        if token is None:
            break
        if token.type == 'scope':
            current_scope = Element.lookup.get(token.value) if current_scope is None else find(current_scope, token.value)
            if not current_scope:
                current_scope = find(scoping_element(element), token.value)
            if not current_scope:
                raise Exception("Unknown scope: %s in this context" % token.value)
        elif token.type == 'name':
            if current_scope is not None:
                return_type = find(current_scope, token.value)
                if return_type:
                    target_type = return_type
            else:
                target_type = element.find(token.value)
                if not target_type and kind:
                    target_type = element.find(kind + " " + token.value)
            assert target_type is not None
            spec = token.value
            for index, qualifier in enumerate(reversed(qualifiers)):
                tag = "%s_%s" % (qualifier, tag)
                spec = "%s %s" % (qualifier, spec)
                qt = QualType(spec, qualifier=qualifier, tag=tag, parent=element)
                # qt._target_type = target_type
                qt.append_child(target_type)
                target_type = qt
            spec = target_type.name
            qualifiers = []
        elif token.type == 'qualifier':
            if target_type is None:
                qualifiers.append(token.value)
            else:
                tag += "_%s" % token.value
                spec = "%s %s" % (spec, token.value)
                qt = QualType(spec, qualifier=token.value, tag=tag, parent=element)
                qt.append_child(target_type)
                target_type = qt
        elif token.type == 'reference':
            assert target_type is not None
            tag += "_%s" % token.value
            spec = spec + token.value
            if token.value == '*':
                target_type = PointerType(spec, tag=tag, parent=element)
            elif token.value == '&':
                target_type = ReferenceType(spec, tag=tag, parent=element)
        elif token.type == 'structured_type':
            kind = token.value
    Element.lookup[target_spec] = target_type
    return target_type


class BuiltinType(Element):

    def __init__(self, *args: str, tag: str, parent: Optional[Element]):
        super().__init__(tag=tag, parent=parent, name=args[-1].replace("'", "").strip())
        print(">>>>>>>>>>> %s" % args[-1])

    @property
    def full_name(self):
        return self.name

    @property
    def python_type_name(self):
        if 'unsigned char' in self.name or 'int' in self.name or 'long' in self.name or 'short' in self.name:
            return "PyLong_Type"
        elif 'bool' in self.name:
            return "PyBool_Type"
        elif 'float' in self.name or 'double' in self.name:
            return "PyFloat_Type"
        elif 'char' in self.name:
            return "PyByte_Type"
        elif 'void' in self.name:
            return "PyNone_Type"
        else:
            raise Exception("Unknown python type for %s" % self.name)


class TypedefDecl(ScopedElement):

    def __init__(self, *args: str, tag: str, parent: Optional[Element]):
        if ':' in args:
            loc1, loc2, *args_, name, _, _, _ = args
        else:
            loc1, loc2, *args_, name, _ = args
        super().__init__(tag=tag, parent=parent, loc1=loc1, loc2=loc2, name=name,
                         qualifiers=args_)

    @property
    def target_type(self):
        if isinstance(self.children()[0], TypedefType):
            return self.children()[0].children()[1]
        return self.children()[0]


class TypedefType(ScopedElement):

    def __init__(self, *args: str, tag: str, parent: Optional[Element]):
        *type_specs, sugar = args
        name = args[0].replace("'", "")
        super().__init__(tag=tag, parent=parent, name=name)
        self._type_specs = type_specs
        self._is_sugar = sugar == 'sugar'


class Typedef(ScopedElement):

    def __init__(self, *args: str, tag: str, parent: Optional[Element]):
        type_specs = args
        name = args[0].replace("'", "")
        super().__init__(tag=tag, parent=parent, name=name)
        self._type_specs = type_specs


class RecordTypeDefn(Element):

    @property
    def is_definition(self):
        return True

    @property
    def is_scoping(self):
        return True

    @property
    def is_typename(self):
        return True


class RecordType(RecordTypeDefn):

    def __init__(self, *args: str, tag: str, parent: Optional[Element]):
        *args, name = args[0].replace("'", "").split()
        super().__init__(tag=tag, parent=parent, name=name)
        self._typ_specs = args


class CXXRecord(RecordTypeDefn):

    def __init__(self, *args: str, tag: str, parent: Optional[Element]):
        super().__init__(tag=tag, parent=parent, name=args[-1])


class CXXRecordDecl(RecordTypeDefn):

    def __init__(self, *args: str, tag: str, parent: Optional[Element]):
        if args[-1] == 'definition':
            if len(args) >= 5:
                loc1, loc2, *qualifiers, kind, name, definition = args
            else:
                loc1, loc2, *qualifiers, kind_or_name, definition = args
                if kind_or_name in ['struct', 'class']:
                    kind = kind_or_name
                    name = None
                else:
                    name = kind_or_name
        else:
            loc1, loc2, *qualifiers, kind, name = args
            definition = None
        super().__init__(tag=tag, parent=parent, name=name, loc1=loc1, loc2=loc2, qualifiers=qualifiers)
        Element.lookup[kind + " " + self.full_name] = self
        if self.full_name.startswith('::'):
            Element.lookup[kind + " " + self.full_name[2:]] = self
        self._kind = kind
        self._is_definition = definition == "definition"
        self._base_classes = []

    @property
    def base_classes(self):
        return self._base_classes

    def add_base_class(self, base_class: "CXXRecordDecl"):
        self._base_classes.append(base_class)

    @property
    def is_definition(self):
        return self._is_definition


class EnumDecl(ScopedElement):

    def __init__(self, *args: str, tag: str, parent: Optional[Element]):
        loc1, loc2, *args = args
        self._target_type = None
        self._target_type_spec = None
        if 'referenced' in args:
            args.remove('referenced')
        if args and args[0] == 'class':
            if len(args) == 3:
                self._target_type_spec = args[2]
            args = args[1:]
        name = args[0] if args else None
        self._definition = args[2] if len(args) > 2 else 'definition'
        super().__init__(tag=tag, parent=parent, name=name, loc1=loc1, loc2=loc2)
        self._base_classes = []

    @property
    def base_classes(self):
        return self._base_classes

    @property
    def is_definition(self):
        return self._definition == 'definition'

    @property
    def target_type(self):
        if self._target_type_spec and not self._target_type:
            self._target_type = _parse_type_spec(self._target_type_spec, self)
        return self._target_type

    @property
    def is_scoping(self):
        return bool(self.name)

    @property
    def is_typename(self):
        return True

    @property
    def full_name(self):
        element = self
        while element and not element.name:
            element = element.parent
        if not element or not element.name or isinstance(element, TranslationUnitDecl):
            return None
        if not self.name:
            return None
        return element.parent.full_name + "::" + self.name if element.parent.name else "::" + self.name


class EnumConstantDecl(Element):

    def __init__(self, *args: str, tag: str, parent: Optional[Element]):
        if 'referenced' in args:
            args = list(args)
            args.remove('referenced')
        loc1, loc2, name, *spec_info = args
        super().__init__(tag=tag, parent=parent, name=name, loc1=loc1, loc2=loc2)
        spec_info[0] = spec_info[0].replace("'", "")
        self._spec_info = spec_info
        if spec_info and spec_info[0].startswith('enum ('):
            Element.lookup[spec_info[0]] = parent
        self._target_type_spec = None
        self._target_type = None
        self._full_name = None

    @property
    def target_type(self):
        if self._target_type_spec is None:
            return None
        if self._target_type is None:
            self._target_type = _parse_type_spec(self._target_type_spec, self)
        return self._target_type

    @property
    def full_name(self):
        parent = self.parent
        while parent and not parent.name:
            parent = parent.parent
        if not parent or isinstance(parent, TranslationUnitDecl):
            return "::" + self.name
        return  parent.full_name + "::" + self.name

    @property
    def python_cpp_module_name(self):
        if not self.name:
            return "PyImport_ImportModule(\"pyllars\")"
        return super().python_cpp_module_name


class FieldDecl(ScopedElement):

    def __init__(self, *args: str, tag: str, parent: Optional[Element]):
        if ':' in args:
            loc1, loc2, name, typ, _, basic_type = args
        elif len(args) == 4:
            loc1, loc2, name, typ = args
            basic_type = None
        else:
            loc1, loc2, typ = args
            name = None
            basic_type = None
        super().__init__(tag=tag, parent=parent, name=name, loc1=loc1, loc2=loc2)
        self._target_typespec = typ.replace("'", "").strip()
        self._target_type = None
        self._basic_type = basic_type
        self._bit_size = None
        self._array_size = 1;

    @property
    def target_type(self):
        if self._target_type is None:
            self._target_type = _parse_type_spec(self._target_typespec, self)
        return self._target_type

    @property
    def bit_size(self):
        return self._bit_size


class VarDecl(ScopedElement):

    def __init__(self, *args: str, tag: str, parent: Optional[Element]):
        loc1, loc2, name, type_spec, *qualifiers = args
        type_spec = type_spec.replace("'", "")
        super().__init__(tag=tag, parent=parent, name=name, loc1=loc1, loc2=loc2)
        self._type_spec = type_spec
        self._target_type = None
        self._qualifiers = qualifiers

    @property
    def type_spec(self):
        return self._type_spec

    @property
    def target_type(self):
        if self._target_type is None:
            self._target_type = Element.lookup.get(self._type_spec) or _parse_type_spec(self._type_spec, self)
        return self._target_type


class ConstAttr(ScopedElement):

    def __init__(self, *args: str, tag: str, parent: Optional[Element]):
        super().__init__(tag=tag, parent=parent, name=None, loc1=args[0])


class NonNullAttr(ScopedElement):

    def __init__(self, *args: str, tag: str, parent: Optional[Element]):
        super().__init__(tag=tag, parent=parent, name=None, loc1=args[0])


class ElaboratedType(Element):

    def __init__(self, *args: str, tag: str, parent: Optional[Element]):
        typ, sugar = args
        typ = typ.replace("'", "")
        name = typ.split()[-1]
        super().__init__(tag=tag, parent=parent, name=name)
        self._is_sugar = sugar == 'sugar'


##############
# Accessor and Decorated Types
##############


class _DecoratingType(UnscopedElement):

    def __init__(self, tag: str, parent: Optional[Element], qualifier: str, target_spec: str):
        super().__init__(tag=tag, parent=parent, name=target_spec)
        self._qualifier = qualifier
        self._qualifiers = [qualifier]
        self._target_spec = target_spec[:-1]
        self._target_type = None

    @property
    def target_type(self):
        if self._children:
            raise Exception()  #return self.children()[0]
        elif not self._target_type:
            self._target_type = _parse_type_spec(self._target_spec.strip(), self)
        return self._target_type

    @property
    def full_name(self):
        return self.target_type.full_name + ' ' + self._qualifier


class PointerType(_DecoratingType):

    def __init__(self, *args, tag: str, parent: Optional[Element]):
        type_spec = args[-1].replace("'", "")
        ref = type_spec[-1]
        assert ref == '*'
        super().__init__(tag=tag, parent=parent, qualifier=ref, target_spec=type_spec)


class ReferenceType(_DecoratingType):

    def __init__(self, *args, tag: str, parent: Optional[Element]):
        type_spec = args[-1].replace("'", "")
        ref = type_spec[-1]
        assert ref == '&'
        super().__init__(tag=tag, parent=parent, qualifier=ref, target_spec=type_spec)


class ConstantArrayType(_DecoratingType):

    def __init__(self, *args, tag: str, parent: Optional[Element]):
        type_spec, ref = args[0].replace("'", "").rsplit(" ", 1)
        assert ref[0] == '[' and ref[-1] == ']'
        self._array_size = int(args[1])
        super().__init__(tag=tag, parent=parent, qualifier=ref, target_spec=type_spec)

    @property
    def array_size(self):
        return self._array_size


class QualType(Element):

    def __init__(self, *args: str, tag: str, parent: Optional[Element], qualifier=None):
        if not qualifier:
            if args[-1] in ['*', '&', '&&', '[]'] + self.KEYWORDS:
                qualifier = args[-1]
                args = args[:-1]
            else:
                qualifier = args[0]
                args = args[1:]
        type_spec = " ".join(args).replace("'", "").strip()
        super().__init__(tag=tag, parent=parent, name=type_spec)
        self._qualifier = qualifier
        if '*' not in type_spec and '&' not in type_spec and type_spec.startswith('const'):
            type_spec = type_spec[5:]
        else:
            assert type_spec.endswith('const')
        self._target_type_spec = type_spec[:-5].strip()
        self._target_type = None

    @property
    def target_type(self):
        if self.children():
            return self.children()[0]
        if not self._target_type:
            self._target_type = _parse_type_spec(self._target_type_spec, self)
        return self._target_type

    @property
    def qualifier(self):
        return self._qualifier

    @property
    def name(self):
        return self.target_type.name + ' ' + self._qualifier

    @property
    def full_name(self):
        return self.target_type.full_name + ' ' + self._qualifier

    @property
    def is_const(self):
         return 'const' == self.qualifier




#################
# literals
#################
class UnaryOperator(Element):
    def __init__(self, *args: str, tag: str, parent: Optional[Element]):
        super().__init__(tag=tag, parent=parent, name=None, loc1=args[0])


class IntegerLiteral(Element):

    def __init__(self, *args: str, tag: str, parent: Optional[Element]):
        super().__init__(tag=tag, parent=parent, name=None, loc1=args[0])
        self._typ = args[1]
        self._value = int(args[2])

    @property
    def typ(self):
        return self._typ

    @property
    def value(self):
        return self._value


class FloatingLiteral(Element):

    def __init__(self, *args: str, tag: str, parent: Optional[Element]):
        super().__init__(tag=tag, parent=parent, name=None, loc1=args[0])
        self._typ = args[1]
        self._value = float(args[2])

    @property
    def typ(self):
        return self._typ

    @property
    def value(self):
        return self._value


class ImplicitValueInitExpr(Element):

    def __init__(self, *args: str, tag: str, parent: Optional[Element]):
        loc1, name, *spec = args
        super().__init__(tag=tag, parent=parent, name=name, loc1=loc1)
        self._spec = [s for s in spec if s != ':']


###################
# Functions and methods
###################

class _Function(ScopedElement):

    def __init__(self, *args: str, tag: str, parent: Optional[Element]):
        loc1, loc2, name, signature, *qualifiers = args
        super().__init__(tag=tag, parent=parent, name=name, loc1=loc1, loc2=loc2, qualifiers=qualifiers)
        self._signature = signature.replace("'", "")
        self._params = None  # set on-demand
        self._return_type_spec = self._signature.split('(', 1)[0].strip()
        self._return_type = self.find(self._return_type_spec)
        self._throws = None
        if "throw" in signature:
            throws = signature.split("throw", 1)[-1].replace(')', '').replace('(', '').replace("'", '').strip()
            self._throws = throws.split(',')

    @property
    def throws(self):
        return self._throws

    @property
    def params(self):
        if self._params is None:
            self._params = [child for child in self.children() if isinstance(child, ParmVarDecl)]
        return self._params

    @property
    def return_type_spec(self):
        return self._return_type_spec

    @property
    def return_type(self):
        if not self._return_type and self._return_type_spec != 'void':
            self._return_type = _parse_type_spec(self._return_type_spec, self)
        return self._return_type


class FunctionDecl(_Function):
    pass


class CXXConstructorDecl(_Function):
    pass


class CXXMethodDecl(_Function):
    pass


class ParmVarDecl(ScopedElement):

    def __init__(self, *args: str, tag: str, parent: Optional[Element]):
        if ':' in args:
            args = args[:args.index(':')]
        loc1, loc2, name, type_spec, *qualifiers = args
        super().__init__(tag=tag, parent=parent, name=name, loc1=loc1, loc2=loc2, qualifiers=qualifiers)
        self._type_spec = type_spec.replace("\'", "")
        self._target_type = None

    @property
    def target_type(self):
        if self._target_type is None:
            self._target_type = _parse_type_spec(self._type_spec, self)
        return self._target_type

    @property
    def full_name(self):
        return self.name

###################
# Linkage
###################

class LinkageSpecDecl(Element):

    def __init__(self, *args: str, tag: str, parent: Optional[Element]):
        super().__init__(tag=tag, parent=parent, name=None, loc1=args[0], loc2=args[1])
        self._linkage_type = args[2]


################
# misc
################

class CompoundStmt(Element):

    def __init__(self, *args: str, tag: str, parent: Optional[Element]):
        super().__init__(tag=tag, parent=parent, name=None, loc1=args[0])


class ReturnStmt(Element):

    def __init__(self, *args: str, tag: str, parent: Optional[Element]):
        super().__init__(tag=tag, parent=parent, name=None, loc1=args[0])


class CXXCtorInitializer(Element):
    def __init__(self, *args: str, tag: str, parent: Optional[Element]):
        super().__init__(tag=tag, parent=parent, name=None, loc1=args[0])


class CStyleCastExpr(Element):

    def __init__(self, *args: str, tag: str, parent: Optional[Element]):
        loc1, cast_type, *qualifiers = args
        super().__init__(tag=tag, parent=parent, name=None, qualifiers=qualifiers)
        self._cast_type = cast_type


class ImplicitCastExpr(Element):

    def __init__(self, *args: str, tag: str, parent: Optional[Element]):
        loc1, cast_type, *qualifiers = args
        super().__init__(tag=tag, parent=parent, name=None, qualifiers=qualifiers)
        self._cast_type = cast_type


class DeclRefExpr(Element):

    def __init__(self, *args: str, tag: str, parent: Optional[Element]):
        loc1, spec, typ, typid, name, spec2, *_ = args
        super().__init__(tag=tag, parent=parent, name=name)
        self._spec = spec
        self._target_type = typ
        self._target_typeid = typid
        self._spec2 = spec2

#####################
# Templates
#####################


class ClassTemplateDecl(ScopedElement):

    def __init__(self, *args: str, tag: str, parent: Optional[Element]):
        raise Exception("Not implemented")

    @property
    def is_scoping(self):
        return True
