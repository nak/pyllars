import os
from typing import Optional, Iterator, List, IO, AnyStr

import dataclasses
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
        prev_c = ''
        word_start = 0
        for i, c in enumerate(text):
            if nb_quote == 0 and c == "'":
              nb_quote += 1
              word_start = i +1
              continue
            elif nb_quote == 0 and c == '<':
                nb_angle_brackets += 1
            elif nb_angle_brackets > 0 and c=='>':
                nb_angle_brackets -=1
            elif nb_quote == 0 and c==' ' and nb_angle_brackets == 0:
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
                continue
            prev_c = c
        word = text[word_start: i+1].strip()
        if word:
            words.append(word)
        return words


    @dataclass
    class Node:

        @classmethod
        def process(cls, lines: IO, parent=None, indent=0):
            def process(text, node_type):
                words = NodeType._line_splitter(text)
                try:
                    return node_type(*words)
                except Exception as e:
                    log.error(f"Failed to process {node_type} with args '{words}': " + str(e))
                    raise e

            line = next(lines, None)
            if not line:
                return parent
            line = line.strip()

            if parent == None:
                tag, text = line.split(' ', maxsplit=1)
                node_type = getattr(NodeType, tag)
                node = process(text, node_type)
                cls.process(lines, parent=node, indent=0)
                return node
            else:
                while line:
                    prefix, substance = line.split('-', maxsplit=1)
                    this_indent = (len(prefix)+1)//2
                    if this_indent > indent + 2:
                        raise SyntaxError("Invalidly formatted file, unexpected tree depth")
                    elif this_indent == indent + 2:
                        tag, text = substance.split(' ', maxsplit=1)
                        node_type = getattr(NodeType, tag)
                        node = process(text, node_type)
                        new_parent = node
                        try:
                            cls.process(lines, parent=new_parent, indent=this_indent)
                        except MismatchedDepth as e:
                            line = e.line
                        else:
                            line = next(lines)
                    elif this_indent == indent+1:
                        tag, text = substance.split(' ', maxsplit=1)
                        node_type = getattr(NodeType, tag)
                        node = process(text, node_type)
                        try:
                            parent.children.append(node)
                        except AttributeError:
                            raise Exception(f"Invalid file format: attempt to append child to non-composite node: {type(parent).__name__}")
                        if line[indent] == '`':
                            break
                        line = next(lines)
                    else:
                        raise MismatchedDepth(line)



    @dataclass
    class LeafNode(Node):
        node_id: str

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


    @dataclass
    class TypedefDecl(LocationNode):
        line_loc: str
        col_loc: str
        name: str
        target_name: str
        target_type: "NodeType.TypeNode"

        def __init__(self, node_id: str, line_loc: str, col_loc: str, *args):
            super().__init__(node_id, line_loc, col_loc)
            arg_iter = iter(args)
            arg = next(arg_iter)
            while arg in ['implicit', 'explicit']:
                arg = next(arg_iter)
            self.name = arg
            self.target_name = next(arg_iter)
            self.target_type = None


    @dataclass
    class BuiltinType(TypeNode):
        pass


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


    @dataclass
    class ReferenceType(CompositeTypeNode):
        pass


    @dataclass
    class CXXRecordDecl(CompositeNode):
        defn_text : str

        def __init__(self, node_id, line_loc, col_loc, *args):
            super().__init__(node_id, line_loc, col_loc)
            arg_iter = iter(args)
            arg = next(arg_iter)
            while arg in ['implicit', 'class', 'struct', 'referenced']:
                arg = next(arg_iter)
            assert(arg is not None)
            self.name = arg


    @dataclass
    class DefinitionData(CompositeNode):
        def __init__(self, *args, **kargs):
            pass


    @dataclass
    class CXXConstructorDecl(CompositeNode):
        signature: str


    @dataclass
    class DefaultConstructor(CXXConstructorDecl):
        pass


    @dataclass
    class CopyConstructor(Node):

        def __init__(self, *args):
            super().__init__()
            self._classifiers = args


    @dataclass
    class MoveConstructor(Node):

        def __init__(self, *args):
            self._classifiers = args


    @dataclass
    class CopyAssignment(Node):

        def __init__(self, *args):
            self._classifiers = args



    @dataclass
    class MoveAssignment(Node):

        def __init__(self, *args):
            self._classifiers = args


    @dataclass
    class Destructor(Node):

        def __init__(self, *args):
            self._classifiers = args

    @dataclass
    class FieldDecl(LeafNode):
        line_loc: str
        col_loc: str
        type_text: str

    @dataclass
    class IntegerLiteral(LeafNode):
        col_loc: str
        type_text: str
        value: int


    @dataclass
    class FloatingLiteral(LeafNode):
        col_loc: str
        type_text: str
        value: float


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
    class ParamVarDecl(LeafNode):
        col_span_loc: str
        col_loc : str
        name: str
        type_text: str
        base_type_text: Optional[str]


    @dataclass
    class ConstAttr(LeafNode):
        line_loc: str


    @dataclass
    class AccessSpecDecl(LeafNode):
        line_loc: str
        col_loc: str
        access: str


    @dataclass
    class CompoundStmt(LeafNode):
        location: str


    @dataclass
    class ReturnStmt(LeafNode):
        location: str


    @dataclass
    class ImplicitCastExpr(LeafNode):
        type_text: str
        info: Optional[str]


    @dataclass
    class ConstantArrayType(CompositeTypeNode):
        dimension: int

        def __init__(self, node_id: str, type_text: str, dim_str: str):
            super().__init__(node_id, type_text)
            self.dimension = int(dim_str)


    @dataclass
    class NamespaceDecl(LocationNode):
        name: str



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
