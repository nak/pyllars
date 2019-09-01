import os
from pyllars.cppparser.parser.clang_translator import NodeType
from .generator import Generator


class CXXRecordDeclGenerator(Generator):

    def generate(self):
        header_stream = open(os.path.join(self.my_root_dir, self._node.name+'.hpp'), 'w',
                             encoding='utf-8')
        body_stream = open(os.path.join(self.my_root_dir, self._source_path_root, self._node.name+'.cpp'), 'w',
                           encoding='utf-8')
        try:
            # generate header
            header_stream.write(Generator.COMMON_HEADER)
            parent = self._node.parent
            parent_name = parent.name if parent else "pyllars"

            #generate body
            body_stream.write(f"""#include "{self.source_path}" 
#include \"{self._node.name}.hpp"
            """)
            if self._node.parent:
                body_stream.write(f"#include \"..{os.sep}{parent_name}.hpp\"\n")
            if not self._node.name and self._node.children:
                body_stream.write(f"""
                    namespace __pyllars_internal{{
                        template<>
                        struct _Types<decltype({self._node.full_cpp_name})>{{
                            static const char* const type_name;
                        }};
                        const char* const _Types<decltype({self._node.full_cpp_name})>::type_name =  "(anonymous enum)";
                    }}
                """)
            else:
                body_stream.write(f"""
                   namespace __pyllars_internal{{
                       template<>
                       struct _Types<::{self._node.full_cpp_name}>{{
                           static const char* const type_name;
                       }};
                       const char* const _Types<::{self._node.full_cpp_name}>::type_name =  "{self._node.name}";
                   }}
               """)
            if not self._node.name or 'definition' in self._node.qualifiers or 'implicit' in self._node.qualifiers:
                return header_stream.name, body_stream.name
            class_name = self._node.name if self._node.name else "anonymous_"
            parent_full_name = parent.full_cpp_name if parent else ""
            body_stream.write(f"""
            
                //From: CXXRecordDeclGenerator.generate
             
                class Initializer_{class_name}: public pyllars::Initializer{{
                public:
                    Initializer_{class_name}():pyllars::Initializer(){{
                    
                        {parent_name}_register(this);                          
                    }}

                    int set_up() override{{
                        static int status = -1;
                        using namespace __pyllars_internal;
                        static bool inited = false;
                        if (inited){{
                            return status;
                        }}
                        //not really much to do here
                        inited = true;
                        status = 0;
                        return status;
                    }}

                    int ready(PyObject * const top_level_mod) override{{
                       int status = pyllars::Initializer::ready(top_level_mod);
                       typedef typename ::{self._node.full_cpp_name}  main_type;
                       status |= __pyllars_internal::PythonClassWrapper< main_type >::initialize();  //classes
                     
                       status |= PyModule_AddObject(::pyllars::{parent_full_name}::{parent_name}_module(), "TestClass", 
                                                    (PyObject*) __pyllars_internal::PythonClassWrapper< typename ::{self._node.full_cpp_name} >::getPyType());
                     
                       return status;
                    }}
                    
                    static Initializer_{class_name}* initializer;
                    
                    static Initializer_{class_name} *singleton(){{
                        static  Initializer_{class_name} _initializer;
                        return &_initializer;
                    }}
                 }};
                 
                
                //ensure instance is created on global static initialization, otherwise this
                //element would never be reigstered and picked up
                Initializer_{class_name} * Initializer_{class_name}::initializer = singleton();
    
            
                """)
            typename = "typename" if 'union' in self._node.qualifiers else ""
            class_full_name = self._node.full_cpp_name
            for base in self._node.bases or []:
                base_class_name = base.full_name
                base_class_bare_name = base.name
                body_stream.write(f"""
                    status |= pylars{base_class_name}::{base_class_bare_name}_set_up();
                    __pyllars_internal::PythonClassWrapper< {typename} ::{class_full_name} >::addBaseClass
                        (&PythonClassWrapper< {typename} {base_class_name} >::getPyType()); /*1*/
                
                """)
            body_stream.write("""}\n\n""")

            code = self.INITIALIZER_CODE % {
                'name': self._node.name,
                'parent_name': self._node.parent.full_cpp_name if self._node.parent else "pyllars"
            }
            code += self.INITIALIZER_INSTANTIATION_CODE % {
                'name': self._node.name,
            }
            body_stream.write(self._wrap_in_namespaces(code))
        finally:
            header_stream.close()
            body_stream.close()
        return header_stream.name, body_stream.name