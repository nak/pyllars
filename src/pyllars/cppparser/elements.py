import re
from abc import ABCMeta, abstractmethod
from typing import Union, List


class Element(metaclass=ABCMeta):

    GLOBAL = None  # set later
    RESERVED_QUALIFEIR_NAMES = ['const', 'volatile', 'mutable', 'constexpr']
    RESERVED_TYPE_DECLS = ['class', 'struct', 'union', ]
    RE_PREAMBLE = "(?P<offset>0x[^ ]*)  *(?P<loc_rang>\<[. ]*\>)? *(?P<range>[^ ]*:[^ ]*) *"
    RE_TYPE_PATTERN = "(?P<qual>(const[ ]*|volatile[ ]*|mutable[ ]*|constexpr[ ]*)*)(?P<type>class|struct|union)?[ ]*(?P<name>[^ ]*) *(?P<ref>(\*|\&|const |volatile |mutable )* *)"
    RE_TYPE_DEFINITION = RE_PREAMBLE + RE_TYPE_PATTERN
    RE_CLASS_DEFINITION = RE_PREAMBLE + "(?P<implicit>implicit)? *(?P<referenced>referenced)? *(?P<type>class|struct|union) *(?P<name>[^ ]*) *(?P<is_definition>definition)?"
    RE_METHOD_DEFINITION = RE_PREAMBLE + "(?P<operator>(operator)[^ ]*)? *'?" + \
        RE_TYPE_PATTERN + "(?P<params>\([ .]*\))[ ]*(?P<method_qual>)'?"
    lookup = {}

    def __init__(self, name, parent, locators=None):
        assert(not isinstance(name, bytes))
        self._name = name
        assert(not name or parent is not None)
        assert(parent is None or isinstance(parent, Element))
        self._parent = parent
        self._children = {}
        self._locators = locators
        Element.lookup[self.full_name] = self
        if parent:
            parent.add_child(self)

    @property
    def name(self):
        return self._name

    @property
    def parent(self):
        return self._parent

    @property
    def is_function(self):
        return False


    @classmethod
    @abstractmethod
    def parse_ast(cls, text: str, parent: Union["Element", None], locators: List[str], *args):
        pass

    @property
    def full_name(self):
        if self._parent and self._parent.name:
            return self._parent.full_name + "::" + self.name
        return self.name

    def add_child(self, element: "Element", access: Union[str, None]):
        self._children.setdefault(access, set([])).add(element)

    def default_access(self):
        return "private"

    def as_function_argument(self, element, index):
        return self.name


class UnknownElement(Element):

    def __init__(self, name, parent, locators, type_, text):
        super(UnknownElement, self).__init__(name, parent, locators)
        self._type = type_
        self._text = text

    @classmethod
    def parse_ast(cls, text, parent, locators, element_type):
        return UnknownElement("<<unknown>>", parent, locators, element_type , text)

    def __repr__(self):
        return "Unknown element:%s:   %s" % (self._type, self._text)


class BuiltinType(Element):

    def __init__(self, name):
        super(BuiltinType, self).__init__(name, NamespaceDecl.GLOBAL)

    @classmethod
    def parse_ast(cls, text, parent, locators):
        _, name = text.split(b' ', maxsplit=1)
        return BuiltinType(name.decode('latin-1').replace("'", ''))


class NamespaceDecl(Element):

    def __init__(self, name, parent):
        super(NamespaceDecl, self).__init__(name, parent)

    @classmethod
    def parse_ast(cls, text, parent, locators):
        _, name = text.rsplit(b' ', maxsplit=1)
        return NamespaceDecl(name.decode('latin-1'), parent)

    def __repr__(self):
        return "namespace " + self.full_name


class CXXRecordDecl(Element):

    def __init__(self, name, parent, kind, is_implicit, is_definition, locators):
        super(CXXRecordDecl, self).__init__(name, parent, locators=locators)
        self._kind = kind
        self._implicit_class = False
        self._implicit_referenced = False
        self._is_implicit = is_implicit
        self._is_definition = is_definition

    def set_implicit(self, referenced):
        self._implicit_class = not referenced
        self._implicit_referenced = referenced

    def default_access(self):
        return {'struct': "public", "union": "public", "class": "private"}[self._kind]

    @classmethod
    def parse_ast(cls, text, parent, locators):
        text = text.decode('latin-1').strip()
        parts = re.search(Element.RE_CLASS_DEFINITION, text).groupdict()
        locators = (parts.get('loc_range') or "") + (parts.get('range') or "")
        locators = locators.strip()
        kind = parts['type']
        is_implicit = parts.get('implicit') is not None
        is_definition = parts.get('is_definition') is not None
        name = parts['name']
        return CXXRecordDecl(name, parent, locators=locators,
                             kind=kind, is_implicit=is_implicit, is_definition=is_definition)

    def __repr__(self):
        return "%s %s" % (self._kind, self.full_name)


class ReferencedElement(Element):

    def __init__(self, ref_kind, base):
        self._base = base
        self._ref_kind = ref_kind


class Pointer(ReferencedElement):

    def __init__(self, base):
        super(Pointer, self).__init__(self, '*', base)


class Reference(ReferencedElement):

    def __init__(self, base):
        super(Reference, self).__init__(self, '&', base)


class MethodDeclElement(Element):

    def __init__(self, name, parent, return_type, locators=None):
        super(MethodDeclElement, self).__init__(name, parent, locators)
        self._return_type = return_type
        self._params = []  # set later

    @classmethod
    def parse_ast(cls, text, parent, locator, *args):
        text = text.decode('latin-1').strip()
        parts = re.search(Element.RE_METHOD_DEFINITION, text)
        name = parts.groupdict()['name']
        typ = parts.groupdict().get('type')
        reference_semantics = parts.groupdict().get('ref') or ""
        qualifiers = parts.groupdict().get('qual') or ""
        return_type = ("%s %s %s" % (qualifiers or "",
                                     name,
                                     reference_semantics or "")).strip()

        if return_type == "void":
            return_type = None
        elif return_type in Element.lookup:
            return_type = Element.lookup[return_type]
        elif name in Element.lookup:
            qualed_name = " ".join([qualifiers, name])
            if qualed_name in Element.lookup:
                name = qualed_name
            base = Element.lookup[name]
            for c in reference_semantics:
                base = {'*': Pointer, '&': Reference}[c](base)
        else:
            print("!!!!! Did not find type %s" % return_type)
        return cls(name, parent, return_type, [locator])

    def is_function(self):
        return True


class CXXMethodDecl(MethodDeclElement):
    pass


class FunctionElement(Element):

    def is_function(self):
        return True

NamespaceDecl.GLOBAL = NamespaceDecl("", None)