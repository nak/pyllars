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
            cmd = "%(cxx)s -O -std=c++14 %(cxxflags)s -c -fPIC -I%(local_include)s -I%(python_include)s " \
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
        cmd = "%(cxx)s -O -fPIC -std=c++14 %(cxxflags)s -I%(python_include)s -shared -o _trial.so -Wl,--no-undefined " \
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
        self._template_argument_lists = []
        self._template_type_params = []

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
            path = os.path.join(*(paths + [header_name])) if paths else header_name
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
        return (element.basic_name or "global") + ".hpp"

    @classmethod
    def body_file_path(cls, element: parser.Element) ->str:
        parent_path = os.path.dirname(cls.header_file_path(element.parent)) if element.parent is not None else "."
        if not os.path.exists(parent_path):
            os.makedirs(parent_path)
        return os.path.join(parent_path, cls.to_path(element.basic_name or "global", ext=".cpp"))

    @classmethod
    def body_file_name(cls, element: parser.Element)->str:
        return (element.basic_name or "global") + ".cpp"

    def scope(self, element):
        return element.scope

    def basic_includes(self, element: parser.Element):
        return ("""
#include <Python.h>
#include <pyllars/pyllars.hpp>
#include <pyllars/pyllars_classwrapper.cpp>
#include <pyllars/pyllars_function_wrapper.hpp>

#include <vector>

#include <%(src_path)s>
%(parent_header_name)s
"""  % {'src_path': self._src_path,
        'parent_header_name': "#include \"%s\"" % self.header_file_path(element.parent) if element.parent else "",
        }).encode('utf-8')

    @contextmanager
    def scoped(self, element: parser.Element, stream: TextIOBase):
        namespace_text, namespace_closure = self.namespaces(element)
        stream.write(b"\nnamespace pyllars{\n")
        stream.write(namespace_text.encode('utf-8'))
        stream.write(("\n\n//============\n\nnamespace %s{\n" % qualified_name(element.name)).encode('utf-8'))
        yield stream
        stream.write(b"\n}\n\n//==============\n\n    ")
        stream.write( namespace_closure.encode('utf-8'))
        stream.write(b"\n}")

    @contextmanager
    def guarded(self, element, stream):
        stream.write(("""
            #ifndef __%(guard)s__
            #define __%(guard)s__
    
            """ % {'guard': element.guard}).encode('utf-8'))
        yield stream
        stream.write(b"""
            #endif
        """)

    @property
    def template_decl(self):
        template_decl = ""
        for template_args in self._template_argument_lists:
            template_decl += "\n%s" % parser.Element.template_declaration(template_args)
        return template_decl

    @property
    def template_arguments(self):
        return "" if not self._template_type_params else "<%s>" % (", ".join([e.name for e in self._template_arguments]))

    def generate_header_code(self, element: parser. Element, stream: TextIOBase) -> None:
        with self.guarded(element, stream) as guarded:
            guarded.write(self.basic_includes(element))
            with self.scoped(element, guarded) as scoped:
                self.generate_header_core(element, scoped)

    def generate_header_core(self, element: Element, stream: TextIOBase, as_top=False):
        if not element.name:
            stream.write(b"")
            return
        stream.write(("""
                %(template_decl)s
                int %(qname)s_register( pyllars::Initializer* const);
                
                %(template_decl)s
                int %(qname)s_init();
            """ % {
                'qname': qualified_name(element.basic_name),
                'template_decl': self.template_decl,
        }).encode('utf-8'))

    def generate_spec(self, element: Element, folder: Folder):
        from ..parser import NamespaceDecl
        if isinstance(element, NamespaceDecl):
            folder = folder.create_subfolder(self.to_path(element.basic_name))
        # generator = self.get_generator(type(element), self._src_path, "")
        # if not generator:
        #     return
        file_name = self.to_path(element.basic_name or "global", ext=".hpp")
        folder.purge(file_name)
        with folder.open(file_name=file_name) as stream:
            self.generate_header_code(element=element, stream=stream)

    def generate_body(self, element: parser.Element, folder: Folder, as_top=False):
        if not element.name and not as_top:
            return
        file_name = self.to_path(element.basic_name or "global", ext=".cpp")
        folder.purge(file_name)
        self.generate_spec(element, folder)
        with folder.open(file_name=file_name) as stream:
            stream.write(("""
        #include "%(my_header_name)s"
        #include <pyllars/pyllars_globalmembersemantics.cpp>
            """ % {"my_header_name": self.header_file_name(element)}).encode('utf-8'))
            if element.is_implicit:
                return
            if not element.name:
                element._anonymous_types.add(self)
            else:
                self.generate_body_proper(element, stream, as_top)
        subfolder = folder.create_subfolder(element.basic_name) if element.name != "::" and element.name else folder
        if not isinstance(element, parser.CXXMethodDecl) and not isinstance(element, parser.FunctionDecl):
            for child in element.children():
                #if child.name == element.name:
                #    continue
                child_generator = self.get_generator(type(child), self._src_path, "")
                if child_generator:
                    child_generator._template_argument_lists += self._template_argument_lists
                    child_generator._template_type_params += self._template_type_params
                    if element.template_arguments:
                        child_generator._template_argument_lists.append(element.template_arguments)
                        child_generator._template_type_params.append(element._template_type_params)
                    child_generator.generate_body(child, subfolder)

    @staticmethod
    def get_generator(clazz: type, src_path: str, indent: str) -> "Generator":
        from . import _get_generator
        return _get_generator(clazz=clazz, src_path=src_path, indent=indent)

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

    def generate_body_proper(self, element: Element, folder: Folder, src_path, as_top: bool = False):
        pass



class ParmVarDecl(Generator):

    @classmethod
    def is_generatable(cls):
        return False


class ClassTemplateDecl(Generator):

    @classmethod
    def is_generatable(cls):
        return True

    def generate_spec(self, element: parser.ClassTemplateDecl, folder: Folder):
        from ..parser import NamespaceDecl
        if isinstance(element, NamespaceDecl):
            folder = folder.create_subfolder(self.to_path(element.basic_name))
        generator = self.get_generator(type(element), self._src_path, "")
        if not generator:
            return
        file_name = self.to_path(element.basic_name or "global", ext=".hpp")
        folder.purge(file_name)
        with folder.open(file_name=file_name) as stream:
            if not element.name:
                stream.write(b"")
                return
            namespace_text, namespace_closure = generator.namespaces(element)
            stream.write(("""
#ifndef __%(guard)s__
#define __%(guard)s__

""" % {'guard': element.guard}).encode('utf-8'))
            if element.parent:
                stream.write(("""
#include "%(target_file_name)s"
#include "%(parent_header_name)s"
""" % {
                    'parent_header_name': self.header_file_path(element.parent),
                    'target_file_name': self._src_path}).encode("utf-8"))

            stream.write(("""
#include <pyllars/pyllars_classwrapper.hpp>

namespace pyllars{
    %(namespaces)s
    
///////////////////////////////

        namespace %(qname)s{
            %(template_decl)s
            status_t %(qname)s_init();
            
            %(template_decl)s
            class Initializer: public pyllars::Initializer{
            public:
               
               Initializer():pyllars::Initializer(){
                   pyllars%(parent)s::%(parent_name)s_register(this);
               }
               
               virtual int init(){
                   int status = pyllars::Initializer::init();
                   return status | %(qname)s_init%(template_args)s();
               }
               
            };
        
            %(template_decl)s
            static Initializer%(template_args)s init = Initializer%(template_args)s();  
        }

//////////////////////////////
        
    %(closure)s
}
#endif
            """ % {
                'qname': qualified_name(element.basic_name),
                'namespaces': namespace_text,
                'parent': element.parent.full_name if element.parent.full_name != "::" else "",
                'parent_name': element.parent.basic_name,
                'template_arg_vals': ",".join([arg.py_var_name(index) for index, arg in enumerate(element.template_args)]),
                'full_name': element.full_name,
                'template_arg_len': len(element.template_args),
                'template_decl': element.template_decl(),
                'template_args': element.template_arguments_string(),
                'closure': namespace_closure}).encode('utf-8'))

    def generate_body_proper(self, element: parser.ClassTemplateDecl, stream: TextIOBase, src_path, as_top: bool = False):
        with self.scoped(element, stream) as scoped:
            scoped.write(("            //From: %(file)s:generate_body_proper\n" % {
                'file': __file__
            }).encode('utf-8'))

            full_class_name = element.full_name
            if isinstance(element.parent, parser.NamespaceDecl):
                add_dict_code = """
                    PyModule_AddObject(
                        %(module_name)s, "%(name)s", mapping);    
                """ % {
                    'module_name' : element.parent.pyllars_module_name,
                    'name': element.name
                }
            elif isinstance(element.parent, parser.RecordTypeDefn):
                add_dict_code = """
                     __pyllars_internal::PythonClassWrapper< %(parent_class_name)s >::addClassMember
                        ("%(name)s", mapping);
                """ % {
                        'parent_class_name': element.parent.full_name,
                        'name': element.name,
                    }
            else:
                raise Exception("Unknown parent type ro template class: %s" % element.parent.__class__)
            scoped.write(("""
                
                template<typename T>
                PyObject* keyFrom( const T& instance){
                    return PyBytes_FromStringAndSize( (const char*)&instance, sizeof(instance) );
                }
                
                static PyObject* mapping = PyDict_New();

                
                %(template_decl)s
                status_t %(qname)s_init(){
                   if (!mapping){
                       return -1;
                   }
                   %(add_dict_code)s
                   return 0;
                }
               
                %(template_decl)s
                int %(qname)s_register( pyllars::Initializer* const init){
                    static pyllars::Initializer _initializer = pyllars::Initializer();
                    static int status = pyllars::%(parent_name)s%(parent_template_decl)s::%(parent)s_register(&_initializer);
                    return status==0?_initializer.register_init(init):status;
                }
                
""" % {
                'qname': qualified_name(element.basic_name or "anonymous_%s" % element.tag),
                'full_class_name': full_class_name,
                'add_dict_code': add_dict_code,
                'full_name_space': element.parent.full_name if element.parent.full_name != "::" else "",
                'parent_name': qualified_name(
                    element.parent.name if (element.parent.name and element.parent.name != "::")
                    else ""),
                'parent': element.parent.name if element.parent else "pyllars",
                'template_decl': element.template_decl(),
                'parent_template_decl': element.parent.template_decl() if element.parent else ""
            }).encode('utf-8')) 