from io import TextIOBase

import os

from .base import Folder, Generator, qualified_name
from . import parser


class CXXRecordDecl(Generator):

    def __init__(self, src_path, indent=""):
        super().__init__(src_path, indent="")

    @classmethod
    def is_generatable(cls):
        return True

    def generate_body_proper(self, element: parser.CXXRecordDecl, stream: TextIOBase, src_path, as_top: bool = False) -> None:
        if element.is_implicit:
            return

        with self.scoped(element, stream) as scoped:
            scoped.write(("                //From: %(file)s:generate_body_proper\n" % {
                'file': os.path.basename(__file__),
            }).encode('utf-8'))

            full_class_name = element.full_name
            class_name = "main_type"
            if element.is_template:
                scoped.write(b"""
                static PyObject* const instances_dict = PyDict_New();
                """)
            scoped.write(("""
                %(template_decl)s
                static status_t init_me(){
                    using namespace __pyllars_internal;
                    typedef %(full_class_name)s%(template_args)s main_type;
                    static status_t _status = -1;
                    static bool inited = false;
                    if (inited){
                        return _status;
                    }
                    inited = true;
                    status_t status = 0;
    """ % {
                'qname': qualified_name(element.name or "anonymous_%s" % element.tag),
                'name': element.name or "anonymous_%s" % element.tag,
                'indent': self._indent,
                'full_class_name': full_class_name,
                'template_decl': element.template_decl(),
                'template_args': element.template_arguments(),
            }).encode('utf-8'))

            for base in element.public_base_classes or []:
                stream.write(("""
                    status |= pyllars%(base_class_name)s_init();
                     __pyllars_internal::PythonClassWrapper<%(full_class_name)s>::addBaseClass
                        (&PythonClassWrapper< %(base_class_name)s >::Type); /*1*/
    """ % {
                    'full_class_name': class_name,
                    'class_name': element.name,
                    'base_class_name': qualified_name(base.full_name),
                }).encode('utf-8'))

            if element.name:
                if isinstance(element.parent, parser.NamespaceDecl):
                    stream.write(("""
                    status |=  __pyllars_internal::PythonClassWrapper< %(class_name)s >::initialize(
                                 "%(name)s",
                                 "%(name)s",
                                 %(module_name)s,
                                 "%(full_name)s");  //classes
        """ % {
                        'class_name': class_name,
                        'name': element.name or "_anonymous%s" % element.tag,
                        'module_name': element.parent.pyllars_module_name,
                        'full_name': element.full_name,
                    }).encode('utf-8'))
                else:
                    stream.write(("""
                     __pyllars_internal::PythonClassWrapper< %(parent_class_name)s%(template_args)s >::addClassMember
                        ("%(name)s",
                         (PyObject*) & __pyllars_internal::PythonClassWrapper< %(class_name)s%(template_args)s >::Type);
""" % {
                        'parent_class_name': element.parent.full_name,
                        'name': element.name,
                        'class_name': element.full_name,
                        'template_args': element.template_arguments(),
                    }).encode('utf-8'))
            stream.write(("""
                    _status = status;
                    return status;
                }

                %(template_decl)s
                int %(qname)s_register( pyllars::Initializer* const init ){
                    static pyllars::Initializer _initializer = pyllars::Initializer();
                    static int status = pyllars%(parent)s::%(parent_name)s_register(&_initializer);
                    return status==0?_initializer.register_init(init):status;
                 }

                namespace{
                    %(template_decl)s
                    class Initializer: public pyllars::Initializer{
                    public:
                       Initializer():pyllars::Initializer(){
                           pyllars%(parent)s::%(parent_name)s_register(this);
                       }
                       virtual int init(){
                           int status = pyllars::Initializer::init();
                           return status | init_me();
                       }
                    };

                    static Initializer init = Initializer();
                }
""" % {
                'indent': self._indent,
                'name': self.sanitize(element.name if element.name else "pyllars"),
                'qname': qualified_name(element.name if element.name else "pyllars"),
                'parent_name': qualified_name
                    (element.parent.name if (element.parent.name and element.parent.name != "::")
                                              else "pyllars"),
                'parent': element.scope if element.scope != '::' else "",
                'template_decl': element.template_decl(),
                'template_args': element.template_arguments(),
            }).encode('utf-8'))




class CXXMethodDecl(Generator):
    METHOD_NAMES = {'operator-': ['addMethod__inv__', 'addMethod__sub__'],
                    'operator+': ['addMethod__pos__', 'addMethod__add__'],
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

    def generate_body_proper(self, element: parser.CXXMethodDecl, stream: TextIOBase, src_path,
                                 as_top: bool = False) -> None:
        if element.is_static:
            base = "addClass"
        else:
            base = "add"
        method_name = CXXMethodDecl.METHOD_NAMES.get(element.name) or ("%sMethod" % base)
        if element.has_varargs:
            method_name += "Varargs"
        with self.scoped(element, stream) as scoped:
            imports = set([])
            for elem in element.params:
                if elem and elem.type_.namespace_name != element.namespace_name and elem.type_.namespace_name != "::":
                    imports.add(elem.namespace_name)
            if element.return_type and \
                            element.return_type.namespace_name != element.namespace_name and element.return_type.namespace_name != "::":
                imports.add(element.return_type.namespace_name)


            scoped.write(("""
                constexpr cstring name = "%(pyname)s";

                //generated from %(file)s.generate_initializer_code
                // FUNCTION %(name)s THROWS %(throws)s
                %(template_decl)s
                static int init_me(){
                   static const char* const argumentNames[] = {%(argument_names)s nullptr};
                   status_t status = 0;
                   %(imports)s
                   %(func_decl)s
                   return status;
                }

                %(template_decl)s
                int %(qname)s_init(){return init_me%(template_args)s();}

                namespace{

                    template< %(template_args)s >
                    class Initializer: public pyllars::Initializer{
                    public:
                        typedef pyllars::Initializer super;

                        Initializer():pyllars::Initializer(){
                            pyllars%(parent)s::%(parent_name)s_register(this);
                        }

                        virtual int init(){
                            int status = super::init();
                            return status | init_me%(template_args)s();
                        }


                    };

                    static Initializer init = Initializer();
                }
""" % {
                'indent': self._indent,
                'file': __file__,
                'imports': "\n".join(
                    ["if(!PyImport_ImportModule(\"pylllars%s\")){return -1;} " % n.replace("::", ".") for n in
                     imports]),
                'module_name': element.parent.pyllars_module_name,
                'name': self.sanitize(element.name),
                'qname': qualified_name(element.name),
                'pyname': CXXMethodDecl.METHOD_NAMES.get(element.name).replace('addMethod', '') if
                element.name in CXXMethodDecl.METHOD_NAMES else element.name if element.name != "operator=" else "assign_to",
                'parent_name': qualified_name(
                    element.parent.name if (element.parent.name and element.parent.name != "::")
                    else "pyllars"),
                'parent': self.scope(element),
                'template_decl': element.template_decl(),
                'template_args': element.template_arguments(),
                'argument_names': ','.join(
                    ["\"%s\"" % (arg.name if arg.name else "_%s" % (index + 1)) for index, arg in
                     enumerate(element.params)]) + (',' if element.params else ''),
                'has_varargs': str(element.has_varargs).lower(),
                'throws': "" if element.throws is None else "void" if len(element.throws) == 0
                else ",".join(element.throws),
                'func_decl': self._func_declaration(element) if not element.is_template else "",
                'return_type': element.return_type.full_name if element.return_type else "void",
                'arguments': (',' if len(element.params) > 0 else "") + ', '.join([t.type_.full_name for
                                                                                   t in element.params]),
            }).encode('utf-8'))

    def generate_header_core(self, element: parser.Element, folder: Folder, as_top=False):
        if element.parent and isinstance(element.parent.parent, parser.ClassTemplateDecl):
            #raise Exception("NOT IMPL")
            pass
        else:
            return super(CXXMethodDecl, self).generate_header_core(element, folder, as_top)


class FieldDecl(Generator):
    @classmethod
    def is_generatable(cls):
        return True

    def generate_body_proper(self, element: parser.FieldDecl, stream: TextIOBase, src_path,
                             as_top: bool = False) -> None:
        if not element.name and not element.type_.name:
            raise Exception("Anonymously typed anonymously name field encountered")
        with self.scoped(element, stream) as scoped:

            imports = set([])

            if element.type_ and element.type_.namespace_name != element.parent.namespace_name:
                imports.add(element.namespace_name)
            if element.bit_size is None:
                if element.type_.array_size is not None:
                    scoped.write(("""
                        constexpr cstring name = "%(name)s";
                        //generated from %(file)s.generate_initializer_code
                        // FUNCTION %(name)s THROWS
                        static int init_me(){
                           status_t status = 0;
                           %(imports)s
                            __pyllars_internal::PythonClassWrapper<%(parent_full_name)s>::addAttribute<name, %(array_size)s, %(full_type_name)s>
                               ( &%(parent_full_name)s::%(name)s, %(array_size)s);
                           return status;
                        }
                        int %(qname)s_init(){return init_me();}
                        namespace{
                            class Initializer: public pyllars::Initializer{
                            public:
                               Initializer():pyllars::Initializer(){
                                   pyllars%(parent)s::%(parent_name)s_register(this);
                               }
                               virtual int init(){
                                   int status = pyllars::Initializer::init();
                                   return status | init_me();
                               }
                            };

                            static Initializer init = Initializer();
                        }
        """ % {
                        'indent': self._indent,
                        'file': __file__,
                        'imports': "\n".join(
                            ["if(!PyImport_ImportModule(\"pylllars%s\")){return -1;} " % n.replace("::", ".") for n in
                             imports]),
                        'module_name': element.parent.pyllars_module_name,
                        'name': self.sanitize(element.name),
                        'qname': qualified_name(element.name),
                        'parent_name': qualified_name(element.parent.name),
                        'parent_full_name': element.parent.full_name,
                        'full_type_name': element.type_._base_type.full_name,
                        'parent': self.scope(element),
                        'array_size': element.type_.array_size,
                    }).encode('utf-8'))
                else:
                    scoped.write(("""
                        constexpr cstring name = "%(name)s";
                        //generated from %(file)s.generate_initializer_code
                        // FUNCTION %(name)s THROWS
                        static int init_me(){
                           status_t status = 0;
                           %(imports)s
                            __pyllars_internal::PythonClassWrapper<%(parent_full_name)s>::addAttribute%(qual)s<name, %(full_type_name)s>
                               ( &%(parent_full_name)s::%(name)s);
                           return status;
                        }
                        int %(qname)s_init(){return init_me();}
                        namespace{
                            class Initializer: public pyllars::Initializer{
                            public:
                               Initializer():pyllars::Initializer(){
                                   pyllars%(parent)s::%(parent_name)s_register(this);
                               }
                               virtual int init(){
                                   int status = pyllars::Initializer::init();
                                   return status | init_me();
                               }
                            };

                            static Initializer init = Initializer();
                        }
""" % {
                        'indent': self._indent,
                        'file': __file__,
                        'imports': "\n".join(
                            ["if(!PyImport_ImportModule(\"pylllars%s\")){return -1;} " % n.replace("::", ".") for n in
                             imports]),
                        'module_name': element.parent.pyllars_module_name,
                        'name': self.sanitize(element.name),
                        'qname': qualified_name(element.name),
                        'parent_name': qualified_name(element.parent.name),
                        'parent_full_name': element.parent.full_name,
                        'full_type_name': element.type_.full_name,
                        'parent': self.scope(element),
                        'qual': 'Const' if element.type_.is_const else ""
                    }).encode('utf-8'))
            else:
                setter_code = """
                   static std::function< %(full_type_name)s(%(parent_full_name)s&, %(qual2)s %(full_type_name)s &)> setter =
                       [](%(parent_full_name)s &self, %(qual2)s %(full_type_name)s &v)->%(full_type_name)s{return self.%(name)s = v;};
""" % {
                    'parent_full_name': element.parent.full_name,
                    'qual2': 'const' if not element.type_.is_const else "",
                    'name': element.name,
                    'full_type_name': element.type_.full_name
                }
                scoped.write(("""
                    constexpr cstring name = "%(name)s";
                    //generated from %(file)s.generate_initializer_code
                    // FUNCTION %(name)s THROWS
                    static int init_me(){
                       status_t status = 0;
                       %(imports)s
                       static std::function< %(full_type_name)s(const %(parent_full_name)s&)> getter =
                           [](const %(parent_full_name)s &self)->%(full_type_name)s{return self.%(name)s;};
                       %(setter_code)s
                        __pyllars_internal::PythonClassWrapper<%(parent_full_name)s>::addBitField%(qual)s<name, %(full_type_name)s, %(bit_size)s>
                           ( getter %(setter)s);
                       return status;
                    }
                    int %(qname)s_init(){return init_me();}
                    namespace{
                        class Initializer: public pyllars::Initializer{
                        public:
                           Initializer():pyllars::Initializer(){
                               pyllars%(parent)s::%(parent_name)s_register(this);
                           }
                           virtual int init(){
                               int status = pyllars::Initializer::init();
                               return status | init_me();
                           }
                        };

                        static Initializer init = Initializer();
                    }
""" % {
                    'indent': self._indent,
                    'file': __file__,
                    'bit_size': element.bit_size,
                    'imports': "\n".join(
                        ["if(!PyImport_ImportModule(\"pylllars%s\")){return -1;} " % n.replace("::", ".") for n in
                         imports]),
                    'module_name': element.parent.pyllars_module_name,
                    'name': self.sanitize(element.name),
                    'qname': qualified_name(element.name),
                    'parent_name': qualified_name(element.parent.name),
                    'parent_full_name': element.parent.full_name,
                    'full_type_name': element.type_.full_name,
                    'parent': self.scope(element),
                    'qual': 'Const' if element.type_.is_const else "",
                    'qual2': 'const' if not element.type_.is_const else "",
                    'setter': ", setter" if not element.type_.is_const else "",
                    'setter_code': setter_code if not element.type_.is_const else ""
                }).encode('utf-8'))