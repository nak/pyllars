import filecmp
import tempfile

from pyllars.cppparser.generation import clang
from pyllars.cppparser.generation.base2 import Compiler
from pyllars.cppparser.parser.clang_translator import ClangTranslator, NodeType

import os

RESOURCES_DIR = os.path.join(os.path.dirname(__file__), "resources")
PYLLARS_INCLUDE_DIR = os.path.join(os.path.dirname(__file__), "..", "..", "..", "src", "python", "resources", "include")


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
        ns_B.parent = ns_A
        ns_C.parent = ns_B
        generator = clang.NamespaceDeclGenerator(ns_C, ".", ".", "/tmp")
        text = generator._wrap_in_namespaces("HERE")
        assert text == """
namespace B{
   namespace A{

        HERE

   }
}
"""

    def test_generate_ns(self, tmpdir):
        ns_C = NodeType.NamespaceDecl(node_id="1", line_loc="", col_loc="", name="C")
        ns_B = NodeType.NamespaceDecl(node_id="1", line_loc="", col_loc="", name="B")
        ns_A = NodeType.NamespaceDecl(node_id="2", line_loc="", col_loc="", name="A")
        ns_B.parent = ns_A
        ns_C.parent = ns_B
        dummy_header = open(os.path.join(str(tmpdir), "dummy.hpp"), 'w')
        dummy_header.close()
        output_dir = os.path.join(str(tmpdir), "output")
        os.makedirs(output_dir, exist_ok=True)
        generator = clang.Generator.create(ns_C, str(tmpdir), os.path.basename(dummy_header.name), output_dir)
        header_file_name, body_file_name = generator.generate()
        assert open(header_file_name).read() == """
#include <vector>
#include <cstddef>

#include <Python.h>
#include <pyllars/pyllars.hpp>

namespace A{
namespace B{

        
            PyObject *C_module(); 
            
            /**
             * static initializer method to register initialization routine
             **/
            status_t C_register( pyllars::Initializer* const);
            
            /**
             * called back on initialization to initialize Python wrapper for this C construct
             * @param top_level_mod:  mod to which the wrapper Python object should belong
             **/
            status_t C_ready(PyObject * const top_level_mod);
            

   }
}
"""
        hash = "#"
        line = f"{hash}include \"C.hpp\"\n#include \"../B.hpp\""
        assert open(body_file_name).read() == line + \
"""

namespace A{
namespace B{

        
                    PyObject * C_module(){
                        static PyObject* C_mod = nullptr;
                        if (!C_mod){
                            #if PY_MAJOR_VERSION==3
        
                                // Initialize Python3 module associated with this namespace
                                static PyModuleDef C_moddef = {
                                    PyModuleDef_HEAD_INIT,
                                    "C",
                                    "Example module that creates an extension type.",
                                    -1,
                                    NULL, NULL, NULL, NULL, NULL
                                };
                                C_mod = PyModule_Create(&C_moddef);
        
                            #else
        
                                // Initialize Python2 module associated with this namespace
                                C_mod = Py_InitModule3("C", nullptr,
                                                              "Module corresponding to C++ namespace C");
        
                            #endif
                        }
                        return C_mod;
                    }
            
                    
                    status_t C__ready(PyObject* global_mod){
                      static bool inited = false;
                        if (inited) return 0;// if already initialized
                        inited = true;
                        int status = 0;
    
                        if (!::A::B_module() || C_module()){
                            status = -2;
                        } else {
                            PyModule_AddObject( ::A::B_module(), "C", C_module());
                        }
                        return status;
                    }
                    
                    status_t C_set_up(){
                        return C_module()?0:-2;
                    } // end init
    
            
                class Initializer_C: public pyllars::Initializer{
                public:
                    Initializer_C():pyllars::Initializer(){
                        A::B_register(this);                          
                    }

                    int set_up() override{
                       int status = pyllars::Initializer::set_up();
                       return status == 0?C_set_up():status;
                    }

                    int ready(PyObject * const top_level_mod) override{
                       int status = pyllars::Initializer::ready(top_level_mod);
                       return status == 0?C_ready(top_level_mod):status;
                    }
                    
                    static Initializer_C* initializer;
                    
                    static Initializer_C *singleton(){
                        static  Initializer_C _initializer;
                        return &_initializer;
                    }
                 };
                 
                
                //ensure instance is created on global static initialization, otherwise this
                //element would never be reigstered and picked up
                Initializer_C * Initializer_C::initializer = singleton();
    
                status_t C_register( pyllars::Initializer* const init ){ 
                    // DO NOT RELY SOLEY ON global static initializatin as order is not guaranteed, so 
                    // initializer initializer var here:
                    
                    return Initializer_C::singleton()->register_init(init);
                }

   }
}
"""

        generator = clang.Generator.create(ns_B, str(tmpdir), os.path.basename(dummy_header.name), output_dir)
        generator.generate()
        generator = clang.Generator.create(ns_A, str(tmpdir), os.path.basename(dummy_header.name), output_dir)
        generator.generate()
        compiler = Compiler(compiler_flags=[f"-I{RESOURCES_DIR}", f"-I{PYLLARS_INCLUDE_DIR}"],
                            output_dir=output_dir,optimization_level="-O0", debug=True)
        compiler.compile(body_file_name)

    def test_generation(self, tmpdir):
        file_name = os.path.join(RESOURCES_DIR, "clang-check-output.example")
        # not the best test stategy, but generic enough:
        # regurgitate the file back out and compare to original to pass test
        output_dir = os.path.join(str(tmpdir), "output")
        os.makedirs(output_dir)
        header = os.path.join(RESOURCES_DIR, "classes.hpp")
        with ClangTranslator(file_name=file_name) as translator:
            root = translator.translate()
            generator = clang.Generator.create(root, os.path.dirname(header), os.path.basename(header), output_dir)
            generator.generate_all()

        compiler = Compiler(compiler_flags=[f"-I{RESOURCES_DIR}", f"-I{PYLLARS_INCLUDE_DIR}"],
                            output_dir=output_dir, optimization_level="-O0", debug=True)
        for dir, dirnames, filenames in os.walk(output_dir):
            for filename in [os.path.join(dir, name) for name in filenames if name.endswith(".cpp")]:
                compiler.compile(filename)