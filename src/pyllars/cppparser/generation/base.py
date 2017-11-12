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
    def __init__(self, path: str, parent: "Folder" = None):
        assert os.path.isdir(path)
        self._path = path
        self._parent = parent

    @property
    def path(self):
        return self._path

    def purge(self, file_name: str) -> None:
        full_path = os.path.join(self._path, file_name)
        if os.path.exists(full_path):
            os.remove(full_path)

    @contextmanager
    def open(self, file_name: str) -> TextIOBase:
        from . import FileWriter
        open_file = None
        try:
            full_path = os.path.join(self._path, file_name)
            if not os.path.exists(os.path.dirname(full_path)):
                os.makedirs(os.path.dirname(full_path))
            open_file = FileWriter.open(full_path, 'ab')
            yield open_file
        finally:
            if open_file:
                open_file.close()

    def create_subfolder(self, name: str)->"Folder":
        folder_name = os.path.join(self._path, name)
        assert not os.path.isfile(folder_name)
        if not os.path.exists(folder_name):
            os.makedirs(folder_name)
        return Folder(folder_name, parent=self)

    @property
    def base_name(self) -> str:
        return os.path.basename(self._path) if self._parent else "."

    @property
    def path(self) -> str:
        if self._parent:
            return os.path.join(self._parent.path, self.base_name)
        return "."


class Generator(metaclass=ABCMeta):

    def __init__(self, element, src_path: str, folder: Folder, parent_generator: "Generator"=None):
        self._src_path = src_path
        self._template_argument_lists = []
        self._template_type_params = []
        self._folder = folder
        self._subfolder = None
        self._element = element
        self._parent = parent_generator

    @classmethod
    def is_generatable(cls):
        return False

    def header_file_path(self) -> str:
        header_name = self.header_file_name()
        return os.path.join(self.folder.path, header_name)
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

    def header_file_name(self)->str:
        return (self.element.basic_name or "global") + ".hpp"

    def body_file_path(self) ->str:
        parent_path = self.folder.path
        if not os.path.exists(parent_path):
            os.makedirs(parent_path)
        return os.path.join(parent_path, self.to_path(ext=".cpp"))

    def body_file_name(self)->str:
        return (self.element.basic_name or "global") + ".cpp"

    @property
    def element(self):
        return self._element

    @property
    def folder(self):
        return self._folder

    @property
    def subfolder(self):
        if self._subfolder is None:
            self._subfolder = self.folder.create_subfolder(self.element.name, self)
        return self._subfolder

    @property
    def parent(self):
        return self._parent

    def to_path(self, ext="") -> str:
        name = self.element.basic_name if self.element.name else "global"
        return name.replace(" ", "_") + ext

    def scope(self):
        return self.element.scope

    def basic_includes(self):
        return ("""
#include <Python.h>
#include <pyllars/pyllars.hpp>
#include <pyllars/pyllars_classwrapper.cpp>
#include <pyllars/pyllars_function_wrapper.hpp>

#include <vector>

#include <%(src_path)s>
%(parent_header_name)s
"""  % {'src_path': self._src_path,
        'parent_header_name': "#include \"%s\"" % self.parent.header_file_path() if self.element.parent else "",
        }).encode('utf-8')

    @contextmanager
    def scoped(self, stream: TextIOBase):
        with self._ns_scope(stream) as stream:
            yield stream

    @contextmanager
    def _ns_scope(self, stream: TextIOBase):
        if self.parent:
            with self.parent._ns_scope(stream) as scoped:
                scoped.write(("\nnamespace %s{\n" % qualified_name(self.element.basic_name)).encode("utf-8"))
                yield stream
        else:
            stream.write(b"\nnamespace pyllars{\n")
            yield stream
        stream.write(b"\n}")

    @contextmanager
    def guarded(self, stream : TextIOBase):
        stream.write(("""
            #ifndef __%(guard)s__
            #define __%(guard)s__
    
            """ % {'guard': self.element.guard}).encode('utf-8'))
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

    def generate_header_code(self, stream: TextIOBase) -> None:
        with self.guarded(stream) as guarded:
            guarded.write(self.basic_includes())
            with self.scoped(guarded) as scoped:
                self.generate_header_core(scoped)

    def generate_header_core(self, stream: TextIOBase, as_top=False):
        if not self.element.name:  # anonymous directly inaccessible type
            stream.write(b"")
            return
        stream.write(("""
                %(template_decl)s
                status_t %(qname)s_register( pyllars::Initializer* const);
                
                %(template_decl)s
                status_t %(qname)s_init();
            """ % {
                'qname': qualified_name(self.element.basic_name),
                'template_decl': self.template_decl,
        }).encode('utf-8'))

    def generate_spec(self):
        file_name = self.to_path(ext=".hpp")
        self.folder.purge(file_name)
        with self.folder.open(file_name) as stream:
            self.generate_header_code(stream)

    def generate_body(self, as_top=False):
        if not self.element.name and not as_top:
            return
        file_name = self.to_path(ext=".cpp")
        self.folder.purge(file_name)
        self.generate_spec()
        with self.folder.open(file_name) as stream:
            stream.write(("""
                #include "%(my_header_name)s"
                #include <pyllars/pyllars_globalmembersemantics.cpp>
                """ % {"my_header_name": self.header_file_name()}).encode('utf-8'))
            if self.element.is_implicit:
                return
            if not self.element.name:
                self.element._anonymous_types.add(self)
            else:
                with self.scoped(stream) as scoped:
                    self.generate_body_proper(scoped, as_top)
        #if not isinstance(self.element, parser.CXXMethodDecl) and not isinstance(self.element, parser.FunctionDecl):
        for child in self.element.children():
            child_generator_class = self.get_generator_class(child)
            if child_generator_class.is_generatable():
                child_generator = child_generator_class(child, self._src_path, self.folder.create_subfolder(child.basic_name),
                                                        parent_generator=self)
                child_generator._template_argument_lists += self._template_argument_lists
                child_generator._template_type_params += self._template_type_params
                if self.element.template_arguments:
                    child_generator._template_argument_lists.append(self.element.template_arguments)
                    child_generator._template_type_params.append(self.element._template_type_params)
                child_generator.generate_body()

    @staticmethod
    def get_generator_class(element: parser.Element) -> "Generator":
        from . import _get_generator_class
        generator_class = _get_generator_class(element)
        return generator_class

    def generate_body_proper(self, stream: TextIOBase, as_top: bool = False):
        pass


class ParmVarDecl(Generator):

    @classmethod
    def is_generatable(cls):
        return False


class ClassTemplateDecl(Generator):

    @classmethod
    def is_generatable(cls):
        return True

    def generate_spec(self):
        generator_class = self.get_generator_class(self.element)
        if not generator_class.is_generatable():
            return
        generator = generator_class(self.element, self._src_path, self.folder, parent_generator=self)
        file_name = self.to_path(ext=".hpp")
        self.folder.purge(file_name)
        with self.folder.open(file_name=file_name) as stream:
            if not self.element.name:
                stream.write(b"")
                return
            with self.guarded(stream) as guarded:
                with self.scoped(guarded) as scoped:
                    scoped.write(("""
#ifndef __%(guard)s__
#define __%(guard)s__

""" % {'guard': self.element.guard}).encode('utf-8'))
                    if self.element.parent:
                        scoped.write(("""
        #include "%(target_file_name)s"
        #include "%(parent_header_name)s"
        """ % {
                            'parent_header_name': self.header_file_path(element.parent),
                            'target_file_name': self._src_path}).encode("utf-8"))

                    scoped.write(("""
        #include <pyllars/pyllars_classwrapper.hpp>
        
        namespace pyllars{
    
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
        
}
#endif
            """ % {
                'qname': qualified_name(self.element.basic_name),
                'parent': self.element.parent.full_name if self.element.parent.full_name != "::" else "",
                'parent_name': self.element.parent.basic_name,
                'template_arg_vals': ",".join([arg.py_var_name(index) for index, arg in enumerate(self.element.template_args)]),
                'full_name': self.element.full_name,
                'template_arg_len': len(self.element.template_args),
                'template_decl': self.element.template_decl(),
                'template_args': self.element.template_arguments_string(),}).encode('utf-8'))

    def generate_body_proper(self, stream: TextIOBase, as_top: bool = False):
        with self.scoped(stream) as scoped:
            scoped.write(("            //From: %(file)s:generate_body_proper\n" % {
                'file': __file__
            }).encode('utf-8'))

            full_class_name = self.element.full_name
            if isinstance(self.element.parent, parser.NamespaceDecl):
                add_dict_code = """
                    PyModule_AddObject(
                        %(module_name)s, "%(name)s", mapping);    
                """ % {
                    'module_name': self.element.parent.pyllars_scope,
                    'name': self.element.name
                }
            elif isinstance(self.element.parent, parser.RecordTypeDefn):
                add_dict_code = """
                     __pyllars_internal::PythonClassWrapper< %(parent_class_name)s >::addClassMember
                        ("%(name)s", mapping);
                """ % {
                        'parent_class_name': self.element.parent.full_name,
                        'name': self.element.name,
                    }
            else:
                raise Exception("Unknown parent type ro template class: %s" % self.element.parent.__class__)
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
                'qname': qualified_name(self.element.basic_name or "anonymous_%s" % self.element.tag),
                'full_class_name': full_class_name,
                'add_dict_code': add_dict_code,
                'full_name_space': self.element.parent.full_name if self.element.parent.full_name != "::" else "",
                'parent_name': qualified_name(
                    self.element.parent.name if (self.element.parent.name and self.element.parent.name != "::")
                    else ""),
                'parent': self.element.parent.name if self.element.parent else "pyllars",
                'template_decl': self.element.template_decl(),
                'parent_template_decl': self.element.parent.template_decl() if self.element.parent else ""
            }).encode('utf-8')) 