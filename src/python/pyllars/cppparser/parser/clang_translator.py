import os
from abc import abstractmethod, ABC
from typing import Optional, Iterator, List, IO, AnyStr

from dataclasses import dataclass, field
import logging

log = logging.getLogger(__file__)


class MismatchedDepth(BaseException):
    def __init__(self, line: str):
        self._line = line

    @property
    def line(self):
        return self._line


class NodeType:

    @staticmethod
    def _line_splitter(text: str):
        nb_angle_brackets = 0
        nb_quote = 0
        text = text.strip()
        words = []
        word_start = 0
        i = 0
        prev_c = None
        for i, c in enumerate(text):
            if nb_quote == 0 and c == "'":
              nb_quote += 1
              word_start = i +1
              prev_c = c
              continue
            elif nb_quote == 0 and c == '<' and prev_c in [None, ' ']:
                nb_angle_brackets += 1
            elif nb_angle_brackets > 0 and c == '>':
                nb_angle_brackets -=1
            elif nb_quote == 0 and c == ' ' and nb_angle_brackets == 0:
                word = text[word_start: i].strip()
                if word:
                    words.append(word)
                word_start = i
            elif c == "'" and nb_quote > 0:
                nb_quote -= 1
                if nb_quote == 0:
                    word = text[word_start: i]
                    words.append(word)
                    word_start = i + 1
                prev_c = c
                continue
            prev_c = c
        word = text[word_start: i+1].strip()
        if word:
            words.append(word)
        return words


    @dataclass
    class Node(ABC):

        @classmethod
        def process(cls, lines: IO, parent=None, indent=0):
            def process(text, node_type):
                words = NodeType._line_splitter(text)
                if 'parent' in words:
                    index = words.index('parent', 0)
                    for i in range(index+2, len(words)):
                        words[i-2] = words[i]
                    words = words[:-2]
                    raise Exception("Unable to process sporadically defined type: %s" % words)
                try:
                    return node_type(*words)
                except Exception as e:
                    log.error(f"Failed to process {node_type} with args '{words}': " + str(e))
                    raise e

            line = next(lines, None)
            if not line:
                return parent
            line = line.rstrip()
            print(line)
            if parent == None:
                tag, text = line.split(' ', maxsplit=1)
                node_type = getattr(NodeType, tag)
                node = process(text, node_type)
                if node:
                    cls.process(lines, parent=node, indent=0)
                return node
            else:
                current_node = parent
                while line:
                    prefix, substance = line.split('-', maxsplit=1)
                    this_indent = (len(prefix)+1)//2
                    if this_indent > indent + 2:
                        # too many indents
                        raise SyntaxError("Invalidly formatted file, unexpected tree depth")
                    elif this_indent == indent + 2:
                        # have new child
                        if ' ' not in substance:
                            substance += ' '
                        tag, text = substance.split(' ', maxsplit=1)
                        node_type = getattr(NodeType, tag)
                        node = process(text, node_type)
                        try:
                            assert isinstance(node, NodeType.Node)
                            current_node.children.append(node)
                            node.parent = current_node
                        except AttributeError:
                            raise Exception(
                                f"Invalid file format: attempt to append child to non-composite node: {type(current_node).__name__}")
                        try:
                            cls.process(lines, parent=node, indent=this_indent)
                            current_node = node
                        except MismatchedDepth as e:
                            line = e.line.rstrip()
                        else:
                            line = next(lines, None)
                            if line:
                                line = line.rstrip()
                                print(line)
                    elif this_indent == indent+1:
                        # at same level as previous item
                        if ' ' in substance:
                            tag, text = substance.split(' ', maxsplit=1)
                        else:
                            tag = substance
                            text = ''
                        node_type = getattr(NodeType, tag)
                        try:
                            node = process(text, node_type)
                        except MismatchedDepth as e:
                            line = e.line.rstrip()
                            current_node = parent
                            continue
                        try:
                            assert isinstance(node, NodeType.Node)
                            parent.children.append(node)
                            if not isinstance(parent, NodeType.TranslationUnitDecl):
                                node.parent = parent
                        except AttributeError:
                            raise Exception(f"Invalid file format: attempt to append child to non-composite node: {type(parent).__name__}")
                        current_node = node
                        if line[indent] == '`':
                            try:
                                cls.process(lines, parent=node, indent=this_indent)
                            except MismatchedDepth as e:
                                pass
                            break
                        line = next(lines, None)
                        if line:
                            line = line.rstrip()
                            print(line)
                    else:
                        raise MismatchedDepth(line)

        def serialize(self, prefix: str):
            """
            convert to string
            :param depth: depth in hieararchy
            :return: string representation
            """
            yield self.to_str(prefix)
            if hasattr(self, "children"):
                for index, child in enumerate(self.children):
                    prefix2 = prefix.replace('-', ' ').replace('`', ' ')
                    char = "|" if index + 1 < len(self.children) else "`"
                    prefix2 += char + "-"
                    assert(isinstance(child, NodeType.Node))
                    for line in child.serialize(prefix2):
                        yield line

        @abstractmethod
        def to_str(self, prefix):
            pass

        @property
        def full_cpp_name(self):
            parent = self.parent
            prefix = ""
            while parent:
                prefix = f"{parent.name}::" + prefix
                parent = parent.parent
            return prefix + self.name

        @property
        def namespace_name(self):
            parent = self.parent
            prefix = ""
            while parent:
                if isinstance(parent, NodeType.NamespaceDecl):
                    prefix = f"{parent.name}" + prefix if prefix else f"{parent.name}"
                parent = parent.parent
            return prefix

    @dataclass
    class LeafNode(Node):
        node_id: str
        parent: Optional["NodeType.Node"]

        def __init__(self, node_id: str, parent: Optional["NodeType.Node"] = None):
            self.node_id = node_id
            self.parent = parent

    @dataclass
    class InheritanceNode(LeafNode):

        clazz: str

        def __init__(self, clazz: str, parent: Optional["NodeType.Node"] = None):
            super().__init__(node_id=None)
            self.clazz = clazz

        def to_str(self, prefix):
            return " ".join([prefix + self.__clazz, self.node_id])

        @property
        def full_name(self):
            return self.clazz

        @property
        def name(self):
            return self.clazz.split('::')[-1]


    @dataclass
    class public(InheritanceNode):

        def __init__(self, clazz: str, parent: Optional["NodeType.Node"] = None):
            super().__init__(clazz=clazz, parent=parent)


    @dataclass
    class protected(InheritanceNode):

        def __init__(self, clazz: str, parent: Optional["NodeType.Node"] = None):
            super().__init__(clazz=clazz, parent=parent)


    @dataclass
    class private(InheritanceNode):

        def __init__(self, clazz: str, parent: Optional["NodeType.Node"] = None):
            super().__init__(clazz=clazz, parent=parent)


    @dataclass
    class LocationNode(LeafNode):
        line_loc: str
        col_loc: str

        def __init__(self, node_id: str, line_loc: str, col_loc: str):
            super().__init__(node_id)
            self.line_loc = line_loc
            self.col_loc = col_loc


    @dataclass
    class CompositeNode(LocationNode):
        children: List["NodeType.Node"]

        def __init__(self, node_id: str, line_loc: str, col_loc: str):
            super().__init__(node_id, line_loc, col_loc)
            self.children = []

        def __iter__(self):
            return iter(self.children)

        def __next__(self):
            return next(iter(self.children))


    @dataclass
    class TranslationUnitDecl(CompositeNode):
        def __init__(self, *args):
            super().__init__(*args)

        def to_str(self, prefix: str):
            return " ".join([prefix + self.__class__.__name__, self.node_id, self.line_loc, self.col_loc])

        def by_header_file(self):
            by_header = {}
            file_name = "  unknown  "
            for child in self:
                if hasattr(child, 'line_loc'):
                    possible_file_name = child.line_loc[1:].split(':')[0]
                    if os.path.exists(possible_file_name):
                        file_name = possible_file_name
                    elif possible_file_name.startswith('line'):
                        pass
                    else:
                        file_name = "  unkonwn  "
                    by_header.setdefault(file_name, []).append(child)
            return by_header


    @dataclass
    class TypeNode(LeafNode):
        type_text: str

        def __init__(self, node_id: str, type_text: str):
            super().__init__(node_id)
            self.type_text = type_text

    @dataclass
    class CompositeTypeNode(TypeNode):
        children: List["NodeType.Node"]

        def __init__(self, node_id: str, type_text: str):
            super().__init__(node_id, type_text)
            self.children = []

        def __iter__(self):
            return iter(self.children)

        def __next__(self):
            return next(iter(self.children))

        def to_str(self, prefix):
            return " ".join([prefix + self.__class__.__name__, self.node_id, f"'{self.type_text}'"])


    @dataclass
    class TypedefDecl(LocationNode):
        line_loc: str
        col_loc: str
        name: str
        target_name: str
        children: "NodeType.TypeNode"

        def __init__(self, node_id: str, line_loc: str, col_loc: str, *args):
            super().__init__(node_id, line_loc, col_loc)
            self.qualifiers = []
            arg_iter = iter(args)
            arg = next(arg_iter)
            while arg in ['implicit', 'explicit']:
                self.qualifiers.append(arg)
                arg = next(arg_iter)
            self.name = arg
            self.target_name = next(arg_iter)
            self.children = []

        def to_str(self, prefix: str):
            if self.qualifiers:
                return " ".join([prefix + self.__class__.__name__, self.node_id, self.line_loc, self.col_loc, " ".join(self.qualifiers),
                               self.name, f"'{self.target_name}'"])
            else:
                return " ".join([prefix + self.__class__.__name__, self.node_id, self.line_loc, self.col_loc,
                                self.name, f"'{self.target_name}'"])

    @dataclass
    class BuiltinType(TypeNode):

        def __init__(self, node_id: str, type_text: str):
            super().__init__(node_id, type_text)

        def to_str(self, prefix):
            return " ".join([prefix + self.__class__.__name__, self.node_id, f"'{self.type_text}'"])


    @dataclass
    class RecordType(CompositeTypeNode):
        def __init__(self, node_id: str, type_text: str):
            super().__init__(node_id, type_text)


    @dataclass
    class CXXRecord(CompositeTypeNode):
        def __init__(self, node_id: str, type_text: str):
            super().__init__(node_id, type_text)


    @dataclass
    class ElbaoratedType(TypeNode):
        qualifiers: Optional[str]


    @dataclass
    class PointerType(CompositeTypeNode):
        def __init__(self, *args):
            super().__init__(*args)

    @dataclass
    class TypedefType(TypeNode):
        qualifiers: Optional[str]


    @dataclass
    class Typedef(TypeNode):
        pass


    @dataclass
    class QualType(CompositeTypeNode):
        qualifier: str

        def __init__(self, node_id: str, type_text: str, qualifier: str):
            super().__init__(node_id, type_text)
            self.qualifier = qualifier

        def to_str(self, prefix):
            return " ".join([prefix + self.__class__.__name__, self.node_id, f"'{self.type_text}'", self.qualifier])


    @dataclass
    class ReferenceType(CompositeTypeNode):
        pass


    @dataclass
    class CXXRecordDecl(CompositeNode):

        def __init__(self, node_id, line_loc, col_loc, *args):
            super().__init__(node_id, line_loc, col_loc)
            arg_iter = iter(args)
            arg = next(arg_iter, None)
            self.qualifiers = []
            while arg in ['implicit', 'class', 'struct', 'referenced']:
                self.qualifiers.append(arg)
                arg = next(arg_iter, None)
            assert(arg is not None)
            self.name = arg
            self.post_qualifires = []
            arg = next(arg_iter, None)
            while arg:
                self.post_qualifires.append(arg)
                arg = next(arg_iter, None)

        def normalize(self):
            access = 'private'
            for field in [f for f in self.children if isinstance(f, (NodeType.AccessSpecDecl, NodeType.VarDecl, NodeType.FieldDecl)) ]:
                if isinstance(field, NodeType.AccessSpecDecl):
                    access = field.access
                else:
                    field.qualifiers.append(access)

        @property
        def bases(self):
            return [child for child in self.children if isinstance(child, NodeType.public)]


        def to_str(self, prefix: str):
            return " ".join([prefix + self.__class__.__name__, self.node_id, self.line_loc, self.col_loc, " ".join(self.qualifiers),
                            self.name, " ".join(self.post_qualifires)])


    @dataclass
    class EnumDecl(CompositeNode):

        def __init__(self, node_id, line_loc, col_loc, *args):
            super().__init__(node_id, line_loc, col_loc)
            arg_iter = iter(args)
            arg = next(arg_iter, None)
            self.qualifiers = []
            while arg in ['implicit', 'class', 'struct', 'referenced']:
                self.qualifiers.append(arg)
                arg = next(arg_iter, None)
            self.name = arg if arg is not None else ""
            self.post_qualifires = []
            arg = next(arg_iter, None)
            while arg:
                self.post_qualifires.append(arg)
                arg = next(arg_iter, None)

        def to_str(self, prefix: str):
            return " ".join([prefix + self.__class__.__name__, self.node_id, self.line_loc, self.col_loc, " ".join(self.qualifiers),
                            self.name, " ".join(self.post_qualifires)])


    @dataclass
    class EnumConstantDecl(LeafNode):
        col_id: str
        name: str
        parent_type_name: str

        def __init__(self, node_id: str, line_loc: str,  col_loc: str, name: str, parent_type_name: str):
            super().__init__(node_id=node_id)
            self.line_loc = line_loc
            self.col_loc = col_loc
            self.name = name
            self.parent_type_name = parent_type_name

        def to_str(self, prefix: str):
            return " ".join([prefix + self.__class__.__name__, " ".join(self._data)])


    @dataclass
    class DefinitionData(Node):
        children: List['NodeType.Node']

        def __init__(self, *args):
            super().__init__()
            self._data = args
            self.children = []

        def to_str(self, prefix: str):
            return " ".join([prefix + self.__class__.__name__, " ".join(self._data)])


    @dataclass
    class CXXConstructorDecl(CompositeNode):
        signature: str

        def __init__(self, node_id: str, line_loc: str, col_loc: str, name: str, signature: str, *args: str):
            super().__init__(node_id, line_loc, col_loc)
            self.signature = signature
            self.children = []
            self.name = name
            self.keywords = args

        def to_str(self, prefix):
            return " ".join([prefix + self.__class__.__name__, self.node_id, self.line_loc, self.col_loc,
                            self.name, f"'{self.signature}'"])


    @dataclass
    class CXXCtorInitializer(CompositeNode):

        def __init__(self, node_id: str, line_loc: str, col_loc: str, name: str, *args: str):
            super().__init__(node_id, line_loc, col_loc)
            self.children = []
            self.name = name
            self.keywords = args

        def to_str(self, prefix):
            return " ".join([prefix + self.__class__.__name__, self.node_id, self.line_loc, self.col_loc,
                            self.name])

    @dataclass
    class BasicDefaultConstructor(Node):
        classifiers: List[str]

        def __init__(self, *args):
            super().__init__()
            self.classifiers = args

        def to_str(self, prefix: str):
            return " ".join([prefix + self.__class__.__name__, " ".join(self.classifiers)])


    @dataclass
    class DefaultConstructor(BasicDefaultConstructor):

        def __init__(self, *args):
            super().__init__( *args)
            self.classifiers = args


    @dataclass
    class CopyConstructor(BasicDefaultConstructor):

        def __init__(self, *args):
            super().__init__(*args)


    @dataclass
    class MoveConstructor(BasicDefaultConstructor):

        def __init__(self, *args):
            super().__init__(*args)


    @dataclass
    class CXXDestructorDecl(LeafNode):
        line_loc : str
        col_loc : str
        signature: str
        qualifiers: List[str]

        def __init__(self, node_id, line_loc, col_loc, name, signature, *args):
            self.node_id = node_id
            self.name = name
            self.line_loc = line_loc
            self.col_loc = col_loc
            self.signature = signature
            self.qualifiers = args

        def to_str(self, prefix):
            return " ".join([prefix + self.__class__.__name__, self.node_id, self.line_loc, self.col_loc, self.name, f"'{self.signature}'",
                            " ".join(self.qualifiers)])


    @dataclass
    class CopyAssignment(Node):
        classifiers: List[str]

        def __init__(self, *args):
            self.classifiers = args

        def to_str(self, prefix: str):
            return " ".join([prefix + self.__class__.__name__, " ".join(self.classifiers)])


    @dataclass
    class MoveAssignment(Node):
        classifiers: List[str]

        def __init__(self, *args):
            self.classifiers = args

        def to_str(self, prefix: str):
            return " ".join([prefix + self.__class__.__name__, " ".join(self.classifiers)])


    @dataclass
    class Destructor(Node):
        classifiers: List[str]

        def __init__(self, *args):
            self.classifiers = args

        def to_str(self, prefix: str):
            return " ".join([prefix + self.__class__.__name__, " ".join(self.classifiers)])

    @dataclass
    class CXXMethodDecl(CompositeNode):
        name: str
        signature: str
        qualifiers: List[str]

        def __init__(self, node_id, line_loc, col_loc, name, signature, *qualifiers):
            super().__init__(node_id=node_id, line_loc=line_loc, col_loc=col_loc)
            self.children = []
            self.name = name
            self.signature = signature
            self.qualifiers = qualifiers

        def to_str(self, prefix):
            return " ".join([prefix + self.__class__.__name__, self.node_id, self.line_loc, self.col_loc,
                            self.name, f"'{self.signature}'"])


    @dataclass
    class FieldDecl(CompositeNode):
        name: str
        type_text: str
        qualifiers: List[str]

        def __init__(self, node_id: str, line_loc: str, col_loc: str, *args: str):
            args = list(args)
            if args[0] == 'referenced':
                args = args[1:]

            self.qualifiers = args[2:]
            name, type_text = args[:2]
            super().__init__(node_id=node_id, line_loc=line_loc, col_loc=col_loc)
            self.name = name
            self.type_text = type_text

        def to_str(self, prefix: str):
            return " ".join([prefix + self.__class__.__name__, self.node_id, self.line_loc, self.col_loc, self.name, self.type_text])

    @dataclass
    class VarDecl(CompositeNode):
        name: str
        type_text: str
        qualifiers: List[str]

        def __init__(self, node_id: str, line_loc: str, col_loc: str, name: str, type_text: str, *qualifiers: str):
            super().__init__(node_id=node_id, line_loc=line_loc, col_loc=col_loc)
            self.name = name
            self.type_text = type_text
            self.qualifiers = list(qualifiers)

        def to_str(self, prefix: str):
            return " ".join([prefix + self.__class__.__name__, self.node_id, self.line_loc, self.col_loc, self.name, self.type_text, self.qualifiers])


    @dataclass
    class IntegerLiteral(LeafNode):
        col_loc: str
        type_text: str
        value: int

        def __init__(self, node_id: str, col_loc: str, type_text: str, value: str):
            super().__init__(node_id=node_id)
            self.col_loc = col_loc
            self.type_text = type_text
            self.value = value

        def to_str(self, prefix: str):
            return " ".join([prefix + self.__class__.__name__, self.node_id, self.col_loc, self.type_text, self.value])



    @dataclass
    class FloatingLiteral(LeafNode):
        col_loc: str
        type_text: str
        value: float

        def __init__(self, node_id: str, col_loc: str, type_text:str, value: str):
            super().__init__(node_id=node_id)
            self.node_id = node_id
            self.col_loc = col_loc
            self.type_text = type_text
            self.value = value

        def to_str(self, prefix):
            return " ".join([prefix + self.__class__.__name__, self.node_id, self.col_loc, f"'{self.type_text}'", str(self.value)])


    @dataclass
    class LinkageSpecDecl(Node):
        line_loc: str
        line_loc2: str
        spec_text: str


    @dataclass
    class FunctionDecl(LeafNode):
        line_loc: str
        col_loc: str
        name: str
        spec: str
        qualifiers: Optional[str]


    @dataclass
    class ParmVarDecl(LeafNode):
        col_span_loc: str
        col_loc : str
        name: str
        type_text: str
        base_type_text: Optional[str]

        def __init__(self, node_id, col_span_loc, col_loc, *args):
            if len(args) > 3 or len(args) == 0:
                raise Exception(f"Extra unexpected args in ParmVarDecl: {args}")
            self.base_type_text = ""
            if len(args) == 1:
                self.name = ""
                self.type_text = args[0]
            elif len(args) >= 2:
                self.name = args[0]
                self.type_text = args[1]
            if len(args) == 3:
                self.base_type_text = args[2]
            super().__init__(node_id)
            self.col_span_loc = col_span_loc
            self.col_loc = col_loc

        def to_str(self, prefix):
            if self.base_type_text and self.name:
                return " ".join([prefix + self.__class__.__name__, self.node_id, self.col_span_loc, self.col_loc,
                                self.name,
                                f"'{self.type_text}':'{self.base_type_text}'"])
            elif self.base_type_text:
                return " ".join([prefix + self.__class__.__name__, self.node_id, self.col_span_loc, self.col_loc,
                                f"'{self.type_text}':'{self.base_type_text}'"])
            elif self.name:
                return " ".join([prefix + self.__class__.__name__, self.node_id, self.col_span_loc, self.col_loc,
                                self.name,
                                f"'{self.type_text}'"])
            else:
                return " ".join([prefix + self.__class__.__name__, self.node_id, self.col_span_loc, self.col_loc,
                                f"'{self.type_text}'"])

    @dataclass
    class ConstAttr(LeafNode):
        line_loc: str


    @dataclass
    class AccessSpecDecl(LeafNode):
        line_loc: str
        col_loc: str
        access: str

        def __init__(self, node_id: str, line_loc: str, col_loc:str, access:str,
                     parent: Optional["NodeType.Node"] = None):
            super().__init__(node_id=node_id, parent=parent)
            self.line_loc = line_loc
            self.col_loc = col_loc
            self.access = access


        def to_str(self, prefix):
            return " ".join([prefix + self.__class__.__name__, self.node_id, self.line_loc, self.col_loc, self.access])

    @dataclass
    class CompoundStmt(LeafNode):
        location: str
        children: List[str]

        def __init__(self, node_id, location):
            self.children = []
            self.node_id = node_id
            self.location = location

        def to_str(self, prefix):
            return " ".join([prefix + self.__class__.__name__, self.node_id, self.location])


    @dataclass
    class ReturnStmt(CompositeNode):
        location: str
        children: List[str]

        def __init__(self, node_id, location):
            self.children = []
            self.node_id = node_id
            self.location = location

        def to_str(self, prefix):
            return " ".join([prefix + self.__class__.__name__, self.node_id, self.location])

    @dataclass
    class ImplicitCastExpr(CompositeNode):
        args: List[str]

        def __init__(self, node_id: str, location: str, col_loc: str, *args):
            super().__init__(node_id=node_id, line_loc=location, col_loc=col_loc)
            self.args = list(args)

        def to_str(self, prefix: str):
            return " ".join([prefix + self.__class__.__name__, self.node_id])



    @dataclass
    class ConstantArrayType(CompositeTypeNode):
        dimension: int

        def __init__(self, node_id: str, type_text: str, dim_str: str):
            super().__init__(node_id, type_text)
            self.dimension = int(dim_str)

        def to_str(self, prefix):
            return " ".join([prefix + self.__class__.__name__, self.node_id, f"'{self.type_text}'", str(self.dimension)])


    @dataclass
    class NamespaceDecl(CompositeNode):
        name: str

        def __init__(self, node_id: str, line_loc: str, col_loc: str, name: str):
            super().__init__(node_id, line_loc, col_loc)
            self.name = name
            self.children = []

        def to_str(self, prefix: str):
            return " ".join([prefix + self.__class__.__name__, self.node_id, self.line_loc, self.col_loc, self.name])

    @dataclass
    class UnaryOperator(CompositeNode):
        col_loc: str

        def __init__(self, node_id: str, col_loc: str, *args):
            super().__init__(node_id=node_id, line_loc="", col_loc=col_loc)
            self.col_loc = col_loc

        def to_str(self, prefix: str):
            return " ".join([prefix + self.__class__.__name__, self.node_id, self.col_loc])

    @dataclass
    class BinaryOperator(CompositeNode):
        col_loc: str

        def __init__(self, node_id: str, col_loc: str, *args):
            super().__init__(node_id=node_id, line_loc="", col_loc=col_loc)
            self.col_loc = col_loc

        def to_str(self, prefix: str):
            return " ".join([prefix + self.__class__.__name__, self.node_id, self.col_loc])

    @dataclass
    class CXXThisExpr(LeafNode):
        col_loc: str

        def __init__(self, node_id: str, col_loc: str, *args):
            super().__init__(node_id=node_id)
            self.col_loc = col_loc

        def to_str(self, prefix: str):
            return " ".join([prefix + self.__class__.__name__, self.node_id, self.col_loc])

    @dataclass
    class ArraySubscriptExpr(CompositeNode):

        type_text: str
        qualifiers: List[str]

        def __init__(self, node_id: str,  col_loc: str, type_text: str, *args: str):
            super().__init__(node_id=node_id, line_loc="", col_loc=col_loc)
            self.type_text = type_text
            self.qualifiers = list(args)

        def to_str(self, prefix: str):
            return " ".join([prefix + self.__class__.__name__, self.node_id, self.col_loc])

    @dataclass
    class MemberExpr(CompositeNode):
        type_text: str
        arguments: List[str]

        def __init__(self, node_id: str,  col_loc: str, type_text: str, *args: str):
            super().__init__(node_id=node_id, line_loc="", col_loc=col_loc)
            self.type_text = type_text
            self.arguments = list(args)

        def to_str(self, prefix: str):
            return " ".join([prefix + self.__class__.__name__, self.node_id, self.col_loc])



    @dataclass
    class DeclRefExpr(CompositeNode):
        type_text: str
        arguments: List[str]

        def __init__(self, node_id: str,  col_loc: str, type_text: str, *args: str):
            super().__init__(node_id=node_id, line_loc="", col_loc=col_loc)
            self.type_text = type_text
            self.arguments = list(args)

        def to_str(self, prefix: str):
            return " ".join([prefix + self.__class__.__name__, self.node_id, self.col_loc])


class ClangTranslator:

    def __init__(self, file_name: AnyStr):
        if not os.path.exists(file_name):
            raise ValueError(f"Path does not exist: {file_name}")
        self._file_name = file_name
        self._file_iterator = None

    def __enter__(self):
        self._file_iterator = open(self._file_name, 'r')
        return self

    def __exit__(self, exc_type, exc_val, exc_tb):
        if self._file_iterator:
            self._file_iterator.close()
        self._file_iterator = None

    def translate(self):
        return NodeType.Node.process(self._file_iterator)