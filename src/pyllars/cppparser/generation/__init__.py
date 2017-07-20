import logging
import os
import shlex
import subprocess
import sysconfig
from abc import ABCMeta
from contextlib import contextmanager
from io import TextIOBase
from typing import List

from .. import parser
from ..elements import Element
import pkg_resources

pyllars_resources_dir = pkg_resources.resource_filename("pyllars", os.path.join("..", "resources"))
INDENT = "    "


def qualified_name(name):
    return Generator.sanitize(name.replace("(", "_lparen_").replace(")", "_rparen_").replace(":", "_"))


class Compiler(object):

    CFLAGS = sysconfig.get_config_var('CFLAGS') or ""
    LDFLAGS = sysconfig.get_config_var('LDFLAGS') or ""
    LDCXXSHARED = sysconfig.get_config_var('LDCXXSHARED') or ""
    PYINCLUDE = sysconfig.get_config_var('INCLUDEPY')
    CXX = sysconfig.get_config_var('CXX')
    PYLIB = sysconfig.get_config_var('BLDLIBRARY')

    def __init__(self, folder):
        self._compileables = []
        for root, dirs, files in os.walk(folder):
            for fil in [f for f in files if f.endswith(".cpp")]:
                self._compileables.append(os.path.join(root, fil))
        self._folder = folder

    def compile_all(self, src_path):
        objects = []
        body = src_path.replace(".hpp", ".cpp")
        for compilable in self._compileables:
            target = os.path.join("objects", compilable[10:]).replace(".cpp", ".o")
            if not os.path.exists(os.path.dirname(target)):
                os.makedirs(os.path.dirname(target))
            cmd = "%(cxx)s -std=c++14 %(cxxflags)s -c -fPIC -I%(local_include)s -I%(python_include)s " \
                  "-I%(pyllars_include)s -o %(target)s %(compilable)s" % {
                      'cxx': Compiler.CXX,
                      'cxxflags': Compiler.CFLAGS,
                      'local_include': self._folder,
                      'pyllars_include': pyllars_resources_dir,
                      'python_include': Compiler.PYINCLUDE,
                      'target': target,
                      'compilable': compilable,
                  }
            cmd = cmd.replace("-O2", "-O0")
            print(cmd)
            p = subprocess.Popen(shlex.split(cmd), stdout=subprocess.PIPE,
                                 stderr=subprocess.STDOUT)
            output = p.communicate()[0].decode('utf-8')
            if p.returncode != 0:
                return p.returncode, "Command \"%s\" failed:\n%s" % (cmd, output)
            objects.append(target)
        cmd = "%(cxx)s -fPIC -std=c++14 %(cxxflags)s -I%(python_include)s -shared -o _trial.so -Wl,--no-undefined " \
              "%(src)s %(objs)s %(python_lib_name)s -lffi %(pyllars_include)s/pyllars/pyllars.cpp" % {
                  'cxx': Compiler.LDCXXSHARED,
                  'src': body,
                  'cxxflags': Compiler.CFLAGS,
                  'objs': " ".join(objects),
                  'pyllars_include': pyllars_resources_dir,
                  'python_include': Compiler.PYINCLUDE,
                  'python_lib_name': Compiler.PYLIB,
              }
        print(cmd)
        p = subprocess.Popen(shlex.split(cmd), stdout=subprocess.PIPE,
                             stderr=subprocess.STDOUT)
        output = p.communicate()[0].decode('utf-8')
        return p.returncode, cmd + "\n" + output


class Folder(object):
    def __init__(self, path: str):
        assert os.path.isdir(path)
        self._path = path

    @property
    def path(self):
        return self._path

    def purge(self, file_name: str) -> None:
        full_path = os.path.join(self._path, file_name)
        if os.path.exists(full_path):
            os.remove(full_path)

    @contextmanager
    def open(self, file_name: str) -> TextIOBase:
        open_file = None
        try:
            full_path = os.path.join(self._path, file_name)
            if not os.path.exists(os.path.dirname(full_path)):
                os.makedirs(os.path.dirname(full_path))
            open_file = open(full_path, 'ab')
            yield open_file
        finally:
            if open_file:
                open_file.close()

    def create_subfolder(self, name: str)->"Folder":
        folder_name = os.path.join(self._path, name)
        assert not os.path.isfile(folder_name)
        if not os.path.exists(folder_name):
            os.makedirs(folder_name)
        return Folder(folder_name)


class Generator(metaclass=ABCMeta):
    def __init__(self, src_path, indent=""):
        self._indent = indent
        self._src_path = src_path

    @classmethod
    def is_generatable(cls):
        return False

    @classmethod
    def to_path(cls, name: str, ext="") -> str:
        return name.replace(" ", "_") + ext

    @classmethod
    def header_file_path(cls, element: Element) -> str:
        header_name = cls.header_file_name(element)
        if isinstance(element, parser.NamespaceDecl):
            paths = [p for p in element.parent.namespace_name.split('::') if p != ''] if element.parent else []
            path = os.path.join(*paths, header_name) if paths else header_name
        else:
            path = header_name
            parent = element.parent
            while parent and element.parent.name:
                path = os.path.join(parent.name, path)
                parent = parent.parent
        parent_path = os.path.basename(path)
        if not os.path.exists(parent_path):
            os.makedirs(parent_path)
        return path

    @classmethod
    def sanitize(cls, text):
        for index, c in enumerate(['-', '=', '&', '<', '>', '*', '&', '|', '!', '^', '[', ']', '/']):
            text = text.replace(c, "_op____%d" % index)
        return text

    @classmethod
    def header_file_name(cls, element: parser.Element)->str:
        return (element.name or "global") + ".hpp"

    @classmethod
    def body_file_path(cls, element: parser.Element) ->str:
        parent_path = os.path.dirname(cls.header_file_path(element.parent)) if element.parent is not None else "."
        if not os.path.exists(parent_path):
            os.makedirs(parent_path)
        return os.path.join(parent_path, cls.to_path(element.name or "global", ext=".cpp"))

    @classmethod
    def body_file_name(cls, element: parser.Element)->str:
        return (element.name or "global") + ".cpp"

    def scope(self, element):
        return element.scope

    def generate_header_code(self, element: Element, folder: Folder) -> None:
        folder.purge(self.header_file_name(element))
        with folder.open(file_name=self.header_file_name(element)) as stream:

            stream.write(("""
    #include <Python.h>
    #include <pyllars/pyllars.hpp>
    #include <pyllars/pyllars_classwrapper.cpp>
    #include <pyllars/pyllars_function_wrapper.hpp>

    #include <%s>
""" % self._src_path).encode('utf-8'))
            namespace_text, namespace_closure = self.namespaces(element)
            stream.write(("""
%s
    %s
%s
""" % (namespace_text, self.generate_header_core(element, folder), namespace_closure)).encode('utf-8'))

    def generate_body_code(self, element: parser.Element, folder: Folder, src_path: str, as_top: bool) -> None:
        self.generate_body_proper(element, folder, src_path, as_top)
        subfolder = folder.create_subfolder(element.name) if element.name != "::"  and element.name else folder
        if not isinstance(element, parser.CXXMethodDecl) and not isinstance(element, parser.FunctionDecl):
            for child in element.children():
                generator = self.get_generator(type(child), src_path, "")
                if generator:
                    generator.generate_body(child, subfolder, src_path=self._src_path)

    def generate_header_core(self, element: Element, folder: Folder, as_top=False):
        with folder.open(self.header_file_name(element)) as stream:
            stream.write("".encode('utf-8'))

    @classmethod
    def generate_spec(cls, top_element: Element, folder: Folder, src_path: str):
        from ..parser import NamespaceDecl
        if isinstance(top_element, NamespaceDecl):
            folder = folder.create_subfolder(cls.to_path(top_element.name))
        generator = cls.get_generator(type(top_element), src_path, "")
        if not generator:
            return
        file_name = cls.to_path(top_element.name or "global", ext=".hpp")
        folder.purge(file_name)
        with folder.open(file_name=file_name) as stream:
            if not top_element.name:
                stream.write(b"")
                return
            namespace_text, namespace_closure = generator.namespaces(top_element)
            stream.write(("""
#ifndef __%(guard)s__
#define __%(guard)s__

""" % {'guard': top_element.guard}).encode('utf-8'))
            if top_element.parent:
                stream.write(("""
#include "%(target_file_name)s"
#include "%(parent_header_name)s"
""" % {
                    'parent_header_name': cls.header_file_path(top_element.parent),
                    'target_file_name': src_path}).encode("utf-8"))
            stream.write(("""
namespace pyllars{
%(namespaces)s
  namespace %(qname)s{
    int %(qname)s_register( pyllars::Initializer* const);
    int %(qname)s_init();
  }
%(closure)s
}
#endif
            """ % {
                    'qname': qualified_name(top_element.name),
                'name': cls.sanitize(top_element.name),
                'qname': qualified_name(top_element.name),
                'namespaces': namespace_text,
                'closure': namespace_closure}).encode('utf-8'))

    @classmethod
    def generate_body(cls, element: Element, folder: Folder, src_path: str, as_top=False):
        if not element.name and not as_top:
            return
        generator = cls.get_generator(type(element), src_path, "")
        if not generator:
            return
        file_name = cls.to_path(element.name or "global", ext=".cpp")
        folder.purge(file_name)
        cls.generate_spec(element, folder, src_path=src_path)
        with folder.open(file_name=file_name) as stream:
            stream.write(("""
#include <Python.h>
#include <pyllars/pyllars.hpp>
#include <pyllars/pyllars_classwrapper.cpp>
#include <pyllars/pyllars_function_wrapper.hpp>

#include "%(my_header_name)s"
            """ % {"my_header_name": cls.header_file_name(element)}).encode('utf-8'))
        generator.generate_body_headers(element, folder)
        generator.generate_body_code(element, folder, src_path, as_top)

    @staticmethod
    def get_generator(clazz: type, src_path: str, indent: str) -> "Generator":
        generator_class = globals().get(clazz.__name__)
        if not generator_class or not generator_class.is_generatable():
            return None
        if generator_class:
            return generator_class(src_path, indent)
        else:
            logging.error("Did not find generator for class %s" % clazz.__name__)
            return Generator(src_path, indent)

    @staticmethod
    def namespaces(element: parser.Element) -> List[str]:
        namespaces = []
        parent = element.parent
        while parent:
            if parent.name:
                namespaces = [qualified_name(parent.name)] + namespaces
            parent = parent.parent
        namespace_text = "\n".join([INDENT*index + "namespace %s{" % n for index, n in enumerate(namespaces)])
        closure = "\n".join([INDENT*index + '}' for index in range(len(namespaces))]) + "\n"
        return namespace_text, closure

    def generate_body_headers(self, element: Element, folder: Folder):
        pass

    def generate_body_proper(self, element: Element, folder: Folder, src_path, as_top: bool = False):
        pass


class NamespaceDecl(Generator):

    @classmethod
    def is_generatable(cls):
        return True

    def generate_header_core(self, element: Element, folder: Folder, as_top=False) -> None:
        with folder.open(self.header_file_name(element)) as stream:
            stream.write(("""
    extern PyModuleObject *%s_mod;
""" % element.name).encode('utf-8'))

    @classmethod
    def generate_spec(cls, element: parser.Element, folder: Folder, src_path: str):
        file_name = cls.to_path(element.name or "global", ext=".hpp")
        if element.name == "::" or not element.name:
            with folder.open(file_name=file_name) as stream:
                stream.write("")
                return
        generator = cls.get_generator(type(element), src_path, "")
        if not generator:
            return
        folder.purge(file_name)
        with folder.open(file_name=file_name) as stream:
            namespace_text, namespace_closure = generator.namespaces(element)
            stream.write(("""
#ifndef __%(guard)s__
#define __%(guard)s__

""" % {'guard': element.guard}).encode('utf-8'))
            if element.parent:
                stream.write(("""
#include "%(parent_header_name)s"
#include "%(target_file_name)s"
""" % {
                    'parent_header_name': cls.header_file_path(element.parent),
                    'target_file_name': src_path}).encode("utf-8"))
            stream.write(("""
namespace pyllars{
%(namespaces)s
    namespace %(qname)s{
        int %(qname)s_register( pyllars::Initializer* const);
        extern PyObject* %(name)s_mod;
    }
%(closure)s
}
#endif
            """ % {'name': cls.sanitize(element.name),
                   'qname': qualified_name(element.name),
                   'namespaces': namespace_text,
                   'closure': namespace_closure,
                   }).encode('utf-8'))

    def generate_body_proper(self, element: Element, folder: Folder, src_path, as_top: bool = False) -> None:
        if not element.parent:
            return

        with folder.open(self.body_file_name(element)) as stream:
            generator = self.get_generator(type(element), ".", "")
            namespace_text, namespace_closure = generator.namespaces(element)
            stream.write(("""
namespace pyllars{
    %(indent)s%(namespaces)s
    %(indent)snamespace %(qname)s{
    %(indent)s    status_t init_me(){
    %(indent)s        static PyObject* pyllars_mod = PyImport_ImportModule("pyllars");
    %(indent)s        if (%(name)s_mod) return 0;// if already initialized
    %(indent)s        int status = 0;
    %(indent)s        #if PY_MAJOR_VERSION==3
    %(indent)s        static PyModuleDef %(name)s_moddef = {
    %(indent)s            PyModuleDef_HEAD_INIT,
    %(indent)s            "noddy",
    %(indent)s            "Example module that creates an extension type.",
    %(indent)s            -1,
    %(indent)s            NULL, NULL, NULL, NULL, NULL
    %(indent)s        };
    %(indent)s        %(name)s_mod = PyModule_Create(&%(name)s_moddef);
    %(indent)s        #else
    %(indent)s        %(name)s_mod = Py_InitModule3("%(name)s", nullptr,
    %(indent)s                                      "Module corresponding to C++ namespace %(fullname)s");
    %(indent)s        #endif
    %(indent)s        if(! %(name)s_mod ){
    %(indent)s            status = -1;
    %(indent)s        }
    %(indent)s        return status;
    %(indent)s    } // end init

    %(indent)sint %(qname)s_register( pyllars::Initializer* const init ){
    %(indent)s    static pyllars::Initializer _initializer = pyllars::Initializer();
    %(indent)s    static int status = pyllars%(parent_name)s::%(parent)s_register(&_initializer);
    %(indent)s    return status==0?_initializer.register_init(init):status;
    %(indent)s }
    %(indent)s PyObject *%(name)s_mod = nullptr;
    %(indent)s}
    %(indent)sint %(name)s_init(){return %(name)s::init_me();}
    %(indent)s%(closure)s
}
    """ % {
                'indent': self._indent,
                'name': self.sanitize(element.name),
                'qname': qualified_name(element.name),
                'fullname': element.full_name,
                'parent': qualified_name(element.parent.name if element.parent.name and element.parent.name != "::" else "pyllars"),
                'parent_name': (element.scope if element.scope != '::' else ""),
                'namespaces': namespace_text,
                'closure': namespace_closure}).encode('utf-8'))


class FunctionDecl(Generator):

    @classmethod
    def is_generatable(cls):
        return True

    def _func_declaration(self, element: parser.FunctionElement):
        has_varargs = element.has_varargs
        altenative_code = element.full_name
        argslambda = ", ".join([arg.as_function_argument(index, typed=True) for
                                index, arg in enumerate(element.params)])
        lambda_code = "[](%(argslambda)s)->%(return_type)s{%(return_cast)s %(func_name)s(%(params)s);}" % {
            'return_type': element.return_type.full_name,
            'func_name': element.full_name,
            'return_cast': "" if element.return_type is None or element.return_type.name == "void" else
            "return (%s)" % element.return_type.name,
            'argslambda': argslambda,
            'params': ", ".join(["%s" % arg.name if arg.name else
                                 "p%s" % index for index, arg in enumerate(element.params)]),
        } if not has_varargs else altenative_code
        return """
%(indent)s   __pyllars_internal::FuncContainer<%(has_varargs)s,
%(indent)s                                     %(return_type)s %(arguments)s>::Type<0,
%(indent)s                                     %(throws)s> func_container;
%(indent)s   func_container._cfunc = %(lambdacode)s;
%(indent)s   return  PyModule_AddObject(
%(indent)s                %(module_name)s, "%(func_name)s",
%(indent)s                (PyObject*)__pyllars_internal::PythonFunctionWrapper<__pyllars_internal::
%(indent)s                is_complete< %(return_type)s >::value, %(has_varargs)s, %(return_type)s %(arguments)s>::
%(indent)s                template Wrapper<%(throws)s>::create("%(func_name)s", func_container, argumentNames));
""" % {
            'indent': self._indent,
            'module_name': element.parent.pyllars_module_name,
            'return_type': element.return_type.full_name,
            'arguments': (',' if len(element.params) > 0 else "") + ', '.join([t.type_.full_name for
                                                                                   t in element.params]),
            'lambdacode': lambda_code,
            'has_varargs': str(element.has_varargs).lower(),
            'throws': "" if element.throws is None else "void" if len(element.throws) == 0
            else ",".join(element.throws),
            'func_name': element.name,
        }

    def generate_body_proper(self, element: parser.FunctionElement, folder: Folder, src_path, as_top: bool = False) -> None:
        with folder.open(self.body_file_name(element)) as stream:
            namespace_text, namespace_closure = self.namespaces(element)
            imports = set([])
            for elem in element.params:
                if elem and elem.type_.namespace_name != element.namespace_name and elem.type_.namespace_name != "::":
                    imports.add(elem.namespace_name)
            if element.return_type and \
              element.return_type.namespace_name != element.namespace_name and element.return_type.namespace_name != "::":
                imports.add(element.return_type.namespace_name)
            stream.write(("""
constexpr cstring name = "%(pyname)s";
namespace pyllars{
    %(indent)s%(namespaces)s
%(indent)s//generated from %(file)s.generate_initializer_code
%(indent)s// FUNCTION %(name)s THROWS %(throws)s
%(indent)sstatic int init_me(){
%(indent)s   static const char* const argumentNames[] = {%(argument_names)s nullptr};
%(indent)s   status_t status = 0;
%(indent)s   %(imports)s
%(indent)s   %(func_decl)s
%(indent)s   return status;
%(indent)s}
%(indent)sint %(qname)s_init(){return init_me();}
%(indent)snamespace{
%(indent)s    class Initializer: public pyllars::Initializer{
%(indent)s    public:
%(indent)s       Initializer():pyllars::Initializer(){
%(indent)s           pyllars%(parent)s::%(parent_name)s_register(this);
%(indent)s       }
%(indent)s       virtual int init(){
%(indent)s           int status = pyllars::Initializer::init();
%(indent)s           return status | init_me();
%(indent)s       }
%(indent)s    };
%(indent)s
%(indent)s    static Initializer init = Initializer();
%(indent)s}
%(indent)s
%(indent)s%(closure)s
}
""" % {
                'indent': self._indent,
                'file': __file__,
                'imports': "\n".join(["if(!PyImport_ImportModule(\"pylllars%s\")){return -1;} " % n.replace("::", ".") for n in imports]),
                'module_name': element.parent.pyllars_module_name,
                'name': self.sanitize(element.name),
                'qname': qualified_name(element.name),
                'pyname': CXXMethodDecl.METHOD_NAMES.get(element.name).replace('addMethod', '') if
                element.name in CXXMethodDecl.METHOD_NAMES else element.name if element.name != "operator=" else "assign_to",
                'namespaces': namespace_text,
                'parent_name': qualified_name(element.parent.name if (element.parent.name and element.parent.name != "::")
                                              else "pyllars"),
                'parent': self.scope(element),
                'argument_names': ','.join(["\"%s\"" % (arg.name if arg.name else "_%s" % (index + 1)) for index, arg in
                                            enumerate(element.params)]) + (',' if element.params else ''),
                'has_varargs': str(element.has_varargs).lower(),
                'throws': "" if element.throws is None else "void" if len(element.throws) == 0
                else ",".join(element.throws),
                'func_decl': self._func_declaration(element),
                'return_type': element.return_type.full_name if element.return_type else "void",
                'arguments': (',' if len(element.params) > 0 else "") + ', '.join([t.type_.full_name for
                                                                                   t in element.params]),
                'closure': namespace_closure
                }).encode('utf-8'))

class FieldDecl(Generator):

    @classmethod
    def is_generatable(cls):
        return True

    def generate_body_proper(self, element: parser.FieldDecl, folder: Folder, src_path, as_top: bool = False) -> None:
        if not element.name and not element.type_.name:
            raise Exception("Anonymously typed anonymously name field encountered")
        with folder.open(self.body_file_name(element)) as stream:

            namespace_text, namespace_closure = self.namespaces(element)
            imports = set([])

            if element.type_ and element.type_.namespace_name != element.parent.namespace_name:
                imports.add(element.namespace_name)
            if element.bit_size is None:
                if element.type_.array_size is not None:
                    stream.write(("""
namespace pyllars{
    %(indent)s%(namespaces)s
constexpr cstring name = "%(name)s";
%(indent)s//generated from %(file)s.generate_initializer_code
%(indent)s// FUNCTION %(name)s THROWS
%(indent)sstatic int init_me(){
%(indent)s   status_t status = 0;
%(indent)s   %(imports)s
%(indent)s    __pyllars_internal::PythonClassWrapper<%(parent_full_name)s>::addAttribute<name, %(array_size)s, %(full_type_name)s>
%(indent)s       ( &%(parent_full_name)s::%(name)s, %(array_size)s);
%(indent)s   return status;
%(indent)s}
%(indent)sint %(qname)s_init(){return init_me();}
%(indent)snamespace{
%(indent)s    class Initializer: public pyllars::Initializer{
%(indent)s    public:
%(indent)s       Initializer():pyllars::Initializer(){
%(indent)s           pyllars%(parent)s::%(parent_name)s_register(this);
%(indent)s       }
%(indent)s       virtual int init(){
%(indent)s           int status = pyllars::Initializer::init();
%(indent)s           return status | init_me();
%(indent)s       }
%(indent)s    };
%(indent)s
%(indent)s    static Initializer init = Initializer();
%(indent)s}
%(indent)s
%(indent)s%(closure)s
}
""" % {
                'indent': self._indent,
                'file': __file__,
                'imports': "\n".join(["if(!PyImport_ImportModule(\"pylllars%s\")){return -1;} " % n.replace("::", ".") for n in imports]),
                'module_name': element.parent.pyllars_module_name,
                'name': self.sanitize(element.name),
                'qname': qualified_name(element.name),
                'namespaces': namespace_text,
                'parent_name': qualified_name(element.parent.name),
                'parent_full_name': element.parent.full_name,
                'full_type_name': element.type_._base_type.full_name,
                'parent': self.scope(element),
                'array_size': element.type_.array_size,
                'closure': namespace_closure
                }).encode('utf-8'))
                else:
                    stream.write(("""
namespace pyllars{
    %(indent)s%(namespaces)s
constexpr cstring name = "%(name)s";
%(indent)s//generated from %(file)s.generate_initializer_code
%(indent)s// FUNCTION %(name)s THROWS
%(indent)sstatic int init_me(){
%(indent)s   status_t status = 0;
%(indent)s   %(imports)s
%(indent)s    __pyllars_internal::PythonClassWrapper<%(parent_full_name)s>::addAttribute%(qual)s<name, %(full_type_name)s>
%(indent)s       ( &%(parent_full_name)s::%(name)s);
%(indent)s   return status;
%(indent)s}
%(indent)sint %(qname)s_init(){return init_me();}
%(indent)snamespace{
%(indent)s    class Initializer: public pyllars::Initializer{
%(indent)s    public:
%(indent)s       Initializer():pyllars::Initializer(){
%(indent)s           pyllars%(parent)s::%(parent_name)s_register(this);
%(indent)s       }
%(indent)s       virtual int init(){
%(indent)s           int status = pyllars::Initializer::init();
%(indent)s           return status | init_me();
%(indent)s       }
%(indent)s    };
%(indent)s
%(indent)s    static Initializer init = Initializer();
%(indent)s}
%(indent)s
%(indent)s%(closure)s
}
""" % {
                    'indent': self._indent,
                    'file': __file__,
                    'imports': "\n".join(["if(!PyImport_ImportModule(\"pylllars%s\")){return -1;} " % n.replace("::", ".") for n in imports]),
                    'module_name': element.parent.pyllars_module_name,
                    'name': self.sanitize(element.name),
                    'qname': qualified_name(element.name),
                    'namespaces': namespace_text,
                    'parent_name': qualified_name(element.parent.name),
                    'parent_full_name': element.parent.full_name,
                    'full_type_name': element.type_.full_name,
                    'parent': self.scope(element),
                    'closure': namespace_closure,
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
                stream.write(("""
namespace pyllars{
    %(indent)s%(namespaces)s
constexpr cstring name = "%(name)s";
%(indent)s//generated from %(file)s.generate_initializer_code
%(indent)s// FUNCTION %(name)s THROWS
%(indent)sstatic int init_me(){
%(indent)s   status_t status = 0;
%(indent)s   %(imports)s
%(indent)s   static std::function< %(full_type_name)s(const %(parent_full_name)s&)> getter =
%(indent)s       [](const %(parent_full_name)s &self)->%(full_type_name)s{return self.%(name)s;};
%(indent)s   %(setter_code)s
%(indent)s    __pyllars_internal::PythonClassWrapper<%(parent_full_name)s>::addBitField%(qual)s<name, %(full_type_name)s, %(bit_size)s>
%(indent)s       ( getter %(setter)s);
%(indent)s   return status;
%(indent)s}
%(indent)sint %(qname)s_init(){return init_me();}
%(indent)snamespace{
%(indent)s    class Initializer: public pyllars::Initializer{
%(indent)s    public:
%(indent)s       Initializer():pyllars::Initializer(){
%(indent)s           pyllars%(parent)s::%(parent_name)s_register(this);
%(indent)s       }
%(indent)s       virtual int init(){
%(indent)s           int status = pyllars::Initializer::init();
%(indent)s           return status | init_me();
%(indent)s       }
%(indent)s    };
%(indent)s
%(indent)s    static Initializer init = Initializer();
%(indent)s}
%(indent)s
%(indent)s%(closure)s
}
""" % {
                    'indent': self._indent,
                    'file': __file__,
                    'bit_size': element.bit_size,
                    'imports': "\n".join(["if(!PyImport_ImportModule(\"pylllars%s\")){return -1;} " % n.replace("::", ".") for n in imports]),
                    'module_name': element.parent.pyllars_module_name,
                    'name': self.sanitize(element.name),
                    'qname': qualified_name(element.name),
                    'namespaces': namespace_text,
                    'parent_name': qualified_name(element.parent.name),
                    'parent_full_name': element.parent.full_name,
                    'full_type_name': element.type_.full_name,
                    'parent': self.scope(element),
                    'closure': namespace_closure,
                    'qual': 'Const' if element.type_.is_const else "",
                    'qual2': 'const' if not element.type_.is_const else "",
                    'setter': ", setter" if not element.type_.is_const else "",
                    'setter_code': setter_code if not element.type_.is_const else ""
                }).encode('utf-8'))


class CXXMethodDecl(FunctionDecl):
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

    def _func_declaration(self, element: parser.FunctionElement):
        if element.is_static:
            base = "addClass"
        else:
            base = "add"
        method_name = CXXMethodDecl.METHOD_NAMES.get(element.name) or ("%sMethod" % base )
        if element.has_varargs:
            method_name += "Varargs"

        return """
    __pyllars_internal::PythonClassWrapper<%(full_class_name)s>::%(py_method_name)s<%(is_const)s name, %(return_type)s %(args)s>
       ( &%(full_class_name)s::%(method_name)s, argumentNames);

""" % {
            'names': ",".join(['"%s"' % (e.name or "param_%d" % index) for index, e in enumerate(element.params)]),
            'method_name': element.name or "anonymous_%s" % element.tag,
            'full_class_name': element.parent.full_name,
            'py_method_name': method_name,
            'is_const': str(element.is_const).lower() + "," if not element.is_static else "",
            'return_type': element.return_type.full_name if element.return_type else "void",
            'args': ("," if element.params else "") + ", ".join([p.type_.full_param_name for p in element.params])
        }


class VarDecl(Generator):

    @classmethod
    def is_generatable(cls):
        return True

    def generate_body_proper(self, element: parser.VarDecl, folder: Folder, src_path, as_top: bool = False) -> None:
        if element.name == 'cinit':
            return
        with folder.open(self.body_file_name(element)) as stream:
            namespace_text, namespace_closure = self.namespaces(element)
            stream.write(("//From: %(file)s:VarDecl.generate_body_proper\n" % {
                'file': os.path.basename(__file__),
            }).encode('utf-8'))
            imports = set([])
            if element.type_ and element.type_.namespace_name != element.parent.namespace_name:
                imports.add(element.namespace_name)
            if isinstance(element.parent, parser.CXXRecordDecl):
                stream.write(("""

namespace pyllars{
%(namespaces)s
  namespace %(qname)s{
    constexpr cstring name = "%(name)s";
    static status_t init_me(){
        status_t status = 0;
        %(imports)s
        __pyllars_internal::PythonClassWrapper<%(parent_full_name)s>::addClassAttribute%(qual)s<name, %(full_type_name)s>
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
  }

  %(closure)s
}
    """ % {
                    'namespaces': namespace_text,
                    'qname': qualified_name(element.name or "anonymous_%s" % element.tag),
                    'name': element.name or "anonymous_%s" % element.tag,
                    'indent': self._indent,
                    'parent': element.parent.full_name,
                    'parent_name': qualified_name(element.parent.name if element.parent.name !='::' else ''),
                    'parent_full_name': element.parent.full_name,
                    'full_type_name': element.type_.full_name,
                    'closure': namespace_closure,
                    'qual': 'Const' if element.type_.is_const else '',
                    'imports': "\n".join(["if(!PyImport_ImportModule(\"pylllars%s\")){return -1;} " % n.replace("::", ".") for n in imports]),
            }).encode('utf-8'))
            elif isinstance(element.parent, parser.NamespaceDecl):
                stream.write(("""
#include <pyllars/pyllars_globalmembersemantics.cpp>

namespace pyllars{
%(namespaces)s
  namespace %(qname)s{
    constexpr cstring name = "%(name)s";
    static status_t init_me(){
        status_t status = 0;
        PyObject* mod = %(mod_name)s;
        %(imports)s
        if( !__pyllars_internal::GlobalVariable::createGlobalVariable<%(full_type_name)s>("%(name)s", "%(tp_name)s",
            &%(parent)s::%(name)s, mod, %(array_size)s)){
           status = -1;
         }
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
  }
  %(closure)s
}
    """ % {
                    'namespaces': namespace_text,
                    'qname': qualified_name(element.name or "anonymous_%s" % element.tag),
                    'name': element.name or "anonymous_%s" % element.tag,
                    'tp_name': element.type_.name,
                    'indent': self._indent,
                    'parent': element.parent.full_name if element.parent.full_name != '::' else "",
                    'parent_name': qualified_name(element.parent.name if element.parent.name else "pyllars"),
                    'mod_name': element.parent.pyllars_module_name,
                    'parent_full_name': element.parent.full_name,
                    'full_type_name': element.type_.full_name,
                    'closure': namespace_closure,
                    'array_size': element.type_.array_size or 0,
                    'qual': 'Const' if element.type_.is_const else 'cont',
                    'imports': "\n".join(["if(!PyImport_ImportModule(\"pylllars%s\")){return -1;} " % n.replace("::", ".") for n in imports]),
            }).encode('utf-8'))
            else:
                logging.error("Unknown parent type for global var")


class ParmVarDecl(Generator):

    @classmethod
    def is_generatable(cls):
        return False


class CXXRecordDecl(Generator):

    def __init__(self, src_path, indent=""):
        super().__init__(src_path, indent="")

    @classmethod
    def is_generatable(cls):
        return True

    def generate_body_headers(self, element: parser.CXXRecordDecl, folder: Folder):
        if element.is_implicit:
            return
        if not element.name:
            element._anonymous_types.add(self)
            return

        with folder.open(self.body_file_name(element)) as stream:
            stream.write(("""
#include <pyllars/pyllars_classwrapper.cpp>
#include <pyllars/pyllars.hpp>
#include "%(header_name)s"

""" % {'header_name': self.header_file_name(element)}).encode('utf-8'))
            for base in element.public_base_classes or []:
                header = self.header_file_path(base)
                stream.write(("#include \"%s\"\n" % header).encode('utf-8'))

    def generate_body_proper(self, element: parser.CXXRecordDecl, folder: Folder, src_path, as_top: bool = False) -> None:
        if element.is_implicit:
            return

        with folder.open(self.body_file_name(element)) as stream:
            namespace_text, namespace_closure = self.namespaces(element)
            stream.write(("//From: %(file)s:generate_body_proper\n" % {
                'file': os.path.basename(__file__),
            }).encode('utf-8'))

            full_class_name = element.full_name
            class_name = "main_type"
            stream.write(("""

namespace pyllars{
%(indent)s%(namespaces)s
  namespace %(qname)s{
    static status_t init_me(){
        using namespace __pyllars_internal;
        typedef %(full_class_name)s main_type;
        static status_t _status = -1;
        static bool inited = false;
        if (inited){
            return _status;
        }
        inited = true;
        status_t status = 0;
    """ % {
                'namespaces': namespace_text,
                'qname': qualified_name(element.name or "anonymous_%s" % element.tag),
                'name': element.name or "anonymous_%s" % element.tag,
                'indent': self._indent,
                'full_class_name': full_class_name
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
             __pyllars_internal::PythonClassWrapper< %(parent_class_name)s >::addClassMember
                ("%(name)s",
                 (PyObject*) & __pyllars_internal::PythonClassWrapper< %(class_name)s >::Type);
""" % {
                        'parent_class_name': element.parent.full_name,
                        'name': element.name,
                        'class_name': element.full_name
                    }).encode('utf-8'))
            stream.write(("""
        _status = status;
        return status;
    }


    %(indent)sint %(qname)s_register( pyllars::Initializer* const init ){
    %(indent)s    static pyllars::Initializer _initializer = pyllars::Initializer();
    %(indent)s    static int status = pyllars%(parent)s::%(parent_name)s_register(&_initializer);
    %(indent)s    return status==0?_initializer.register_init(init):status;
    %(indent)s }

    %(indent)snamespace{
    %(indent)s    class Initializer: public pyllars::Initializer{
    %(indent)s    public:
    %(indent)s       Initializer():pyllars::Initializer(){
    %(indent)s           pyllars%(parent)s::%(parent_name)s_register(this);
    %(indent)s       }
    %(indent)s       virtual int init(){
    %(indent)s           int status = pyllars::Initializer::init();
    %(indent)s           return status | init_me();
    %(indent)s       }
    %(indent)s    };
    %(indent)s
    %(indent)s    static Initializer init = Initializer();
    %(indent)s}
    %(indent)s
%(indent)s%(closure)s
%(indent)s}
}
    """ % {'indent': self._indent,
           'name': self.sanitize(element.name if element.name else "pyllars"),
           'qname': qualified_name(element.name if element.name else "pyllars"),
            'parent_name': qualified_name(element.parent.name if (element.parent.name and element.parent.name != "::")
                                          else "pyllars"),
            'parent': element.scope if element.scope != '::' else "",
            'closure': namespace_closure
          }).encode('utf-8'))
