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


@pytest.fixture(autouse=True)
def setup_test_path(request):
    output_dir = "modules"

    sys.path += [output_dir]

    def fin():
        sys.path.remove(output_dir)

    request.addfinalizer(fin)
    return sys.path


def generate_clang_output(src_path, out_dir):
    """
    :param base_name: base name of file to process (assumed with RESROUCES_DIR)
    :return: path to clang check output for given file name
    """
    output_path = os.path.join(str(out_dir), "clang-output.classes")
    cmd = ["clang-check", "-ast-dump", src_path, "--extra-arg=\"-fno-color-diagnostics\""]
    with open(output_path, 'w') as output_file:
        p = subprocess.run(cmd, stdout=output_file, stderr=subprocess.PIPE)
        if p.returncode != 0:
            raise Exception(f"Failed to parse {src_path} through clang-check tool")
    return ClangOutput(src_path, output_path, os.path.basename(src_path).replace(".hpp", ""))


def build_test_module(src_path, out_dir):
    # not the best test stategy, but generic enough:
    # regurgitate the file back out and compare to original to pass test
    clang_output = generate_clang_output(src_path, out_dir)
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

    return clang_output.module_name


@pytest.fixture
def module_classes(setup_test_path, tmpdir):
    """
    :return: path to clang check output for simple class tests (inheritance, methods, attributes)
    """
    path = os.path.join(RESOURCES_DIR, "classes.hpp")
    return build_test_module(src_path=path, out_dir=tmpdir)


@pytest.fixture
def module_classoperators(setup_test_path, tmpdir):
    """
    :return: path to clang check output for simple class tests (inheritance, methods, attributes)
    """
    path = os.path.join(RESOURCES_DIR, "classoperators.hpp")
    return build_test_module(src_path=path, out_dir=tmpdir)


@pytest.fixture
def module_anonymous(setup_test_path, tmpdir):
    """
    :return: path to clang check output for simple class tests (inheritance, methods, attributes)
    """
    path = os.path.join(RESOURCES_DIR, "anonymous.hpp")
    return build_test_module(src_path=path, out_dir=tmpdir)


@pytest.fixture
def module_complexattributes(setup_test_path, tmpdir):
    """
    :return: path to clang check output for simple class tests (inheritance, methods, attributes)
    """
    path = os.path.join(RESOURCES_DIR, "complexattributes.hpp")
    return build_test_module(src_path=path, out_dir=tmpdir)


@pytest.fixture
def module_enums(setup_test_path, tmpdir):
    """
    :return: path to clang check output for simple class tests (inheritance, methods, attributes)
    """
    path = os.path.join(RESOURCES_DIR, "enums.hpp")
    return build_test_module(src_path=path, out_dir=tmpdir)


@pytest.fixture
def module_functions(setup_test_path, tmpdir):
    """
    :return: path to clang check output for simple class tests (inheritance, methods, attributes)
    """
    path = os.path.join(RESOURCES_DIR, "functions.hpp")
    return build_test_module(src_path=path, out_dir=tmpdir)



@pytest.fixture
def module_globals(setup_test_path, tmpdir):
    """
    :return: path to clang check output for simple class tests (inheritance, methods, attributes)
    """
    path = os.path.join(RESOURCES_DIR, "globals.hpp")
    return build_test_module(src_path=path, out_dir=tmpdir)


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

    def ___test_clang_translation(self):
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

    def test_anonymous(self, module_anonymous):
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

    def test_classes(self, module_classes):
        import pyllars
        import classes
        assert pyllars.outside.ExternalDependency
        obj = pyllars.trial.attributestesting.AttributesClass()
        assert obj.int_val == 22
        obj.int_val = -1
        assert obj.int_val == -1
        assert abs(obj.const_float_val - 39872.55) < 0.001
        with pytest.raises(Exception):
            obj.const_float_val = 2.0
        assert obj.constexpr_int_val() == 42
        assert not hasattr(obj, "private_float_val")
        assert obj.ptr is not None
        assert obj.ptr.at(0).int_val == -1

        #  bit fields
        obj = pyllars.trial.attributestesting.BitFieldsClass()
        assert obj.size_1bit == -1
        assert obj.size_2bit == 0
        assert obj.const_size_11bit == 3
        with pytest.raises(Exception):
            obj.const_size_11bit = 1
        assert obj.size_31bit == 42

        obj.size_31bit = 1 << 28
        assert obj.size_31bit == 1 << 28
        with pytest.raises(ValueError):
            obj.size_1bit = 11  # out of bounds of 1 bit field

        # inheritance testing
        objA = pyllars.trial.inheritancetesting.BaseA()
        objB = pyllars.trial.inheritancetesting.BaseB()
        objInherited = pyllars.trial.inheritancetesting.Inherited()
        objMultiInherited = pyllars.trial.inheritancetesting.MultipleInherited()

        assert objA.int_val == 0
        objA = pyllars.trial.inheritancetesting.BaseA(123)
        assert objA.int_val == 123
        pyllars.trial.inheritancetesting.BaseA.set(objA, -987)
        assert objA.int_val == -987
        assert objB.bMethod().const_size_11bit == 3

        objA2 = objInherited.createBase(911)
        assert objA2.int_val == 911
        assert type(objA) == type(objA2)

        assert objInherited.int_val == 0
        pyllars.trial.inheritancetesting.BaseA.set(objInherited, -135)
        assert objInherited.int_val == -135
        assert objInherited.aMethod().at(0).int_val == 22

        assert objMultiInherited.int_val == 0
        pyllars.trial.inheritancetesting.BaseA.set(objMultiInherited, -135)
        assert objMultiInherited.int_val == -135
        assert objMultiInherited.aMethod().at(0).int_val == 22
        assert objMultiInherited.bMethod().size_31bit == 42



    def test_enums(self, module_enums ):
        import pyllars
        import enums
        assert pyllars.GlobalEnum.value(pyllars.ONE) == 0
        assert pyllars.GlobalEnum.value(pyllars.TWO) == 1
        assert pyllars.GlobalEnum.value(pyllars.THREE) == 2
        assert pyllars.GlobalEnumClass.value(pyllars.GlobalEnumClass.CL_ONE) == 0
        assert pyllars.GlobalEnumClass.value(pyllars.GlobalEnumClass.CL_TWO) == 1
        assert pyllars.GlobalEnumClass.value(pyllars.GlobalEnumClass.CL_THREE) == 2

        assert pyllars.Struct.ClassEnum.value(pyllars.Struct.ClassEnum.ONE) == 1
        assert pyllars.Struct.ClassEnum.value(pyllars.Struct.ClassEnum.TWO) == 2
        assert pyllars.Struct.ClassEnum.value(pyllars.Struct.ClassEnum.THREE) == 3
        value = pyllars.Struct.ONE.value
        assert value(pyllars.Struct.ONE) == 1
        assert value(pyllars.Struct.TWO) == 12
        assert value(pyllars.Struct.THREE) == 23

    def test_classoperators(self, module_classoperators ):
        import pyllars
        import classoperators
        obj = pyllars.trial.operators.FullOperatorList();
        assert ((+obj).val() - obj.val()) == 0.0
        assert abs(obj.val() - 0.134) < 0.001
        assert abs((~obj).val() - (-0.134 - 1.2)) < 0.001
        assert abs((obj + 2).val() - (0.134 + 2.0)) < 0.001
        assert abs((obj - 99).val() - (0.134 - 99.0)) < 0.001
        assert abs((obj/10.0).val() - 0.0134) < 0.001
        obj = pyllars.trial.operators.FullOperatorList(35.0)
        assert abs((obj & 33).val() - 33.0) < 0.001
        assert abs((obj | 8).val() - 43.0) < 0.001
        assert abs((obj ^ 3).val() - 32.0) < 0.001
        assert abs((obj << 2).val() - 140.0) < 0.001
        assert abs((obj >> 1).val() - 17.0) < 0.001
        obj += 64
        assert abs(obj.val() - (35.0 + 64.0)) < 0.001
