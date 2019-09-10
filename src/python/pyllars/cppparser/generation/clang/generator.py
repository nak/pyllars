import os
from abc import ABC, abstractmethod

from pyllars.cppparser.parser.clang_translator import NodeType
from pyllars.cppparser.generation import clang


class Generator(ABC):
    KEYWORDS = ["asm", "auto", "bool", "break", "case", "catch", "char", "class", "const", "const_cast",
                "constinue", "default", "delete", "do", "double", "dynamic_cast", "else", "enum", "explicit",
                "export", "extern", "false", "float", "for", "friend", "goto", "if", "inline", "int", "long",
                "mutable", "namespace", "new", "operator", "private", "protected", "public", "register",
                "reinterpret_cast", "return", "short", "signed", "sizeof", "static", "static_cast", "struct",
                "switch", "template", "this", "throw", "true", "try", " typedef", "typeid", "typename",
                "union", "unsigned", "using", "virtual", "void", "volatile", "wchar_t", "while", "&", "*", "&&", "..."]

    COMMON_HEADER = """
#include <vector>
#include <cstddef>

#include <Python.h>
#include <pyllars/pyllars.hpp>
"""

    INITIALIZER_CODE = """
                class Initializer_%(name)s: public pyllars::Initializer{
                public:
                    Initializer_%(name)s():pyllars::Initializer(){
                        %(parent_name)s_register(this);                          
                    }

                    int set_up() override{
                       int status = pyllars::Initializer::set_up();
                       return status == 0?%(name)s_set_up():status;
                    }

                    int ready(PyObject * const top_level_mod) override{
                       int status = pyllars::Initializer::ready(top_level_mod);
                       return status == 0?%(name)s_ready(top_level_mod):status;
                    }
                    
                    static Initializer_%(name)s* initializer;
                    
                    static Initializer_%(name)s *singleton(){
                        static  Initializer_%(name)s _initializer;
                        return &_initializer;
                    }
                 };
                 
                """

    INITIALIZER_INSTANTIATION_CODE = """
                //ensure instance is created on global static initialization, otherwise this
                //element would never be reigstered and picked up
                Initializer_%(name)s * Initializer_%(name)s::initializer = singleton();
    """

    REGISTRATION_CODE = """
                status_t %(name)s_register( pyllars::Initializer* const init ){ 
                    // DO NOT RELY SOLEY ON global static initializatin as order is not guaranteed, so 
                    // initializer initializer var here:
                    
                    return Initializer_%(name)s::singleton()->register_init(init);
                }"""

    @classmethod
    def create(cls, node: NodeType.Node, root_dir: str, source_path: str, output_dir: str):
        clazz_name = node.__class__.__name__ + "Generator"
        clazz = getattr(clang, clazz_name, None)
        if clazz:
            return clazz(node, root_dir, source_path, output_dir)

    def __init__(self, node: NodeType.Node, root_dir: str, source_path: str, output_dir: str):
        assert os.path.isdir(root_dir)
        assert not os.path.isabs(source_path)
        assert os.path.exists(os.path.join(root_dir, source_path))
        assert os.path.exists(output_dir)
        assert os.path.abspath(output_dir) != os.path.abspath(root_dir)
        assert not os.path.abspath(root_dir).startswith(os.path.abspath(output_dir))
        self._c_namespace = os.path.splitext(os.path.basename(source_path))[0]
        self._output_dir = output_dir
        self._root_dir = os.path.abspath(root_dir)
        self._source_path_root = os.path.dirname(source_path)
        self._node = node
        self._source_path = source_path

    @abstractmethod
    def generate(self):
        """
        generate header and body for this objects' node
        """

    def generate_all(self):
        self.generate()
        for child in getattr(self._node, "children", []):
            generator = Generator.create(child, self._root_dir, self._source_path, self._output_dir)
            if generator:
                generator.generate_all()

    @property
    def source_path(self):
        return self._source_path

    @property
    def root_dir(self):
        return self._root_dir

    @property
    def source_path_root(self):
        return self._source_path_root

    @property
    def my_root_dir(self):
        root = self._output_dir
        parent = self._node.parent
        path = ""
        while parent:
            if hasattr(parent, 'name'):
                path = os.path.join(parent.name, path)
            parent = parent.parent
        if path:
            root = os.path.join(root, path)
        os.makedirs(root, exist_ok=True)
        return root

    def _wrap_in_namespaces(self, string: str, within_pyllars: bool = False, prefix=""):
        indent = 0
        text = "\n"
        parent = self._node.parent
        while parent:
            if hasattr(parent, 'name') and parent.name:
                text = f"\nnamespace {prefix}{parent.name}{{" + text
                indent += 1
            parent = parent.parent
        text += f"\n        {string}\n\n"
        for indent in reversed(range(indent)):
            indent_text = indent * "   "
            text += f"{indent_text}}}\n"
        if within_pyllars:
            text = f"namespace pyllars{{\n{text}\n}}\n"
        return text
