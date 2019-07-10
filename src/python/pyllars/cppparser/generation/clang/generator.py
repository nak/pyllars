import os
from abc import ABC, abstractmethod
from typing import AnyStr

from pyllars.cppparser.parser.clang_translator import NodeType
from pyllars.cppparser.generation import clang


class Generator(ABC):

    @classmethod
    def create(cls, node: NodeType.Node):
        clazz_name = node.__class__.__name__
        clazz = getattr(clang, clazz_name, None)
        if clazz:
            return clazz(node)

    def __init__(self, root_dir: str, source_file: str, output_dir: str):
        assert os.path.isdir(root_dir)
        assert os.path.exists(os.path.join(root_dir, source_file))
        assert os.path.exists(output_dir)
        assert output_dir != root_dir
        self._c_namespace = os.path.splittext(os.path.basename(source_file))[0]
        self._output_dir = output_dir


    @abstractmethod
    def generate(self):
        """
        generate header and body for this objects' node
        """

