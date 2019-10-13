import filecmp
import glob
import importlib
import shutil
import subprocess
import tempfile
from contextlib import suppress

import pytest
from dataclasses import dataclass

from pyllars.cppparser.generation import clang
from pyllars.cppparser.compilation.compile import Compiler, Linker
from pyllars.cppparser.parser.clang_translator import ClangTranslator, NodeType

import os
import sys

RESOURCES_DIR = os.path.join(os.path.dirname(__file__), "resources")
PYLLARS_INCLUDE_DIR = os.path.join(os.path.dirname(__file__), "..", "..", "..", "src", "python", "resources", "include")


@dataclass
class ClangOutput:
    src_path: str
    output_path: str
    module_name: str


@pytest.fixture(params=glob.glob(os.path.join(RESOURCES_DIR, "*.hpp")))
def clang_output(tmpdir, request):
    """
    :param base_name: base name of file to process (assumed with RESROUCES_DIR)
    :return: path to clang check output for given file name
    """
    src_path = request.param
    output_path = os.path.join(str(tmpdir), "clang-output.classes")
    cmd = ["clang-check", "-ast-dump", src_path, "--extra-arg=\"-fno-color-diagnostics\""]
    with open(output_path, 'w') as output_file:
        p = subprocess.run(cmd, stdout=output_file, stderr=subprocess.PIPE)
        if p.returncode != 0:
            raise Exception(f"Failed to parse {src_path} through clang-check tool")
    return ClangOutput(src_path, output_path, os.path.basename(request.param).replace(".hpp", ""))


@pytest.fixture
def test_module(clang_output, request):
    # not the best test stategy, but generic enough:
    # regurgitate the file back out and compare to original to pass test
    output_dir = "modules"
    output_gen_dir = os.path.join(output_dir, clang_output.module_name)
    with suppress(Exception):
        shutil.rmtree(output_gen_dir)
    os.makedirs(output_gen_dir, exist_ok=True)
    header = clang_output.src_path
    with ClangTranslator(file_name=clang_output.output_path) as translator:
        root = translator.translate()
        generator = clang.Generator.create(root, os.path.dirname(header), os.path.basename(header), output_gen_dir)
        generator.generate_all()

    compiler_flags = [f"-I{RESOURCES_DIR}", f"-I{PYLLARS_INCLUDE_DIR}"]
    compiler = Compiler(compiler_flags=compiler_flags,
                        output_dir=output_dir, optimization_level="-O0", debug=True)
    objects = []
    body_path = clang_output.src_path.replace(".hpp", ".cpp")
    if os.path.exists(body_path):
        objects.append(compiler.compile(body_path))
    for dir, dirnames, filenames in os.walk(output_gen_dir):
        for filename in [os.path.join(dir, name) for name in filenames if name.endswith(".cpp")]:
            objects.append(compiler.compile(filename))
    linker = Linker(compiler_flags=compiler_flags, linker_options=[], debug=True)
    linker.link(objects=objects, output_module_path=output_dir, module_name=clang_output.module_name)
    sys.path += [output_dir]
    importlib.import_module(clang_output.module_name)

    def fin():
        sys.path.remove(output_dir)

    request.addfinalizer(fin)
    return clang_output.module_name

@pytest.fixture
def clang_output_classes(tmpdir):
    """
    :return: path to clang check output for simple class tests (inheritance, methods, attributes)
    """
    return clang_output(tmpdir, "classes")


@pytest.fixture
def clang_output_complexattributes(tmpdir):
    """
    :return: path to clang check output for classes with compled attributes (array attributes, etc)
    """
    return clang_output(tmpdir, "complextattributes")


@pytest.fixture
def clang_output_enums(tmpdir):
    """
    :return: path to clang check output for simple enums/class enums
    """
    return clang_output(tmpdir, "enums")


@pytest.fixture
def clang_output_functions(tmpdir):
    """
    :return: path to clang check output for simple function tests
    """
    return clang_output(tmpdir, "functions")


@pytest.fixture
def clang_output_globals(tmpdir):
    """
    :return: path to clang check output for simple class tests (inheritance, methods, attributes)
    """
    return clang_output(tmpdir, "globals")


def compare_builtintype(self, other: NodeType.BuiltinType):
    return type(other) == NodeType.BuiltinType and \
        other.node_id == self.node_id and \
        other.type_text == self.type_text


NodeType.BuiltinType.compare = compare_builtintype


def compare_typedefdecl(self, other: NodeType.TypedefDecl):
    return type(other) == NodeType.TypedefDecl and \
        other.node_id == self.node_id and \
        other.col_loc == self.col_loc and \
        other.line_loc == self.line_loc and \
        other.name == self.name and \
        other.target_name == self.target_name and \
        len(self.children) == len(other.children) and \
           (self.children[0].compare(other.children[0]) if self.children else True)


NodeType.TypedefDecl.compare = compare_typedefdecl


def compare_namespacedecl(self, other: NodeType.NamespaceDecl):
    return type(other) == NodeType.NamespaceDecl and \
            other.node_id == self.node_id  and \
            other.col_loc == self.col_loc and \
            other.line_loc == self.line_loc and \
            other.name == self.name


NodeType.NamespaceDecl.compare = compare_namespacedecl


expectation = NodeType.TranslationUnitDecl('0x186bca8', '<<invalid sloc>>', '<invalid sloc>')
child = NodeType.TypedefDecl('0x186c580', "<<invalid sloc>>", '<invalid sloc>', 'implicit', '__int128_t', '__int128')
child.children.append(NodeType.BuiltinType('0x186c240', '__int128'))
expectation.children.append(child)
child = NodeType.TypedefDecl('0x186c5e8', '<<invalid sloc>>',  '<invalid sloc>', 'implicit', '__uint128_t', 'unsigned __int128')
expectation.children.append(child)
child = NodeType.TypedefDecl('0x186c928', '<<invalid sloc>>',  '<invalid sloc>', 'implicit', '__NSConstantString', '__NSConstantString_tag')
expectation.children.append(child)
child = NodeType.TypedefDecl('0x18a6130', '<<invalid sloc>>',  '<invalid sloc>', 'implicit', '__builtin_ms_va_list', 'char *')
expectation.children.append(child)
child = NodeType.TypedefDecl('0x18a6468', '<<invalid sloc>>',  '<invalid sloc>', 'implicit', '__builtin_va_list', '__va_list_tag [1]')
expectation.children.append(child)
child = NodeType.NamespaceDecl('0x18a64b8', '</home/jrusnak/workspace/pyllars/test/python/resources/classes.hpp:4:1, line:5:1>',
                                'line:4:11', 'unused')
expectation.children.append(child)
child = NodeType.TypedefDecl('0x18a6538', '<line:7:1, col:16>', 'col:16', 'str', 'char *')
expectation.children.append(child)
child = NodeType.TypedefDecl('0x18a65a0', '<line:8:1, col:27>', 'col:27', 'const_str', 'const char *const')
expectation.children.append(child)
child = NodeType.NamespaceDecl('0x18a65f0', '<line:11:1, line:14:1>', 'line:11:11', 'outside')
expectation.children.append(child)
child = NodeType.NamespaceDecl('0x18a6830', '<line:16:1, line:68:1>', 'line:16:11', 'trial')
expectation.children.append(child)


class TestClangTranslation:

    def test_clang_translation(self):
        file_name = os.path.join(RESOURCES_DIR, "clang-check-output.example")
        # not the best test stategy, but generic enough:
        # regurgitate the file back out and compare to original to pass test
        with ClangTranslator(file_name=file_name) as translator:
            root = translator.translate()
            with tempfile.NamedTemporaryFile(mode='w+b') as f:
                for line in root.serialize(""):
                    if 'ConstantArrayType' in line:
                        f.write(line.rstrip().encode('utf-8') + b" \n")
                    else:
                        f.write(line.rstrip().encode('utf-8') + b"\n")
                f.flush()
                assert filecmp.cmp(f.name, file_name)

        root.by_header_file()

    def test_wrap_ns(self):
        ns_C = NodeType.NamespaceDecl(node_id="1", line_loc="", col_loc="", name="C")
        ns_B = NodeType.NamespaceDecl(node_id="1", line_loc="", col_loc="", name="B")
        ns_A = NodeType.NamespaceDecl(node_id="2", line_loc="", col_loc="", name="A")
        ns_A.parent = None
        ns_B.parent = ns_A
        ns_C.parent = ns_B
        generator = clang.NamespaceDeclGenerator(ns_C, ".", ".", "/tmp")
        text = generator._wrap_in_namespaces("HERE")
        assert text == """
namespace A{
namespace B{

        HERE

   }
}
"""

    def test_generation(self, test_module):
        test_function = getattr(self, f"test_{test_module}")
        test_function()

    def test_anonymous(self):
        sys.path.append("./modules")
        import pyllars
        import anonymous
        AnonInner = pyllars.trial.AnonymousInnerTypes.AnonInner
        inst = AnonInner()
        inst.anon_struct_instance.word1 = 1
        assert inst.anon_struct_instance.word1 == pyllars.c_int(1)
        inst.anon_struct_instance.word1 = 5
        assert inst.anon_struct_instance.word1 == 5
        with pytest.raises(ValueError):
            inst.anon_struct_instance.word1 = -2
        assert inst.anon_struct_instance.word1 == 5

        inst.anon_struct_instance.bool1 = False
        assert inst.anon_struct_instance.bool1 is False
        inst.anon_struct_instance.bool1 = True
        assert inst.anon_struct_instance.bool1 is True
        inst.anon1 = pyllars.c_int(1284)
        assert inst.anon1 == 1284
        inst.recursiveAnon2 = -121
        assert inst.recursiveAnon2 == -121
        inst.recursiveAnon2 = 2020
        assert inst.recursiveAnon2 == 2020

        inst.intval = -122
        assert inst.intval == -122
        assert inst.sval1 == -122
        assert inst.sval2 == -1  # union two shorts overlapping with intval
        inst.sval1 = 1
        assert inst.sval1 == 1
        assert inst.intval == -65535

    def test_classes(self, test_module):
        sys.path.append("./modules")
        import pyllars
        import classes