import logging
from io import TextIOBase

from ..parser import code_structure
from .namespaces import *
from .fundamentals import *
from .structs import *
from .methods import *

log = logging.getLogger(__name__)


def _get_generator_class(element: code_structure.Element) -> "Generator":
    from pyllars.cppparser.generation.base import Generator
    clazz = type(element)
    generator_class = globals().get(clazz.__name__)
    if not generator_class:
        if not isinstance(element, code_structure.BuiltinType):
            logging.error("Did not find generator for class %s" % clazz.__name__)
        return Generator
    return generator_class


def _get_generator_header_class(element: code_structure.Element) -> "GeneratorHeader":
    from pyllars.cppparser.generation.base2 import GeneratorHeader
    clazz = type(element)
    generator_class = globals().get("GeneratorHeader" + clazz.__name__)
    if not generator_class:
        if not isinstance(element, code_structure.BuiltinType):
            logging.error("Did not find header generator for class %s" % clazz.__name__)
        return GeneratorHeader
    return generator_class


def _get_generator_body_class(element: code_structure.Element) -> "GeneratorBody":
    from pyllars.cppparser.generation.base2 import GeneratorBody
    clazz = type(element)
    generator_class = globals().get("GeneratorBody" + clazz.__name__)
    if not generator_class:
        if not isinstance(element, code_structure.BuiltinType):
            logging.error("Did not find body generator for class %s" % clazz.__name__)
        return GeneratorBody
    return generator_class


class FileWriter(TextIOBase):

    INDENT = b"    "

    def __init__(self, fil, *args, **kargs):
        super(FileWriter, self).__init__(*args, **kargs)
        self.indent = b""
        self._file = fil
        self.first_line_indent = None

    def incr_indent(self):
        self.indent += FileWriter.INDENT
        self.first_line_indent = None

    def decr_indent(self):
        self.indent = self.indent[:-len(FileWriter.INDENT)]
        self.first_line_indent = None

    def write(self, text: bytes, *args, **kargs):

        def indentation(line):
            index = 0
            if not line:
                return index
            while line and line[0] == 10:
                line = line[1:]

            while index < len(line) and line[index] == 32:
                index += 1
            return index

        def realign(line):
            indent = self.indent
            if line.strip().endswith(b'{'):
                self.incr_indent()
            elif line.strip().startswith(b'}'):
                self.decr_indent()
                indent = self.indent
            if not self.first_line_indent and line.decode("utf-8").strip():
                self.first_line_indent = b' ' * indentation(text)
            length = len(self.first_line_indent) if self.first_line_indent is not None else 0
            return indent + (line[length:] if line.startswith(self.first_line_indent or b'') else line.strip())

        new_text = b"\n".join([realign(line).replace(b'\n', b'') for line in text.splitlines(keepends=True)]) + b"\n"
        self._file.write(new_text, *args, **kargs)

    @classmethod
    def open(cls, *args, **kargs):
        return FileWriter(open(*args, **kargs))

