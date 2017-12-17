from contextlib import contextmanager
from io import TextIOBase

import os

from .base import Folder, Generator, qualified_name
from . import parser
from .fundamentals import VarDecl


def template_decl(element: Generator):
    if element.parent:
        return template_decl(element.parent) + (("\n" + element.template_decl) if element.template_decl.strip() else "")
    else:
        return element.template_decl


class CXXRecordDecl(Generator):

    @classmethod
    def is_generatable(cls):
        return True

    @contextmanager
    def templated(self, stream: TextIOBase):
        stream.write(("""
        class %(name)s{""" % {
            'name': self.element.basic_name,
        }).encode('utf-8'))
        yield stream
        stream.write(("""};""").encode('utf-8'))
        return stream

    def basic_includes(self):
        text = super().basic_includes()
        for subelement in list(self.element._base_classes.get('public') or []):
            child_generator = Generator.generator_mapping[subelement]
            text += ("\n#include \"%s\"" % child_generator.header_file_path()).encode('utf-8')
        return text

    def generate_body_proper(self, stream: TextIOBase, as_top: bool = False) -> None:
        if not self.element._is_definition:
            return
        if self.element.is_implicit:
            return

        stream.write(("                //From: %(file)s:generate_body_proper\n" % {
            'file': os.path.basename(__file__),
        }).encode('utf-8'))

        if not self.element.is_union:
            typename = 'typename'
        else:
            typename = ""
        stream.write(("""
            %(template_decl)s
            status_t %(pyllars_scope)s::%(name)s::%(basic_name)s_init(PyObject * const global_mod){
                using namespace __pyllars_internal;
                typedef %(typename)s %(scope)s::%(template_prefix)s%(name)s  main_type;
                static status_t _status = -1;
                static bool inited = false;
                if (inited){
                    return _status;
                }
                inited = true;
                status_t status = 0;
""" % {
            'qname': qualified_name(self.element.name or "anonymous_%s" % self.element.tag),
            'basic_name': self.element.basic_name,
            'name': self.element.name or "anonymous_%s" % self.element.tag,
            'template_decl': template_decl(self),
            'pyllars_scope': self.element.pyllars_scope,
            'scope': self.element.scope,
            'template_prefix': 'template ' if self.parent and self.parent.element.is_template_macro else "",
            'typename': typename,
        }).encode('utf-8'))

        for base in self.element.public_base_classes or []:
            stream.write(("""
                status |= pyllars%(base_class_name)s::%(base_class_bare_name)s_init(PyObject* const global_mod);
                 __pyllars_internal::PythonClassWrapper< %(typename)s %(scope)s::%(template_prefix)s%(class_name)s >::addBaseClass
                    (&PythonClassWrapper< %(typename)s %(base_class_name)s >::Type); /*1*/
""" % {
                'class_name': self.element.name,
                'base_class_name': base.full_name,
                'base_class_bare_name': base.name,
                'scope': self.element.scope,
                'template_prefix': 'template ' if self.parent and self.parent.element.is_template_macro else "",
                'typename': typename
            }).encode('utf-8'))

        if self.element.name:
            if self.element.parent.is_namespace:
                stream.write(("""
                status |=  __pyllars_internal::PythonClassWrapper< main_type >::initialize(
                             "%(name)s",
                             "%(name)s",
                             %(module_name)s,
                             "%(full_name)s");  //classes
    """ % {
                    'name': self.element.name or "_anonymous%s" % self.element.tag,
                    'module_name': self.element.parent.pyllars_module_name,
                    'full_name': self.element.full_name,
                }).encode('utf-8'))
            elif self.element.parent.is_template_macro:
                if len(self.element.template_arguments) == 1:
                    arg = self.element.template_arguments[0]
                    if isinstance(arg, parser.TemplateTypeParmDecl):
                        typename = 'typename' if not arg._type.is_union else ""
                        stream.write(("""
                             PyObject* sequence = (PyObject*) & __pyllars_internal::PythonClassWrapper<%s %s >::Type ;
                  """ % (typename, arg._type.full_name).encode("utf-8")))
                    elif isinstance(arg._type, parser.BuiltinType):
                        stream.write(("""
                            PyObject* sequence = (PyObject*) & %s;
                        """ % arg._type.to_py_conversion_code(arg.name)).encode("utf-8"))
                    else:
                        stream.write(("""
                            PyObject* sequence = (PyObject*) & _pyllars_internal::Factor<%(class_name)s>::create_single_instance(
                                1, 
                                %(arg_name)s,
                                false
                                ));
                        """ % {

                            'class_name': arg._type.full_name,
                            'arg_name': arg.name,
                        }).encode("utf-8"))
                else:
                    stream.write(("""
                    PyObject* sequence = PyTuple_New(%d);
                    """ % len(self.element.template_arguments)).encode("utf-8"))
                    for index, arg in enumerate(self.element.template_arguments):
                        typename = "typename" if not arg._type.is_union else ""
                        if isinstance(arg, parser.TemplateTypeParmDecl):
                            stream.write(("""
                            PyTuple_SetItem(sequence, %d, (PyObject*) & __pyllars_internal::PythonClassWrapper< %s %s >::Type )
                            """ % (index, typename, arg.name)).encode("utf-8"))
                        elif isinstance(arg._type, parser.BuiltinType):
                            stream.write(("""
                            PyTuple_Add(sequence, %d, (PyObject*) & %s )
                            """ % (index, arg._type.to_py_conversion_code(arg.name))).encode("utf-8"))
                        else:
                            stream.write(("""
                            PyTuple_Add(sequence, %(index)d, (PyObject*) & _pyllars_internal::Factory<%(typename)s %(class_name)s >::create_single_instance(
                                 1, 
                                 %(arg_name)s,
                                 false
                                 ));
                            """ % {
                                'index': index,
                                'class_name': arg._type.full_name,
                                'arg_name': arg.name,
                                'typename': typename,
                            }).encode("utf-8"))
                stream.write(("""
                %(pyllarse_scope)s::%(parent_name)s::register_instance(
                    (PyObject*) sequence,
                    (PyObject*) & __pyllars_internal::PythonClassWrapper< %(typename)s %(scope)s::%(template_prefix)s%(class_name)s >::Type
                );
                """ % {
                    'pyllarse_scope': self.element.pyllars_scope,
                    'parent_name': qualified_name
                    (self.element.parent.basic_name if (self.element.parent.name and self.element.parent.name != "::")
                     else "pyllars"),
                    'parent': self.element.scope if self.element.scope != '::' else "",
                    'class_name': self.element.name,
                    'scope': self.element.scope,
                   'template_prefix': 'template ' if self.parent and self.parent.element.is_template_macro else "",
                    'typename': 'typename' if not self.element.is_union else ""
                }).encode("utf-8"))
            else:
                stream.write(("""
                __pyllars_internal::PythonClassWrapper< %(typename)s %(parent_class_scope)s::%(parent_class_name)s >::addClassMember
                    ("%(class_name)s",
                     (PyObject*) & __pyllars_internal::PythonClassWrapper< %(typename)s %(scope)s::%(class_name)s >::Type);
""" % {
                    'parent_class_name': self.element.parent.name,
                    'parent_class_scope': self.element.parent.scope,
                    'class_name': self.element.name,
                    'scope': self.element.scope,
                    'typename': 'typename' if not self.element.is_union else ""
                }).encode('utf-8'))
        stream.write(("""
                _status = status;
                return status;
            }
            
            %(template_decl)s
            static %(pyllars_scope)s::%(name)s::Initializer_%(basic_name)s* _init(){
                static %(pyllars_scope)s::%(name)s::Initializer_%(basic_name)s *_initializer = 
                  new %(pyllars_scope)s::%(name)s::Initializer_%(basic_name)s();
                %(pyllars_scope)s::%(name)s::Initializer_%(basic_name)s::initializer = _initializer;
                return _initializer;
            }
            
            %(template_decl)s
            status_t %(pyllars_scope)s::%(name)s::%(qname)s_register( pyllars::Initializer* const init ){ 
                static Initializer_%(basic_name)s *_initializer = _init();
                Initializer_%(basic_name)s::initializer = _initializer;
                return Initializer_%(basic_name)s::initializer->register_init(init);
             }


""" % {
            'name': self.element.name if self.element.name else "pyllars",
            'basic_name': self.element.basic_name,
            'qname': self.element.basic_name if self.element.basic_name else "pyllars",
            'parent_name': qualified_name
                (self.element.parent.basic_name if (self.element.parent.name and self.element.parent.name != "::")
                                          else "pyllars"),
            'pyllars_scope': self.element.pyllars_scope,
            'template_decl': template_decl(self),
        }).encode('utf-8'))


        stream.write(("""
            %(template_decl)s
            %(typename)s %(pyllars_scope)s::%(template_prefix)s%(name)s::Initializer_%(basic_name)s 
            *%(pyllars_scope)s::%(template_prefix)s%(name)s::Initializer_%(basic_name)s::initializer = _init();
        """ % {
            'basic_name': self.element.basic_name,
            'name': self.element.name,
            'pyllars_scope': self.element.pyllars_scope,
            'template_decl': template_decl(self),
            'template_prefix': 'template ' if self.parent and self.parent.element.is_template_macro else "",
            'typename': 'typename' if not self.element.is_union else "",
        }).encode('utf-8'))


class CXXMethodDecl(Generator):
    METHOD_NAMES = {'operator-': ['addMethod__inv__', 'addMethod__sub__'],
                    'operator+': ['addMethod__pos__', 'addMethod__addd__'],
                    'operator*': ['addMethod__deref__', 'addMethod__mul__'],
                    'operator/': [None, 'addMethod__div__'],
                    'operator&': ['addMethod_addr__', 'addMethod__and__'],
                    'operator|': [None, 'addMethod__or__'],
                    'operator^': [None, 'addMethod__xor__'],
                    'operator<<': [None, 'addMethod__lshift__'],
                    'operator>>': [None, 'addMethod__rshift__'],
                    'operator%' : [None, 'addMethod__mod__'],
                    'operator+=': [None, 'addMethod__iadd__'],
                    'operator-=': [None, 'addMethod__isub__'],
                    'operator*=': [None, 'addMethod__imul__'],
                    'operator%=': [None, 'addMethod__imod__'],
                    'operator&=': [None, 'addMethod__iand__'],
                    'operator|=': [None, 'addMethod__ior__'],
                    'operator^=': [None, 'addMethod__ixor__'],
                    'operator<<=': [None, 'addMethod__ilshift__'],
                    'operator>>=': [None, 'addMethod__irshift__'],
                    }

    @classmethod
    def is_generatable(cls):
        return True

    def _func_declaration(self):
        base = "addClass" if self.element.is_static else "add"
        method_name = CXXMethodDecl.METHOD_NAMES.get(self.element.name) or ("%sMethod" % base )
        if self.element.has_varargs:
            method_name += "Varargs"

        return """
    __pyllars_internal::PythonClassWrapper< %(typename)s %(scope)s>::template %(py_method_name)s<%(is_const)s name, %(return_type)s %(args)s>
       ( &%(scope)s::%(method_name)s, argumentNames);

""" % {
            'args': ("," if self.element.params else "") + ", ".join([p.type_.full_param_name for p in self.element.params]),
            'is_const': str(self.element.is_const).lower() + "," if not self.element.is_static else "",
            'method_name': self.element.name or "anonymous_%s" % self.element.tag,
            'names': ",".join(['"%s"' % (e.name or "param_%d" % index) for index, e in enumerate(self.element.params)]),
            'py_method_name': method_name,
            'return_type': self.element.return_type.full_name if self.element.return_type else "void",
            'scope': self.element.block_scope,
            'template_prefix': 'template ' if self.element.parent.parent.is_template_macro else "",
            'typename': 'typename' if not self.element.is_union else ""
        }

    def generate_header_core(self, stream: TextIOBase, as_top=False):
        if "operator delete" in self.element.name:
            return
        super(CXXMethodDecl, self).generate_header_core(stream, as_top=as_top)

    def generate_body_proper(self, stream: TextIOBase, as_top: bool = False) -> None:
        if "operator delete" in self.element.basic_name:
            return
        imports = set([])
        for elem in self.element.params:
            if elem and elem.type_.namespace_name != self.element.namespace_name and elem.type_.namespace_name != "::":
                imports.add(elem.namespace_name)
        if self.element.return_type and \
                        self.element.return_type.namespace_name != self.element.namespace_name and self.element.return_type.namespace_name != "::":
            imports.add(self.element.return_type.namespace_name)


        stream.write(("""
            constexpr cstring name = "%(pyname)s";

            //generated from %(file)s.generate_body_proper
            // FUNCTION %(name)s THROWS %(throws)s
            %(template_decl)s
            status_t %(pyllars_scope)s::%(sanitized_name)s_init(PyObject * const global_mod){
               static const char* const argumentNames[] = {%(argument_names)s nullptr};
               status_t status = 0;
               %(imports)s
               %(func_decl)s
               return status;
            }

            %(template_decl)s
            status_t %(pyllars_scope)s::%(sanitized_name)s_register(pyllars::Initializer* const init){
                return 0;
            }
            
            %(template_decl)s
            typename %(pyllars_block_scope)s::Initializer_%(sanitized_name)s 
            *%(pyllars_scope)s::Initializer_%(sanitized_name)s::initializer =
            new %(pyllars_block_scope)s::Initializer_%(sanitized_name)s();
""" % {
            'arguments': (',' if len(self.element.params) > 0 else "") + ', '.join([t.type_.full_name for
                                                                               t in self.element.params]),
            'argument_names': ','.join(
                ["\"%s\"" % (arg.name if arg.name else "_%s" % (index + 1)) for index, arg in
                 enumerate(self.element.params)]) + (',' if self.element.params else ''),
            'sanitized_name': self.sanitize(self.element.basic_name),
            'file': __file__,
            'func_decl': self._func_declaration(),
            'imports': "\n".join(
                ["if(!PyImport_ImportModule(\"pyllars::%s\")){return -1;} " % n.replace("::", ".") for n in
                 imports]),
            'name': self.sanitize(self.element.name),
            'pyllars_scope': self.element.pyllars_scope,
            'pyllars_block_scope': self.element.pyllars_block_scope,
            'pyname': CXXMethodDecl.METHOD_NAMES.get(self.element.name).replace('addMethod', '') if
            self.element.name in CXXMethodDecl.METHOD_NAMES else self.element.name if self.element.name != "operator=" else "assign_to",
            'template_decl': template_decl(self),
            'throws': "" if self.element.throws is None else "void" if len(self.element.throws) == 0
                         else ",".join(self.element.throws),
        }).encode('utf-8'))


class FieldDecl(Generator):
    @classmethod
    def is_generatable(cls):
        return True

    def generate_body_proper(self, stream: TextIOBase, as_top: bool = False) -> None:
        if not self.element.name and not self.element.type_.name:
            raise Exception("Anonymously typed anonymously name field encountered")

        imports = set([])

        if self.element.type_ and self.element.type_.namespace_name != self.element.parent.namespace_name:
            imports.add(self.element.namespace_name)
        if self.element.bit_size is None:
            if self.element.type_.array_size is not None:
                stream.write(("""
                    constexpr cstring name = "%(name)s";
                    //generated from %(file)s.generate_body_proper #1
                    // FUNCTION %(name)s THROWS
                    
                    %(template_decl)s
                    status_t %(pyllars_scope)s::%(basic_name)s_init(PyObject * const global_mod){
                       status_t status = 0;
                       %(imports)s
                        __pyllars_internal::PythonClassWrapper< %(typename)s %(scope)s >::template addArrayAttribute<name, %(array_size)s, %(full_type_name)s>
                           ( &%(scope)s::%(name)s, %(array_size)s);
                       return status;
                    }
                    
                %(template_decl)s
                typename %(pyllars_scope)s::Initializer_%(basic_name)s
                * %(pyllars_scope)s::Initializer_%(basic_name)s::initializer = 
                new %(pyllars_scope)s::Initializer_%(basic_name)s();
    """ % {
                    'file': __file__,
                    'basic_name': self.element.basic_name,
                    'imports': "\n".join(
                        ["if(!PyImport_ImportModule(\"pyllars::%s\")){return -1;} " % n.replace("::", ".") for n in
                         imports]),
                    'name': self.sanitize(self.element.name),
                    'pyllars_scope': self.element.pyllars_scope,
                    'full_type_name': self.element.type_._target_type.full_param_name,
                    'scope': self.element.block_scope,
                    'array_size': self.element.type_.array_size,
                    'template_decl': template_decl(self),
                    'typename': 'typename' if not self.element.parent.is_union else ""
                }).encode('utf-8'))
            else:
                stream.write(("""
                    constexpr cstring name = "%(name)s";
                    //generated from %(file)s.generate_body_proper #2
                    // FUNCTION %(name)s THROWS
                    
                    %(template_decl)s
                    status_t %(pyllars_scope)s::%(basic_name)s_init(PyObject* const global_mod){
                       status_t status = 0;
                       %(imports)s
                        __pyllars_internal::PythonClassWrapper< %(typename)s %(scope)s >::template addAttribute%(qual)s<name, decltype(%(scope)s::%(name)s)>
                           ( &%(scope)s::%(name)s);
                       return status;
                    }
                   
                    %(template_decl)s
                    status_t %(pyllars_scope)s::%(basic_name)s_register(pyllars::Initializer*){
                            //do nothing, functions have no children
                            return 0;
                    }

                    %(template_decl)s
                    typename %(pyllars_scope)s::Initializer_%(basic_name)s
                    * %(pyllars_scope)s::Initializer_%(basic_name)s::initializer = 
                    new %(pyllars_scope)s::Initializer_%(basic_name)s();
""" % {
                    'file': __file__,
                    'basic_name': self.element.basic_name,
                    'imports': "\n".join(
                        ["if(!PyImport_ImportModule(\"pyllars::%s\")){return -1;} " % n.replace("::", ".") for n in
                         imports]),
                    'name': self.sanitize(self.element.name),
                    'full_type_name': self.element.type_.full_param_name,
                    'pyllars_scope': self.element.pyllars_scope,
                    'qual': 'Const' if self.element.type_.is_const else "",
                    'scope': self.element.block_scope,
                    'template_args': self.element.template_arguments_string(),
                    'template_decl': template_decl(self),
                    'typename': 'typename' if not self.element.parent.is_union else ""
                }).encode('utf-8'))
        else:
            setter_code = """
               static std::function< %(full_type_name)s(%(parent_full_name)s&, %(qual2)s %(full_type_name)s &)> setter =
                   [](%(parent_full_name)s &self, %(qual2)s %(full_type_name)s &v)->%(full_type_name)s{return self.%(name)s = v;};
""" % {
                'parent_full_name': self.element.parent.full_name,
                'qual2': 'const' if not self.element.type_.is_const else "",
                'name': self.element.name,
                'full_type_name': self.element.type_.full_name
            }
            stream.write(("""
                constexpr cstring name = "%(name)s";
                //generated from %(file)s.generate_body_proper #3
                // FUNCTION %(name)s THROWS
                %(template_decl)s
                status_t %(pyllars_scope)s::%(basic_name)s_init(PyObject * const global_mod){
                   status_t status = 0;
                   %(imports)s
                   static std::function< %(full_type_name)s(const %(scope)s&)> getter =
                       [](const %(scope)s &self)->%(full_type_name)s{return self.%(name)s;};
                   %(setter_code)s
                    __pyllars_internal::PythonClassWrapper< %(typename)s %(scope)s >::template addBitField%(qual)s<name, %(full_type_name)s, %(bit_size)s>
                       ( getter %(setter)s);
                   return status;
                }
    
                %(template_decl)s
                status_t %(pyllars_scope)s::%(basic_name)s_register(pyllars::Initializer*){
                    //do nothing, functions have no children
                    return 0;
                }
               
                %(template_decl)s
                typename %(pyllars_scope)s::Initializer_%(basic_name)s
                * %(pyllars_scope)s::Initializer_%(basic_name)s::initializer = 
                new %(pyllars_scope)s::Initializer_%(basic_name)s();
""" % {
                'file': __file__,
                'bit_size': self.element.bit_size,
                'imports': "\n".join(
                    ["if(!PyImport_ImportModule(\"pyllars::%s\")){return -1;} " % n.replace("::", ".") for n in
                     imports]),
                'name': self.sanitize(self.element.name),
                'basic_name': self.element.basic_name,
                'full_name' : self.element.type_.full_name,
                'full_type_name': self.element.type_.full_param_name,
                'pyllars_scope': self.element.pyllars_scope,
                'qual': 'Const' if self.element.type_.is_const else "",
                'qual2': 'const' if not self.element.type_.is_const else "",
                'setter': ", setter" if not self.element.type_.is_const else "",
                'setter_code': setter_code if not self.element.type_.is_const else "",
                'scope': self.element.block_scope,
                'template_args': self.element.template_arguments_string(),
                'template_decl': template_decl(self),
                'typename': 'typename' if not self.element.parent.is_union else ""
            }).encode('utf-8'))


class ClassTemplateDecl(Generator):

    @classmethod
    def is_generatable(cls):
        return True

    @contextmanager
    def templated(self, stream: TextIOBase):
        stream.write(("""
        class %(name)s{
            
            static status_t register_instance(PyObject* const sequence_key,  PyObject* const object);
     
            static status_t %(basic_name)s_register(pyllars::Initializer* const);
            
            static status_t %(basic_name)s_init(PyObject* const global_mod);
            
            %(template_decl)s""" % {
                'basic_name': self.element.basic_name,
                'template_decl': self.template_decl,
                'name': self.element.basic_name,
                'true_name': self.element.basic_name[:-12]
            }).encode('utf-8'))
        yield stream
        stream.write(("""};""").encode('utf-8'))
        return stream

    @property
    def is_namespace(self):
        return True

    def generate_header_core(self, stream: TextIOBase, as_top=False):
        #super(ClassTemplateDecl, self).generate_header_core(scoped, as_top)
        pass

    def generate_body_proper(self, scoped: TextIOBase, as_top: bool = False) -> None:
        if self.element.is_implicit:
            return

        scoped.write(("                //From: %(file)s:generate_body_proper\n" % {
            'file': os.path.basename(__file__),
        }).encode('utf-8'))
        if isinstance(self.element.parent, parser.NamespaceDecl):
            code_for_adding_dict = """ PyModule_AddObject(%(module_name)s, "%(name)s", dictionary);
            """ % {
                'name': self.element.basic_name or "_anonymous%s" % self.element.tag,
                'module_name': self.element.parent.pyllars_module_name,
                'full_name': self.element.full_name,
            }
        else:
            code_for_adding_dict = """__pyllars_internal::PythonClassWrapper< %(typename)s %(parent_class_scope)s::%(parent_class_name)s >::addClassMember
                        ("%(name)s",
                         (PyObject*) dictionary);
            """ % {
                'parent_class_name': self.element.parent.name,
                'parent_class_scope': self.element.parent.scope,
                'name': self.element.name,
                'typename': 'typename' if not self.element.is_union else ""
            }
        scoped.write(("""
            %(template_decl)s
            status_t %(pyllars_scope)s::%(basic_name)s::register_instance(PyObject* const sequence_key,  PyObject* const object){
                static PyObject* dictionary = PyDict_New();
                static status_t status = %(add_text)s
                PyDict_SetItem(dictionary, sequence_key, object);
            }
            
           
""" % {
            'name': self.sanitize(self.element.name if self.element.name else "pyllars"),
            'basic_name' : self.element.basic_name,
            'parent': self.element.scope if self.element.scope != '::' else "",
            'pyllars_scope': self.element.pyllars_scope,
            'template_decl': template_decl(self.parent) if self.parent else "",
            'add_text': code_for_adding_dict,
        }).encode('utf-8'))


class CXXConstructorDecl(CXXMethodDecl):

    @classmethod
    def is_generatable(cls):
            return True

    def generate_body_proper(self, stream: TextIOBase, as_top: bool = False) -> None:
        imports = set([])
        for elem in self.element.params:
            if elem and elem.type_.namespace_name != self.element.namespace_name and elem.type_.namespace_name != "::":
                imports.add(elem.namespace_name)
        if self.element.return_type and \
                        self.element.return_type.namespace_name != self.element.namespace_name and self.element.return_type.namespace_name != "::":
            imports.add(self.element.return_type.namespace_name)

        stream.write(("""

            //generated from %(file)s.generate_body_proper
            // CONSTRUCTOR %(name)s THROWS %(throws)s
            %(template_decl)s
            status_t %(pyllars_scope)s::%(sanitized_name)s::%(sanitized_name)s_init(PyObject * const global_mod){
               static const char* const argumentNames[] = {%(argument_names)s nullptr};
               status_t status = 0;
               %(imports)s
               %(func_decl)s
               return status;
            }

            %(template_decl)s
            status_t %(pyllars_scope)s::%(sanitized_name)s::%(sanitized_name)s_register(pyllars::Initializer* const init){
                return 0;
            }
            
            %(template_decl)s
            typename %(pyllars_scope)s::%(sanitized_name)s::Initializer_%(sanitized_name)s 
            *%(pyllars_scope)s::%(sanitized_name)s::Initializer_%(sanitized_name)s::initializer = 
            new %(pyllars_scope)s::%(sanitized_name)s::Initializer_%(sanitized_name)s();
        """ % {
            'arguments': (',' if len(self.element.params) > 0 else "") + ', '.join([t.type_.full_name for
                                                                                    t in self.element.params]),
            'argument_names': ','.join(
                ["\"%s\"" % (arg.name if arg.name else "_%s" % (index + 1)) for index, arg in
                 enumerate(self.element.params)]) + (',' if self.element.params else ''),
            'sanitized_name': self.sanitize(self.element.basic_name),
            'file': __file__,
            'func_decl': self._func_declaration(),
            'imports': "\n".join(
                ["if(!PyImport_ImportModule(\"pyllars::%s\")){return -1;} " % n.replace("::", ".") for n in
                 imports]),
            'name': self.sanitize(self.element.name),
            'pyllars_scope': self.element.pyllars_scope,
            'pyname': CXXMethodDecl.METHOD_NAMES.get(self.element.name).replace('addMethod', '') if
            self.element.name in CXXMethodDecl.METHOD_NAMES else self.element.name if self.element.name != "operator=" else "assign_to",
            'template_decl': template_decl(self),
            'throws': "" if self.element.throws is None else "void" if len(self.element.throws) == 0
            else ",".join(self.element.throws),
        }).encode('utf-8'))

    def _func_declaration(self):
        #if self.element.has_varargs:
        #    method_name += "Varargs"

        return """
    __pyllars_internal::PythonClassWrapper< %(typename)s %(scope)s>::template addConstructor<%(args)s>
       ( argumentNames);

""" % {
            'args': ", ".join([p.type_.full_param_name for p in self.element.params]),
            'basic_name': self.element.parent.basic_name,
            'is_const': str(self.element.is_const).lower() + "," if not self.element.is_static else "",
            'names': ",".join(['"%s"' % (e.name or "param_%d" % index) for index, e in enumerate(self.element.params)]),
            'scope': self.element.block_scope,
            'template_prefix': 'template ' if self.element.parent.parent.is_template_macro else "",
            'typename': 'typename' if not self.element.parent.is_union else ""
        }


class EnumDecl(CXXRecordDecl):

    @classmethod
    def is_generatable(cls):
        return True


class EnumConstantDecl(VarDecl):

    @classmethod
    def is_generatable(cls):
        return True

    def generate_body_proper(self, scoped: TextIOBase, as_top: bool = False) -> None:
        if self.element.parent and isinstance(self.element.parent.parent, parser.ClassTemplateDecl):
            raise Exception("NOT IMPL")
        scoped.write(("\n                    //generated rom: %(file)s:VarDecl.generate_body_proper\n" % {
            'file': os.path.basename(__file__),
        }).encode('utf-8'))
        imports = set([])
        if self.element.type_ and self.element.type_.namespace_name != self.element.parent.namespace_name:
            imports.add(self.element.namespace_name)
        if isinstance(self.element.parent, parser.RecordTypeDefn):
            # static class member var:
            scoped.write(("""
                constexpr cstring name = "%(basic_name)s";
                constexpr cstring type_name = "%(parent_name)s";
                status_t %(pyllars_scope)s::%(basic_name)s::%(basic_name)s_init(PyObject * const global_mod){
                    __pyllars_internal::PythonClassWrapper<%(parent_full_name)s>::initialize("%(parent_basic_name)s",
                                                                  "%(parent_basic_name)s",
                                                                  %(type_mod)s,
                                                                  "%(parent_full_name)s");
                    return __pyllars_internal::PythonClassWrapper<%(parent_full_name)s>::%(prefix)s addEnum%(suffix)s(name, %(full_name)s);
                }

                status_t %(pyllars_scope)s::%(basic_name)s::%(basic_name)s_register( pyllars::Initializer* const){
                    status_t status = 0;
                    // do nothing
                    return status;
                }

                 %(template_decl)s
                 %(pyllars_scope)s::%(basic_name)s%(template_args)s::Initializer_%(basic_name)s
                 *%(pyllars_scope)s::%(basic_name)s%(template_args)s::Initializer_%(basic_name)s::initializer = 
                 new %(pyllars_scope)s::%(basic_name)s%(template_args)s::Initializer_%(basic_name)s();

""" % {
                'pyllars_scope': self.element.pyllars_scope,
                'basic_name': self.element.basic_name,
                'full_name': self.element.full_name or "anonymous_%s" % self.element.tag,
                'parent': self.element.parent.full_name,
                'parent_name': qualified_name(self.element.parent.name if self.element.parent.name != '::' else ''),
                'parent_full_name': self.element.parent.full_name,
                'parent_basic_name': self.element.parent.basic_name,
                'full_type_name': self.element.type_.full_name,
                'qual': 'Const' if self.element.type_.is_const else '',
                'prefix': "" if not self.element.is_structure else "template ",
                'suffix': "Value" if not self.element.is_structure else "ClassValue<type_name, %s>" % self.element.parent.full_name,
                'template_decl': template_decl(self),
                'template_args': self.element.template_arguments_string(),
                'type_mod': self.element.pyllars_scope + "::" + self.element.parent.parent.name + "_mod" if self.element.parent.parent.name != '' else "global_mod",
            }).encode('utf-8'))
        elif isinstance(self.element.parent, parser.NamespaceDecl):
            # global or namespace var:
            scoped.write(("""
                constexpr cstring name = "%(name)s";

                %(template_decl)s
                status_t %(pyllars_scope)s::%(basic_name)s::%(basic_name)s_init(PyObject * const global_mod){
                    status_t status = 0;
                    PyObject* mod = %(module_name)s;
                    PyObject* value = PyInt_FromLong(static_cast<long int>(%(parent_full_name)s::%(basic_name)s));
                    if (!mod || !value){
                        status = -1;
                    } else {
                        status = PyModule_AddObject(mod, "%(basic_name)s", value);
                    }
                    return status;
                }


                status_t %(pyllars_scope)s::%(basic_name)s::%(basic_name)s_register( pyllars::Initializer* const){
                    status_t status = 0;
                    // do nothing
                    return status;
                }

                 %(template_decl)s
                 %(pyllars_scope)s::%(basic_name)s%(template_args)s::Initializer_%(basic_name)s
                 *%(pyllars_scope)s::%(basic_name)s%(template_args)s::Initializer_%(basic_name)s::initializer = 
                 new %(pyllars_scope)s::%(basic_name)s%(template_args)s::Initializer_%(basic_name)s();
""" % {
                'pyllars_scope': self.element.pyllars_scope,
                'basic_name': self.element.basic_name,
                'name': self.element.full_name or "anonymous_%s" % self.element.tag,
                'parent_full_name': self.element.parent.full_name if self.element.parent.full_name != '::' else "",
                'parent_name': qualified_name(self.element.parent.name if self.element.parent.name else "pyllars"),
                'module_name': self.element.parent.pyllars_module_name,
                'full_type_name': self.element.type_.full_name,
                'template_args': self.element.template_arguments_string(),
                'template_decl': template_decl(self)
            }).encode('utf-8'))
        else:
            logging.error("Unknown parent type for global var")