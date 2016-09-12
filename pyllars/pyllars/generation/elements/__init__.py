import os
from abc import abstractmethod

system_mapping = {'bits/waitlist.h': '/usr/include/wait.h',
                  'bits/waitstatus.h': '/usr/include/wait.h'}

def system_patch(path):
    if path and path.startswith('/usr/include'):
        for mapped_path, patched in system_mapping.items():
            if path.endswith(mapped_path):
                return patched
    return path

def sanitize(name):
    if name == "::":
        return "pyllars"
    return  name.replace("<", "__").replace(">", "__").replace(",", "___").replace(" ", "_"). \
        replace('::::','::').replace("::", "____").replace('&','_and_').replace("*","_star_").replace('c++', 'cxx').\
        replace('+', 'X').replace('-', '_').replace('&','_and_').replace('*','_start_')

class BaseElement(object):

    PSEUDO_ID = "PSEUDO"
    KEYWORDS = ['errno', 'wait', 'obstack']

    def __init__(self, name, id_, header_filename, context):
        assert (context is None or isinstance(context, Namespace) or isinstance(context, Type))
        assert (id_ is not None)
        assert (name is not None)
        nameparts = name.split('<')
        nameparts[0] = nameparts[0].replace('::',"")
        self.name = "<".join(nameparts)
        self.id_ = id_
        self.context = context
        self.header_filename = system_patch(header_filename) or header_filename
        self.sanitized_name = sanitize(name)
        self.children = []
        fallback_name = os.path.basename(self.header_filename).split('.')[0] if self.header_filename else None
        if context:
            parent_mod = self
            while not isinstance(parent_mod, Namespace):
                parent_mod = parent_mod.context
            parent_mod.is_global_namespace = (parent_mod.name == "::" or parent_mod.name =="")
            self.module_name = parent_mod.name if (not parent_mod.is_global_namespace or not fallback_name) else fallback_name
        else:
            parent_mod = Namespace.global_ns or self if name == "" or name == "pyllars" else Namespace("", "_1", None, None)
            parent_mod.is_global_namespace = True
            self.module_name = "pyllars" if isinstance(self, Namespace) else ( fallback_name or "pyllars")
        if self.module_name in BaseElement.KEYWORDS:
            self.module_name += "_"
        self.pseudo_context = context if id_ == "_1" or id_==BaseElement.PSEUDO_ID or (parent_mod and not parent_mod.is_global_namespace) else \
            (Namespace.namespaces.get(self.module_name) or Namespace( name=self.module_name,
                                                                      id_=BaseElement.PSEUDO_ID,
                                                                      header_filename=header_filename,
                                                                      context=Namespace.global_ns or Namespace("","_1",None,None)))

    def get_full_module_name(self):
        context = self.context
        if context is None or context.id_ == BaseElement.PSEUDO_ID:
            return self.module_name
        while not isinstance(context, Namespace):
            context = context.context
        if context.is_global_namespace:
            return self.module_name
        elif context.context and context.context.name:
            return ".".join([context.context.get_full_module_name(), self.module_name])
        else:
            return self.module_name

    def get_top_module_name(self):
        if self.context and self.id_ == BaseElement.PSEUDO_ID:
            return self.name
        top = self
        while top.context and top.context.name not in ["::","","pyllars"]:
            top = top.context
        return top.module_name

    def get_name(self):
        return self.name

    def empty(self):
        return len(self.children) == 0

# noinspection PyUnusedLocal
class Namespace(BaseElement):
    """
    Class to capture namespace properties
    """

    namespaces = {}
    global_ns = None

    def __init__(self, name, id_, header_filename, context):
        BaseElement.__init__(self, name, id_, header_filename, context)
        assert (context is None or isinstance(context, Namespace))
        self.is_global_namespace = (self.name == "::" or self.name =="")
        if self.is_global_namespace:
            Namespace.global_ns = self
        if context and context.full_name != "::":
            self.full_name = context.full_name + "::" + name
        else:
            self.full_name = name
        if self.full_name not in Namespace.namespaces:
            Namespace.namespaces[self.full_name] = self
            Namespace.namespaces[id_] = Namespace.namespaces[self.full_name]
        self._classes = {}

    @staticmethod
    def lookup(id_):
        return Namespace.namespaces.get(id_)

    def get_path(self, path_prefix=None):
        if self.context:
            return os.path.join(path_prefix or ".", self.context.get_path(), self.name)
        else:
            return os.path.join(path_prefix or ".", self.name)

    def mod_object_name(self):
        if self.context is None or self.context.name == "::":
            return "pyllars__%s" % self.name
        else:
            return "__".join([self.context.mod_object_name(), self.name])

    def get_include_parent_path(self):
        if self.context is None or self.name == "::" or self.name == "":
            path = "pyllars"
        else:
            path = sanitize(os.path.join(self.context.get_include_parent_path(), self.name or "pyllars").replace(" ", "_"))#. \
                ###replace("<", "__").replace(">", "__").replace("::::", "::").replace("::", "____").replace(", ", "__").replace('&','_and_').replace("*","_star_")

        return path

    def get_header_filename(self, path=None):
        if self.id_ == BaseElement.PSEUDO_ID and self.name != "pyllars":
            return os.path.join(self.get_include_parent_path(), "%s_module.hpp" % self.name) if path is None else \
                os.path.join(path, self.get_include_parent_path(), "%s_module.hpp" % self.name)
        return os.path.join(self.get_include_parent_path(), "module.hpp") if path is None else \
            os.path.join(path, self.get_include_parent_path(), "module.hpp")

    def get_body_filename(self, path=None):
        if self.id_ == BaseElement.PSEUDO_ID and self.name != "pyllars":
            return os.path.join(self.get_include_parent_path(), "%s_module.cpp" % self.name) if path is None else \
                os.path.join(path, self.get_include_parent_path(), "%s_module.cpp" % self.name)
        return os.path.join(self.get_include_parent_path(), "module.cpp") if path is None else \
            os.path.join(path, self.get_include_parent_path(), "module.cpp")

    def generate_code(self, path, suffix="", class_name=None):
        include_path = os.path.join(path, self.get_include_parent_path())
        if not os.path.exists(include_path):
            os.makedirs(include_path)

        code = ""
        header_code = ""
        p = self
        depth = 0
        indent = "   "
        fullindent = indent
        context_name = self.context.name + "::" + self.context.name + "_mod" if self.context and self.context.name and not self.is_global_namespace else \
            "pyllars::pyllars_mod"
        precode = ""
        precode += """
%(indent)sstatic status_t init_functions(PyObject *%(name)s_mod){
%(indent)s   int status = 0;
""" % {'name': self.sanitized_name, 'indent': indent, 'fullname': self.full_name}
        headers = []
        for func in [f for f in self.children if isinstance(f, Function)]:
            if func.header_filename is not None:
                if func.header_filename not in headers:
                    headers.append(func.header_filename)
            precode += """\n//ID IS %s""" % func.id_
            precode += """%(indent)s
%(indent)s      {
%(indent)s         static const char* const argumentnames[] = {%(argument_names)s};
%(indent)s         status |= PyModule_AddObject( %(name)s_mod, "%(func_name)s",
%(indent)s              (PyObject*)__pyllars_internal::PythonFunctionWrapper<__pyllars_internal::
%(indent)s                 is_complete< %(return_type)s >::value, %(return_type)s %(arguments)s>::
%(indent)s                 create("%(func_name)s", %(context)s::%(func_name)s, argumentnames));
%(indent)s      }""" % {'indent': indent,
                        'name': self.sanitized_name,
                        'func_name': func.name,
                        'context': self.get_qualified_name(),
                        'return_type': func.return_type.get_qualified_name(),
                        'arguments': (',' if len(func.arguments) > 0 else "") + ','.join([t.get_qualified_name()
                                                                                         for _, t in func.arguments]),
                        'argument_names': ','.join(["\"%s\"" % (n if n != "" else "_%s" % index) for index, (n, _) in
                                                    enumerate(func.arguments)])}

        precode += """
%(indent)s   return status;
%(indent)s}

""" % {'indent': indent}
        while p is not None and p.name != "" and p.name != "::":
            code = """%(indent)snamespace %(name)s{
%(indent)s""" % {'name': p.sanitized_name, 'indent': indent} + code
            p = p.context
            depth += 1
            fullindent += indent

        if self.name != "" and self.name != "::" and self.name != "pyllars":
            code = "namespace pyllars{\n" + code
        indent = fullindent[3:]
        header_code = header_code + code + """
%(indent)sextern PyObject* %(name)s_mod;

%(indent)sstatus_t init();
""" % {'name': self.sanitized_name, 'indent': indent}
        code += """
%(indent)sPyObject* %(name)s_mod;
""" % {'name': self.sanitized_name, 'indent': indent}
        for header in headers + ["iostream"]:
            precode = "#include <%s>\n" % header + precode
        precode += "\n// MUST INCLUDE IOSTREAM DUE TO NON_FWD DECLARATION IN GNU C++ FOR std::getline THAT NEEDS COMPLETE TYPE DEFINITION AS AND WHERE DEFINED\n\n"
        code = precode +  code
        code += """
%(indent)sstatus_t init(){
%(indent)s   if (%(name)s_mod) return 0;// if already initialized
%(indent)s   %(parent_init)s
%(indent)s   int status = 0;
%(indent)s   %(name)s_mod = Py_InitModule3("%(name)s", nullptr,"Module corresponding to C++ namespace %(fullname)s");
%(indent)s   if( %(name)s_mod ){
%(indent)s      status |= init_functions(%(name)s_mod);
""" % {'name': self.sanitized_name, 'indent': indent, 'fullname': self.full_name or "pyllars",
       'parent_init': ("pyllars%s::init();" % self.context.get_qualified_name())
       if self.context and self.context.name and self.context.full_name != "::" else ""}#######
        if self.context or (self.context is not None and self.context.context is not None):
            code += """
%(indent)s      status |= PyModule_AddObject( %(contextname)s, "%(name)s", %(name)s_mod );
""" % {'name': self.sanitized_name, 'indent': indent, 'contextname': context_name, 'fullname': self.full_name or "pyllars",
       'parent_init': "pyllars%s::init();" % self.context.get_qualified_name()
       if self.context and self.context.full_name != "::" else ""}
        code += """
%(indent)s      Initializer::init();
%(indent)s   } else {
%(indent)s      status = -1;
%(indent)s   }
%(indent)s   return status;
%(indent)s}

extern "C"{
   status_t init%(name)s(){
      return init();
   }
}

%(indent)s//add init to methods to be called on intiailization
%(indent)sstatic pyllars::Initializer __initializer( init );

""" % {'name': self.sanitized_name, 'indent': indent, 'contextname': context_name, 'fullname': self.full_name or "pyllars",
       'parent_init': ("pyllars%s::init();" % self.context.get_qualified_name())
       if self.context and self.get_qualified_name() and self.context.full_name != "::" else ""}

        #  closing brackets:
        for i in range(depth):
            indent = indent[:-3]
            code += indent + '}\n'
            header_code += indent + '}\n'
        if self.name != "" and self.name != "::" and self.name != "pyllars":
            code += "}//ENDNS\n"
        if self.name != "" and self.name != "::" and self.name != "pyllars":
            header_code += "\n}//ENDns '%s'" % self.sanitized_name
        code += "// END %s" % self.full_name
        guard = sanitize(include_path[1:].replace('/', '__').replace('.',''))
        mod_header_path = os.path.dirname(self.get_include_parent_path())
        if mod_header_path == '/' or mod_header_path == "":
            mod_header_path = "pyllars.hpp"
        elif mod_header_path != "":
            mod_header_path += "/module.hpp"
        assert (not mod_header_path.startswith('/'))
        header_code = """#ifndef %(guard)s
#define %(guard)s
#include <%(path)s>

""" % {'path': mod_header_path, 'guard': guard} + header_code + """

#endif
"""
        if suffix == "":
            top_context = self.context
            while top_context and top_context.context and top_context.context.name and top_context.context.name != "":
                top_context = top_context.context
            if top_context and top_context.name == "std":
                code += """using namespace std; \n"""
            code = """
#include <Python.h>
#include <%(path)s/%(prefix)smodule.hpp>
#include <pyllars_classwrapper.cpp>
#include <pyllars_function_wrapper.hpp>
%(header_include)s
""" % {'header_include': ("#include \"%s\"" % self.header_filename) if self.header_filename else "",
       'path': self.get_include_parent_path(),
       'prefix': self.name+"_" if self.id_ == BaseElement.PSEUDO_ID  and self.name != "pyllars" else ""} + code
        return header_code, code

    def get_qualified_name(self, iterative=False):
        name = ""
        if self.id_ == BaseElement.PSEUDO_ID:
            name = "::" + self.name
        elif self.context is not None:
            name = "::".join([self.context.get_qualified_ns_name(True), self.sanitized_name])
        elif not iterative:
            name = self.get_name().replace('  ',' ').replace('const const', 'const')
        else:
            return ""
        return name

    def get_qualified_ns_name(self, sanitized=False):
        return self.get_qualified_name(sanitized)


# noinspection PyMethodMayBeStatic
class Type(BaseElement):

    def __init__(self, name, id_, context, is_incomplete, header_filename, scope):
        BaseElement.__init__(self, name=name, id_=id_, context=context, header_filename=header_filename)
        assert (isinstance(context, Type) or isinstance(context, Namespace) or context is None)
        self.is_incomplete = is_incomplete
        if context:
            self.full_name = context.full_name + "::" + name
        else:
            self.full_name = name
        self.qualifiers = []
        self.scope = scope
        #assert(context is not None or isinstance(self, TypeAlias) or isinstance(self, FundamentalType))

    def get_qualified_ns_name(self, sanitized=False):
        if not self.context:
            return "::"
        return self.context.get_qualified_ns_name(sanitized) + (
        "::" + self.sanitized_name + "___ns" if self.sanitized_name != "" else "")

    def get_qualified_name(self, sanitized=False):
        name = self.name if not sanitized else self.sanitized_name
        if self.context is not None:
            assert(self.context != self), "Context is same as self for %s" % self.id_
            return "::".join([self.context.get_qualified_name(sanitized), name]) + " " + " ".join(self.qualifiers)
        elif isinstance(self, FundamentalType):
            return name + " " + " ".join(self.qualifiers)
        else:
            return "::" + name + " " + "  ".join(self.qualifiers)

    def is_function_type(self):
        return False

    def set_qualifiers(self, qualifiers):
        self.qualifiers = qualifiers or []

    def get_array_size(self):
        return None

    def as_parameter(self):
        return self

    def as_attribute(self):
        return self

    def is_const(self):
        return False

    def is_fundamental(self):
        return False

    @abstractmethod
    def generate_code(self, path):
        pass

    def get_include_parent_path(self):
        if self.context is None:
            return None
        path = os.path.join(self.context.get_include_parent_path(), self.sanitized_name) if self.context is not None else None
        return path

    def get_header_filename(self, path=None):
        if self.scope in ['private', 'protected']:
            return None
        if self.sanitized_name == '' or self.header_filename is None or self.get_include_parent_path() is None: return None
        return os.path.join(self.get_include_parent_path(), self.sanitized_name + ".hpp") if path is None else \
            os.path.join(path, self.get_include_parent_path(), self.sanitized_name + ".hpp")

    def get_body_filename(self, path=None):
        if self.scope in ['private', 'protected']:
            return None
        if self.sanitized_name == '' or self.header_filename is None or self.get_include_parent_path() is None: return None
        return os.path.join(self.get_include_parent_path(), self.sanitized_name + ".cpp") if path is None else \
            os.path.join(path, self.get_include_parent_path(), self.sanitized_name + ".cpp")

    def get_core_type(self):
        return self

    def generate_preamble(self, path, code):
        if self.context is None:
            nsid = "_1"
        else:
            nsid = self.context.id_
        my_header_path = self.get_header_filename(path)

        top_context = self.context
        while top_context.context and top_context.context.name and top_context.context.name != "":
            top_context = top_context.context
        if top_context.name == "std":
            code += """using namespace std;\n"""

        code += """
#include <Python.h>
#include <pyllars.hpp>
#include <pyllars_classwrapper.cpp>
#include <pyllars_function_wrapper.hpp>
#include "%(myheader)s"
//parent module header: %(id)s
#include "%(myparentheader)s"

using namespace __pyllars_internal;

""" % {'id': self.pseudo_context.id_,
       'myheader': my_header_path, 'headername': self._header_filename, 'namespace_id': nsid,
       'myparentheader': self.pseudo_context.get_header_filename(path)}
        for child in [c for c in self.children if isinstance(c, Type) if c.name != ""]:
            if child.get_header_filename(path):
                code += """
#include "%(header)s"
""" % {'header':child.get_header_filename(path),}
        return code


class Class(Type):
    SCOPES = ["public", "protected", "private"]

    class Method(object):

        QUALIFIERS = ["static", "const"]

        def __init__(self, name, scope, qualifiers, return_type, arguments):
            assert (isinstance(return_type, Type) or return_type is None)
            assert (scope in Class.SCOPES or scope is None)
            self.name = name
            for qualifier in qualifiers or []:
                assert (qualifier in Class.Method.QUALIFIERS)
            self.is_const = 'const' in qualifiers
            self.is_static = 'static' in qualifiers
            for argument in arguments:
                assert (isinstance(argument, tuple) and len(argument) == 2)
                assert (isinstance(argument[1], Type)), "%s is not a Type" % argument[1]
            self.qualifiers = qualifiers
            self.return_type = return_type
            self.arguments = arguments
            self.scope = scope

    # TODO: operator methods

    class Constructor(Method):

        def __init__(self, clazz, scope, qualifiers, arguments):
            Class.Method.__init__(self, clazz, scope, qualifiers, None, arguments)

    class Member(object):

        def __init__(self, name, id_, type_, scope, is_static, bit_field_size=None):
            assert (isinstance(type_, Type))
            assert (scope in Class.SCOPES)
            self.name = name
            self.id_ = id_
            self.type = type_
            self.is_static = is_static
            self.bit_field_size = bit_field_size
            self.scope = scope

        def is_const(self):
            return self.type.is_const()

        def is_static(self):
            return self.is_static

        def get_name(self):
            return self.name or "anonymous%s" % self.id_

        def array_size(self):
            if self.type.get_array_size() is None:
                return "__pyllars_internal::UNKNOWN_SIZE"
            return self.type.get_array_size()

    def __init__(self, name, id_, header_filename, is_incomplete, is_absrtact, context=None, inherited_from=None,
                 scope=None):
        assert (context is None or isinstance(context, Namespace) or isinstance(context, Class))
        Type.__init__(self, name, id_, context, is_incomplete, header_filename, scope)
        self.id_ = id_
        assert(context != self)
        self._header_filename = header_filename
        self.bases = inherited_from or []
        self._methods = []
        self._tainted_methods = []
        self._operator_map_methods=[]
        self.members = []
        self._constructors = []
        self._is_abstract = is_absrtact

    def get_path(self):
        context = self.context
        while not isinstance(context, Namespace):
            context = context.context
            if context is None:
                return self.name + ".cpp"
        return os.path.join(context.get_path(), self.name + ".cpp")

    def add_method(self, method_name, method_scope, qualifiers, return_type, method_parameters):
        """
        add method to this class
        :param qualifiers: list of qualifiers (const, static, ...)
        :param return_type: Return type or None if void return
        :param method_name: string name of method
        :param method_scope: public, protected, private
        :param method_parameters: 2-tuples of type/name pairs
        """
        #assert(not(method_name == "back" and return_type == None))
        self._methods.append(Class.Method(method_name, method_scope, qualifiers, return_type, method_parameters))

    def mark_method_tainted(self, name, qualifiers):
        self._tainted_methods.append(name)

    def add_operator_mapping(self, method_scope, qualifiers, return_type, method_parameter):
        """
        add method to this class
        :param qualifiers: list of qualifiers (const, static, ...)
        :param return_type: Return type or None if void return
        :param method_name: string name of method
        :param method_scope: public, protected, private
        :param method_parameters: 2-tuples of type/name pairs
        """
        self._operator_map_methods.append(Class.Method("operator_map", method_scope, qualifiers, return_type, [method_parameter]))

    def add_constructor(self, method_scope, qualifiers, method_parameters):
        """
        add method to this class
        :param qualifiers: list of qualifiers (const, static, ...)
        :param method_scope: public, protected, private
        :param method_parameters: 2-tuples of type/name pairs
        """
        self._constructors.append(Class.Constructor(self.name, method_scope, qualifiers, method_parameters))

    def add_member(self, member_name, id_, type_, member_scope, qualifiers=None, bit_field_size=None):
        """
        add a class member
        :param type_: Type (python class type) of member
        :param bit_field_size: if bit field, specify integer size of field
        :param member_name: string name of member of this class
        :param member_scope: public, protected, private
        :param qualifiers: const, static, ...
        """
        self.members.append(Class.Member(member_name, id_, type_, member_scope, "static" in (qualifiers or []), bit_field_size))

    def generate_code(self, path, suffix="", class_name=None):
        class_name = class_name or self.get_qualified_name()
        include_path = os.path.join(path, self.get_include_parent_path())
        #  first generate #includes
        if self.context is None:
            nsid = "_1"
        else:
            nsid = self.context.id_
        my_header_path = self.get_header_filename(
            path)  # os.path.join(self.get_include_parent_path(), self.name + ".hpp")
        code = ""
        if suffix == "":
            code += self.generate_preamble(path=path, code=code)
        header_code = ""
        if include_path.startswith('.'):
            guard = include_path[1:].replace('/', '__')
        else:
            guard = include_path.replace('/', '__')
        parent = self.pseudo_context
        while parent is not None and not isinstance(self.context, Namespace):
            parent = parent.context
        mod_header_path = parent.get_header_filename() if parent is not None and parent.name != "<builtin>" else "pyllars.hpp"
        assert (not mod_header_path.startswith('/'))
        indent = ""
        p = self
        namespace_decls = ""
        level = 0
        while p.context is not None:
            if isinstance(p.context, Namespace):
                namespace_decls = "\nnamespace %s{" % (p.context.sanitized_name or "pyllars") + namespace_decls.\
                    replace('\n', '\n' + indent)
                level += 1
                indent += "   "
            elif p.context.sanitized_name:
                namespace_decls = "\nnamespace %s___ns{" % p.context.sanitized_name + namespace_decls.\
                    replace('\n', '\n' + indent)
                level += 1
                indent += "   "
            p = p.context
        namespace_decls += "\n%snamespace %s___ns{\n" % (indent, self.sanitized_name)
        level += 1
        if suffix == "":
            header_code += """#ifndef %(guard)s
#define %(guard)s
#include <pyllars_classwrapper.hpp>
#include <%(path)s>
#ifndef _%(CLASSHEADER)s
#include <%(classheader)s>
#endif

typedef %(full_class_name)s %(sname)s_Target_Type;

    """ % {'path': mod_header_path, 'classheader': self.header_filename, 'guard': guard,
           'CLASSHEADER': os.path.basename(self.header_filename).upper().replace('.', '_'),
           'full_class_name': class_name, 'sname': self.sanitized_name or self.id_} + namespace_decls
        header_code += """
%(indent)sstatus_t initialize_type%(suffix)s();
%(indent)s//PyTypeObject *%(class_name)s_obj = &__pyllars_internal::PythonClassWrapper< %(full_class_name)s >::Type;
""" % {'class_name': self.sanitized_name, 'full_class_name': self.sanitized_name + "_Target_Type", 'indent': indent,
       'suffix': suffix}
        indent2 = indent[3:]
        if suffix == "":
            code += """
typedef const char c_string[];
"""
        for member in [m for m in self.members if m.scope == 'public' and
                       m.type.get_core_type() != self and
                       m.type.get_core_type().name == "" and
                       (isinstance(m.type.get_core_type(), Type) and not m.name == "" and
                        not isinstance(m.type.get_core_type(), FundamentalType))]:
            child = member.type.get_core_type()
            name = "_anonymous%s" % child.id_
            child_class_name = "std::remove_pointer<typename extent_as_pointer" + \
            "<decltype(%(class_name)s::%(name)s)>::type>::type" % {
                'class_name': class_name, 'name': member.name}
            h, c = child.generate_code(path, suffix=name, class_name=child_class_name)
            header_code += h
            code += c
        if suffix == "":
            for _ in range(level):
                header_code += "%s}\n" % indent2
                indent2 = indent2[3:]
            header_code += """

    #endif
    """
        if isinstance(self.context, Namespace):
            parent_init = self.context.get_qualified_name() + "::init"
        else:
            parent_init = self.context.get_qualified_ns_name() + "::initialize_type"
        parent_mod = self.pseudo_context
        while not isinstance(parent_mod, Namespace):
            parent_mod = parent_mod.context

        # initialize type
        if suffix == "":
            for base in self.bases:
                code += """
    #include <%(baseheader)s>
    """ % {'indent': indent,
           'baseheader': base.get_header_filename(path)
           }

        for method_name in set([m.name for m in self._methods if m.scope == 'public' and
                        m.name not in self._tainted_methods]):
            code += """
constexpr c_string %s_methodname = "%s";
""" % (method_name, method_name)

        def add_member_att_names(from_, code2):
            for member_ in set([m_ for m_ in from_.members if m_.scope == 'public']):
                if member_.name == "" and isinstance(member_.type, Class):
                    code2 = add_member_att_names(member_.type, code2)
                    continue
                code2 += """
constexpr c_string %s_attrname = "%s";
""" % (member_.get_name(), member_.get_name())
            return code2

        code = add_member_att_names(self, code)
        typedef_code = ""
        if suffix != "":
            typedef_code = "typedef %s main_type;" % class_name
            class_name = "main_type"
        code += """

static inline status_t initialize_type%(suffix)s(){
   %(typedef_code)s
   static bool inited = false;
   if (inited){
      return 0;
   }
   inited = true;
   status_t status = 0;//pyllars::%(parent_init)s();
   if (status != 0) return status;
""" % {'class_name': class_name, 'name': self.name, 'pyname': self.name,
       'module_name': "%s_mod" % (parent_mod.name or "pyllars"), 'full_name': self.full_name,
       'parent_init': parent_init if not parent_init.startswith("::") else parent_init[2:],
       'indent': "   ", 'namespace_name': parent_mod.get_qualified_name(),
       'suffix': suffix, 'typedef_code': typedef_code}

        def add_members(from_, code2, is_const, is_static):
            for member_ in [m_ for m_ in from_.members if m_.scope == 'public']:
                if member_.name == "" and (isinstance(member_.type, Class)):
                    # recursively add anonymous members
                    code2 = add_members(member_.type, code2, member_.is_const(), member_.is_static)
                    continue
                if member_.name and member_.type.get_core_type().name == "" and isinstance(member_.type.get_core_type(), Class):
                    if member_.type != member_.type.get_core_type():
                        code2 += """
       PythonClassWrapper< decltype(%(class_name)s::%(member_name)s) >::initialize
        ( "decltype(%(class_name)s::%(member_name)s)",
          "decltype(%(class_name)s::%(member_name)s)", nullptr, "decltype(%(class_name)s::%(member_name)s)");
    """ % {'class_name': class_name, 'member_name': member_.name, 'member_type_name': member_.type.get_qualified_name(),
           'indent': indent}
                    code2 += """initialize_type_anonymous%s();
        """ % member_.type.get_core_type().id_
                    code2 += """
       PythonClassWrapper< decltype(%(class_name)s::%(member_name)s) >::initialize
        ( "decltype(%(class_name)s::%(member_name)s)",
          "decltype(%(class_name)s::%(member_name)s)", nullptr, "decltype(%(class_name)s::%(member_name)s)");
    """ % {'class_name': class_name, 'member_name': member_.name, 'member_type_name': member_.type.get_qualified_name(),
           'indent': indent}
                attr_method_name = "add"
                if is_const or member_.is_const():
                    attr_method_name += 'Const'
                if is_static or member_.is_static:
                    attr_method_name += 'Class'
                if member_.name and member_.bit_field_size is None:
                    attr_method_name += "Attribute"
                    code2 += """
       PythonClassWrapper< %(class_name)s >::%(methodname)s
         < %(member_name)s_attrname, decltype(%(class_name)s::%(member_name)s) >
          ( &%(class_name)s::%(member_name)s);
    """ % {'class_name': class_name, 'member_name': member_.name, 'member_type_name': member_.type.get_qualified_name(),
           'array_size': member_.array_size(), 'methodname': attr_method_name, 'indent': indent}
                elif member_.name:
                    attr_method_name += "BitField"
                    args="getter"
                    code2 += """
      {"""
                    if not member_.is_const():
                        code2 += """
          std::function<decltype(%(class_name)s::%(member_name)s)(%(class_name)s&, const decltype(%(class_name)s::%(member_name)s)&)> setter = [](%(class_name)s& cobj, const decltype(%(class_name)s::%(member_name)s) &val){ return cobj.%(member_name)s = val;};
                       """% {'class_name': class_name, 'member_name': member_.name}
                        args += ", setter"
                    code2 += """
          std::function<decltype(%(class_name)s::%(member_name)s)(const %(class_name)s&)> getter = [](const %(class_name)s& cobj){ return cobj.%(member_name)s;};
          PythonClassWrapper< %(class_name)s >::%(methodname)s
            < %(member_name)s_attrname, decltype(%(class_name)s::%(member_name)s), %(bitsize)s >(%(args)s);
       }
    """ % {'bitsize': member_.bit_field_size, 'class_name': class_name, 'member_name': member_.name, 'member_type_name': member_.type.get_qualified_name(),
           'array_size': member_.array_size(), 'methodname': attr_method_name, 'indent': indent, 'args': args}
                    
            return code2

        code = add_members(self, code, False, False)
        for method in [m for m in self._operator_map_methods if m.scope == 'public']:
            qual = "Const" if method.is_const else ""
            code += """
      PythonClassWrapper< %(class_name)s >::addMapOperatorMethod%(qual)s< %(key_type)s,  %(value_type)s >
         ( &%(class_name)s::operator[]);
""" % {'class_name': class_name,
       'value_type': method.return_type.get_qualified_name() if method.return_type else "void",
       'key_type': method.arguments[0][1].get_qualified_name(),
       'indent': indent,
       'qual': qual}
        for method in [m for m in self._methods if m.scope == 'public']:
            call_method_name = "add"
            if method.is_const:
                call_method_name += 'Const'
            if method.is_static:
                call_method_name += 'Class'
            call_method_name += "Method"
            args = []
            kwlist = []
            for arg_name, arg_type in method.arguments:
                args.append(arg_type.get_qualified_name().replace('  ', ' ').replace("const const", "const"))
                kwlist.append('"%s"' % arg_name)
            if len(kwlist) > 0:
                code += """
   {
      static const char* const kwlist[] = {%(kwlist)s, nullptr};
""" % {'indent': indent, 'kwlist': ", ".join(kwlist), }
            else:
                code += """
   {
      static const char* const *kwlist = {nullptr};
"""
            code += """
      PythonClassWrapper< %(class_name)s >::%(callable)s< %(method_name2)s_methodname, %(return_type)s %(args)s >
         ( &%(class_name)s::%(method_name)s, kwlist);
   }
""" % {'class_name': class_name,
       'method_name': method.name,
       'method_name2': method.name,
       'return_type': method.return_type.get_qualified_name() if method.return_type else "void",
       'callable': call_method_name,
       'args': (", " if len(args) else "") + ", ".join(args),
       'indent': indent}
        if not self._is_abstract:
            for constructor in [c for c in self._constructors if c.scope == "public"]:
                call_method_name = "add"
                if constructor.is_const:
                    call_method_name += 'Const'
                call_method_name += "Constructor"
                args = []
                kwlist = []
                for arg_name, arg_type in constructor.arguments:
                    if arg_type.get_core_type().name == "":
                        args.append(class_name + arg_type.name)
                    else:
                        args.append(arg_type.get_qualified_name())
                    kwlist.append('"%s"' % arg_name)
                if len(kwlist) > 0:
                    code += """
       {
          static const char* const kwlist[] = {%(kwlist)s, nullptr};
    """ % {'indent': indent, 'kwlist': ", ".join(kwlist), }
                else:
                    code += """
       {
          static const char* const kwlist[] = {nullptr};
    """
                code += """
          PythonClassWrapper< %(class_name)s>::%(callable)s
          ( kwlist,
            PythonClassWrapper< %(class_name)s >::create< %(args)s >);
       }
    """ % {'class_name': class_name, 'bare_class_name': self.name,
           'callable': call_method_name, 'args': ", ".join(args), 'indent': indent}

        def add_children(elem, from_, code2):
            for child_ in [c_ for c_ in from_.children if isinstance(c_, Type) and c_.scope == 'public']:
                if child_.get_qualified_name().startswith('std::remove'):
                    continue
                if child_.get_qualified_name().startswith('std::is_'):
                    continue
                base_type = child_
                while (not isinstance(base_type, Enumeration) and hasattr(base_type, "base_type") or
                       (isinstance(base_type, Pointer) and isinstance(base_type.base_type, FunctionType))):
                    base_type = base_type.base_type
                if child_.name == "" and isinstance(base_type, Enumeration):
                    for enum, value in child_.enum_values:
                        code2 += """
       PythonClassWrapper<%(full_class_name)s>::addEnumValue( "%(child_name)s", %(value)s);
    """ % {'full_class_name': elem.get_qualified_name(), 'child_name': enum, 'value': value,
           'indent': indent}
                    continue
                if child_.name == "" and  isinstance(child_, Class):
                    # recursively add anonymous union's children
                    add_children(elem, child_, code2)
                    continue
                if len(child_.name) == 0:
                    print("EMPTY CHILD NAME FOR COMPONENT ID %s", child_.id_)
                assert len(child_.name) != 0
                if child_.is_function_type():
                    code2 += """
        {
          PythonFunctionWrapper2< %(child_fullname)s >::initialize_type("%(child_name)s");
          PythonClassWrapper<%(full_class_name)s>::addClassMember( "%(child_name)s",
          (PyObject*)&PythonFunctionWrapper2< %(child_fullname)s >::Type);
        }
    """ % {'full_class_name': elem.get_qualified_name(), 'class_name': elem.name, 'child_name': child_.name,
           'child_fullname': child_.get_qualified_name(),
           'return_is_complete': str(not base_type.return_type.is_incomplete).lower(),
           'indent': indent}
                    continue
                if not child_.is_fundamental() and child_.get_qualified_name(True).replace('_', '').replace('::', '').strip()!="":
                    top_child_context = child_.context
                    while top_child_context.context and top_child_context.context.name and top_child_context.context.name != "::":
                        top_child_context = top_child_context.context
                    top_context = self.context
                    while top_context.context and top_context.context.name and top_context.context.name != "::":
                        top_context = top_context.context
                    if top_context.name == top_child_context.name:
                        code2 += """
   //initialize subtype %(name)s:
   status |= pyllars%(child_name)s::initialize_type();
""" % {'child_name': child_.get_qualified_ns_name(True).replace('<',"__").replace('>','__').replace(',','____').replace(' ','').replace('&','_and_').replace("*","_star_"),
       'name': child_.get_qualified_name(True),
       'indent': indent}
                    else:
                        code2 += """
   //initialize module containing subtype %(name)s:
   if(!PyImport_ImportModule("pyllars.%(name)s")){
      status |= 1;
      PyErr_SetString( PyExc_RuntimeError, "Unable to load module %(name)s");
   }
""" % {'name': top_child_context.name}
            return code2

        code = add_children(self, self, code)
        for base in self.bases:
            code += """
   PythonClassWrapper<%(full_class_name)s>::addBaseClass
      (&PythonClassWrapper< %(base_class_full_name)s >::Type);
""" % {'full_class_name': class_name, 'class_name': self.name,
       'base_class_name': base.get_qualified_ns_name(True),
       'base_class_full_name': base.get_qualified_name(),
       'indent': indent}
        module_name = "pyllars%(namespace_name)s::%(module_name)s" % \
                      {'namespace_name': parent_mod.get_qualified_name(),
                       'module_name': "%s_mod" % (parent_mod.name or "pyllars")} if  suffix == "" else "nullptr"
        code += """    status |= PythonClassWrapper< %(class_name)s >::initialize("%(name)s", "%(pyname)s",%(module_name)s, "%(full_name)s");
""" % {'class_name': class_name, 'name': self.name or "_anonymous%s" % self.id_,
       'pyname': self.name or "_anonymous%s" % self.id_,
       'module_name': module_name,
       'full_name': self.full_name.replace("::::", "::") + ("_anonymous%s" % self.id_ if self.name == "" else ""),
       'parent_init': parent_init if not parent_init.startswith("::") else parent_init[2:],
       'indent': "   ", 'namespace_name': parent_mod.get_qualified_name()}
        code += """
   return status;
}

status_t pyllars%(namespace_name)s::initialize_type%(suffix)s(){
    return ::initialize_type%(suffix)s();
}

static pyllars::Initializer _initializer%(suffix)s(pyllars%(namespace_name)s::initialize_type%(suffix)s);
""" % {'indent': indent, 'namespace_name': self.get_qualified_ns_name(), 'suffix': suffix}
        return header_code, code


class Struct(Class):
    pass



class Referencing(Type):

    def __init__(self, base_type, ext, id_, context, header_filename, scope):
        assert (isinstance(base_type, Type)), "%s: %s is not a Type" % (id_, base_type)
        Type.__init__(self, base_type.name + ext, id_, base_type.context, base_type.is_incomplete, header_filename, scope)
        self.base_type = base_type
        self.ext = ext
        if isinstance(self.base_type, TypeAlias):
            self.base_type.context = context

    @abstractmethod
    def generate_code(self, path):
        raise Exception("Abstract method")

    def is_function_type(self):
        return self.base_type.is_function_type()

    def get_qualified_name(self, sanitized=False):
        if isinstance(self.base_type, FundamentalType):
            return " ".join([self.name] + self.qualifiers)
        elif isinstance(self.base_type, TypeAlias):
            return Type.get_qualified_name(self, sanitized)
        return " ".join([self.base_type.get_qualified_name(sanitized) + self.ext] + self.qualifiers)

    def get_core_type(self):
        return self.base_type.get_core_type()

    def is_fundamental(self):
        return self.base_type.is_fundamental()


def get_alias(clazz, name, aliased_type):
    assert(issubclass(clazz, BaseElement))

    class Alias(clazz):

        def __init__(self):
            assert(isinstance(aliased_type, BaseElement))
            self.alias = aliased_type
            self.name = name
            self.id_ = aliased_type.id_ + ("_typealias_%s" % name)

        def __getattr__(self, item):
            return self.alias.__getattribute__(item)

    return Alias()


class Array(Referencing):
    def __init__(self, base_type, id_, context, header_filename, array_size=None, scope=None):
        """
        :param base_type:
        :param array_size: None means unbounded
        :return:
        """
        assert (isinstance(base_type, Type))
        Referencing.__init__(self, base_type, "[]", id_, context, header_filename, scope)
        assert (isinstance(base_type, Type))
        self.array_size = array_size
        self.base_type = base_type

    def generate_code(self, path):
        pass


class Pointer(Referencing):
    def __init__(self, base_type, id_, context, header_filename, scope=None):
        assert (isinstance(base_type, Type))
        Referencing.__init__(self, base_type, "" if isinstance(base_type, FunctionType) else "*", id_, context,
                             header_filename, scope)

    def generate_code(self, path):
        pass


class Reference(Referencing):
    def __init__(self, base_type, id_, context, header_filename, scope=None):
        Referencing.__init__(self, base_type, "&", id_, context, header_filename, scope)

    def generate_code(self, path):
        pass


class Typedef(Referencing):
    def __init__(self, base_type, id_, context, alias, header_filename, scope=None):
        Referencing.__init__(self, base_type, "", id_, context, header_filename, scope)
        self.alias = alias
        self.set_qualifiers(base_type.qualifiers)

    def generate_code(self, path):
        pass

    def get_qualified_name(self, sanitized=False):
        if self.context and self.context.name == "pyllars":
            return self.name
        context = self.base_type.context
        try:
            self.base_type.context = self.context
            name = super(Typedef, self).get_qualified_name(sanitized)
        finally:
            self.base_type.context = context
        assert(self.base_type.context != self.base_type)
        return name

    def get_header_filename(self, path=None):
        if isinstance(self.base_type, TypeAlias):
            return Referencing.get_header_filename(self)
        return self.base_type.get_header_filename()


class TypeAlias(Typedef):

    pass


class FundamentalType(Type):

    TYPES = ["signed char", "unsigned char", "char", "short int", "short unsigned int", "int", "unsigned int",
             "long int", "long unsigned int", "long long int", "long long unsigned int",
             "__int128", "unsigned __int128", "char16_t", "char32_t",
             "float", "__float128", "double", "long double",  "bool", "wchar_t",
             "void"]

    def __init__(self, type_, id_, size, alignment):
        if type_ not in FundamentalType.TYPES:
            print "TYPE NOT FUNDAMENTAL: " + type_
        assert type_ in FundamentalType.TYPES, "%s not in fundamental types" % type_
        Type.__init__(self, type_, id_, None, False, None, None)
        self.type_ = type_
        self.size = size
        self.alignment = alignment
        self.is_incomplete = False
        self.name = type_

    def get_qualified_name(self, sanitized=False):
        return self.name

    def generate_code(self, path):
        pass

    def is_fundamental(self):
        return True

    def get_include_parent_path(self):
        return None


class CvQualifiedType(Referencing):
    QUALIFIERS = ["const", "volatile"]

    def __init__(self, base_type, id_, context, qualifiers, header_filename, scope=None):
        assert (len(qualifiers) >= 0)
        for qualifier in qualifiers:
            assert (qualifier in CvQualifiedType.QUALIFIERS)
        Referencing.__init__(self, base_type, "", id_, context, header_filename, scope)
        self.set_qualifiers(qualifiers)

    def is_const(self):
        return 'const' in self.qualifiers

    def generate_code(self, path):
        pass


class Enumeration(Type):
    def generate_code(self, path, suffix="", class_name=None):
        class_name = class_name or self.get_qualified_name()
        if isinstance(self.context, Namespace):
            parent_init = self.context.get_qualified_name() + "::init"
        else:
            parent_init = self.context.get_qualified_ns_name() + "::initialize_type"
        parent_mod = self.context
        while not isinstance(parent_mod, Namespace):
            parent_mod = parent_mod.context

        include_path = os.path.join(path, self.get_include_parent_path())
        #  first generate #includes
        if self.context is None:
            nsid = "_1"
        else:
            nsid = self.context.id_
        my_header_path = self.get_header_filename(path)
        if include_path.startswith('.'):
            guard = include_path[1:].replace('/', '__')
        else:
            guard = include_path.replace('/', '__')
        parent = self.context
        while parent is not None and not isinstance(self.context, Namespace):
            parent = parent.context
        mod_header_path = parent.get_header_filename() if parent is not None else "pyllars.hpp"
        assert (not mod_header_path.startswith('/'))
        indent = ""
        p = self
        namespace_decls = ""
        level = 0
        while p.context is not None:
            level += 1
            if isinstance(p.context, Namespace):
                namespace_decls = "\nnamespace %s{" % (p.context.name or "pyllars") + namespace_decls.\
                    replace('\n', '\n' + indent)
            else:
                namespace_decls = "\nnamespace %s___ns{" % p.context.sanitized_name + namespace_decls.\
                    replace('\n', '\n' + indent)
            indent += "   "
            p = p.context
        namespace_decls += "\n%snamespace %s___ns{\n" % (indent, self.sanitized_name)
        level += 1
        header_code = """#ifndef %(guard)s
#define %(guard)s
#include <pyllars_classwrapper.hpp>
#include <%(path)s>
#include <%(classheader)s>

typedef %(full_class_name)s %(sname)s_Target_Type;

""" % {'path': mod_header_path, 'classheader': self.header_filename, 'guard': guard,
       'full_class_name': class_name, 'sname': self.sanitized_name} + namespace_decls
        header_code += """
%(indent)sstatus_t initialize_type%(suffix)s();
%(indent)s//PyTypeObject *%(class_name)s_obj = &__pyllars_internal::PythonClassWrapper< %(full_class_name)s >::Type;
""" % {'class_name': self.sanitized_name, 'full_class_name': self.sanitized_name + "_Target_Type", 'indent': indent,
       'suffix': suffix}
        indent2 = indent[3:]
        for _ in range(level):
            header_code += "%s}\n" % indent2
            indent2 = indent2[3:]
        header_code += """

#endif
"""
        code = ""
        if suffix == "":
            code += """
#include <Python.h>
#include <pyllars.hpp>
#include <pyllars_classwrapper.cpp>
#include <pyllars_function_wrapper.hpp>
#include "%(myheader)s"
//parent module header: %(id)s
#include "%(myparentheader)s"

using namespace __pyllars_internal;

""" % {'id': self.pseudo_context.id_,
       'myheader': my_header_path, 'headername': self._header_filename, 'namespace_id': nsid,
           'myparentheader': self.pseudo_context.get_header_filename(path)}
        code += """static inline status_t initialize_type(){
   static bool inited = false;
   if (inited){
      return 0;
   }
   inited = true;
   status_t status = 0;//// pyllars::%(parent_init)s();
   if (status != 0) return status;
""" % {'class_name': class_name, 'name': self.name, 'pyname': self.name,
       'module_name': "%s_mod" % (self.module_name or "pyllars"), 'full_name': self.full_name,
       'parent_init': parent_init if not parent_init.startswith("::") else parent_init[2:],
       'indent': "   ", 'namespace_name': parent_mod.get_qualified_name()}
        for v in self.enum_values:
            name, val = v
            code += """
   PythonClassWrapper< %(fullname)s >::addEnumValue("%(name)s", %(val)s);
""" % {'val': val, 'name': name, 'fullname': self.get_qualified_name()}
        code += """
   status |= PythonClassWrapper< %(fullname)s >::initialize("%(fullname)s", "%(name)s", %(parent_mod)s,"%(fullname)s");
""" % {'fullname': self.get_qualified_name(True).replace('<',"__").replace('>','__').replace(',','__').replace('&','_and_').replace("*","_star_"),
       'name': self.name, 'parent_mod': ("pyllars%s::%s_mod" % (self.pseudo_context.get_qualified_ns_name(),
                                                                self.module_name))
        if isinstance(self.context, Namespace) else "nullptr"}
        if not isinstance(self.context, Namespace):
            code += """
   PythonClassWrapper< %(parent_fullname)s>::addClassMember( "%(name)s",
       (PyObject*) &PythonClassWrapper< %(fullname)s>::Type);
""" % {'parent_fullname': self.context.get_qualified_name(),
       'fullname': class_name,
       'name': self.name}
        namespace_name = "pyllars" + parent_mod.get_qualified_name()
        suffix = ""
        context = self.context
        while context is not None and not isinstance(context, Namespace):
            suffix = "::" + context.name + "___ns" + suffix
            context = context.context
        namespace_name += suffix + "::" + self.name + "___ns"
        code += """
   return status;
}

status_t %(namespace_name)s::initialize_type(){
    return ::initialize_type();
}

static pyllars::Initializer _initializer(%(namespace_name)s::initialize_type);
""" % {'namespace_name': namespace_name}
        return header_code, code

    def __init__(self, name, id_, context, enumerators, header_filename, base_type=None, is_incomplete=False,
                 scope=None):
        Type.__init__(self, name, id_, context, is_incomplete, header_filename, scope)
        assert (base_type is None or isinstance(base_type, Type))
        self.base_type = base_type or FundamentalType("int", "internal", 16, 8)
        for value in enumerators or []:
            assert (isinstance(value[1], int))
        self.enum_values = enumerators or []
        self._header_filename = header_filename


class Union(Class):
    #def generate_code(self, path, suffix="", class_name=None):
        pass


class FunctionType(Type):
    def __init__(self, id_, return_type, arguments, header_filename, qualifiers=None, context=None, name=None, scope=None):
        assert (isinstance(return_type, Type))
        Type.__init__(self, name or self.generate_type_name(arguments, return_type, [] if not qualifiers else ("const" in qualifiers)),
                      id_, context, False, header_filename, scope)
        for argument in arguments or []:
            assert (isinstance(argument, tuple))
            assert (len(argument) == 2)
            assert (isinstance(argument[1], Type))
        self.arguments = arguments or []
        for index, argument in enumerate(arguments or []):
            if argument[0] is None:
                self.arguments[index] = ("_%d" % index, arguments[index][1])
        # self.name = "func%s"%id_
        self.return_type = return_type
        if not name:
            self.sanitized_name = self.generate_type_name(arguments, return_type, [] if not qualifiers else ("const" in qualifiers))
        self.is_anon = name is None or name is ""

    def generate_code(self, path):
        pass

    def is_function_type(self):
        return True

    def get_qualified_name(self, sanitized=False):
        if self.is_anon:
            return self.name
        return super(FunctionType, self).get_qualified_name(sanitized=sanitized)

    @classmethod
    def generate_type_name(cls, arguments, return_type, is_const):
        args = ", ".join([type.get_qualified_name() for _, type in arguments or []])
        qual = "const" if is_const else ""
        return "%(return_type)s (*)(%(args)s) %(qual)s" % {'return_type': return_type.name,
                                                           'args': args,
                                                           'qual': qual}


class Function(object):

    def __init__(self, id_, name, return_type, context, header_filename, arguments=None, scope=None):
        assert (isinstance(return_type, Type))
        assert (isinstance(context, Namespace) or isinstance(context, Type))
        self.header_filename = system_patch(header_filename) or header_filename
        for argument in arguments or []:
            assert (isinstance(argument, tuple))
            assert (len(argument) == 2)
            assert (isinstance(argument[1], Type))
        self.arguments = arguments or []
        for index, argument in enumerate(self.arguments):
            if argument[0] is None:
                self.arguments[index] = ("_%d" % index, argument[1])
        self.name = name
        self.return_type = return_type
        assert(self != context and context is not None)
        self.context = context
        self.scope = scope
        self.context.children.append(self)
        self.id_ = id_

    def generate_code(self, path):
        pass

    def get_header_filename(self):
        return self.header_filename

    def get_body_filename(self):
        return self.header_filename.replace('.h','.c')

# TODO: handle operator functions
