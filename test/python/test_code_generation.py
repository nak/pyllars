import glob
import os.path
import shutil

import pytest
from pyllars.cppparser import parser
from pyllars.cppparser.compilation import CompilationModel, ByNamespaceCompilationModel, CodeBase
from pyllars.cppparser.generation import Generator, Folder, Compiler

RESOURCES_DIR = os.path.join(os.path.dirname(__file__), "resources")
hpp_input_files = glob.glob(os.path.join(RESOURCES_DIR, "*.hpp")) + ["/usr/include/pthread.h"]


class TestCodeGen(object):

    @pytest.mark.parametrize("input_file",  hpp_input_files,
                             ids=[os.path.basename(f).replace(".hpp", "") for f in hpp_input_files])
    def test_generation(self, input_file):
        parser.init()
        gen_path = os.path.join("generated", os.path.basename(input_file))
        gen_path = os.path.splitext(gen_path)[0]
        gen_path = "generated"
        if os.path.exists(gen_path):
            shutil.rmtree(gen_path)
        os.makedirs(gen_path)
        top =Generator.generate_code(source=input_file,
                                     src_paths=[input_file], folder=Folder(gen_path),
                                     module_name=os.path.basename(input_file).split('.')[0],
                                     include_paths=[os.path.dirname(input_file)])
        code_base = CodeBase(top, base_dir=gen_path)
        compilation_model = ByNamespaceCompilationModel(code_bases=[code_base], globals_module_name=os.path.basename(input_file).replace(".hpp", "") )
        compilation_model.compile_modules(compiler_flags=[],
                                          linker_flags=["-lpthread"],
                                          addl_sources=[input_file.replace(".hpp", ".cpp")])
        #rc, output = Compiler(gen_path).compile_all(src_paths=[input_file], output_module_path=os.path.join("..", "..", "src", "pyllars", os.path.basename(input_file).split('.')[0] +".so"))
        #assert rc == 0, "Failed to compile/link generated code: \n%s" % output


class TestGlobalImports(object):

    @pytest.mark.run(after='test_generation')
    def test_globals_import(self):
        import pyllars.globals as globals
        assert abs(globals.global_number() - 4.2) < 0.000001
        #assert globals.trial.some_global_function(23.1, ??) == 23  # can't test since incomplete type for 2nd param
        assert globals.trial.some_global_function is not None  # at least test it exists

        assert globals.FIRST == 1
        assert globals.SECOND == 2
        assert globals.THIRD == 3

        assert globals.ZERO == 0
        assert globals.ONE == 1
        assert globals.TWO == 2

        with pytest.raises(RuntimeError):  #null ptr deref
            globals.null_long_ptr2().at(0)
        with pytest.raises(RuntimeError):  #null ptr deref
            globals.null_long_ptr().at(0)

        assert globals.UnsizedClassEnum.A == 0
        assert globals.UnsizedClassEnum.B == 1
        assert globals.UnsizedClassEnum.C == 2

        assert globals.SizedClassEnum.A == 0
        assert globals.SizedClassEnum.B == 1
        assert globals.SizedClassEnum.C == 2

        assert globals.enum_var() == globals.TWO

