import os
from abc import abstractmethod


class BaseElement(object):
    def __init__(self):
        self.children = []


# noinspection PyUnusedLocal
class Namespace(BaseElement):
    """
    Class to capture namespace properties
    """

    namespaces = {}

    def __init__(self, name, id_, header_filename, context):
        BaseElement.__init__(self)
        assert (context is None or isinstance(context, Namespace))
        assert (id is not None)
        assert (name is not None)
        self.name = name.replace("::", "")
        self.id_ = id_
        self.context = context
        if context is None or context.name == "::" or context.name =="":
            self.is_global_namespace = True
        if context and context.full_name != "::":
            self.full_name = context.full_name + "::" + name
        else:
            self.full_name = name
        if self.full_name not in Namespace.namespaces:
            Namespace.namespaces[self.full_name] = self
            Namespace.namespaces[id_] = Namespace.namespaces[self.full_name]
        self._classes = {}
        self.from_header = header_filename
        self.sanitized_name = name.replace("<", "__").replace(">", "__").replace(",", "___").replace(" ", "_"). \
            replace("::", "____")

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
        if self.context is None or self.context.name == "::":
            return "pyllars"
        else:
            return os.path.join(self.context.get_include_parent_path(), self.name or "pyllars").replace(" ", "_"). \
                replace("<", "__").replace(">", "__").replace("::", "____").replace(", ", "__")

    def get_header_filename(self, path=None):
        return os.path.join(self.get_include_parent_path(), "module.hpp") if path is None else \
            os.path.join(path, self.get_include_parent_path(), "module.hpp")

    def get_body_filename(self, path=None):
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
        context_name = self.context.name if self.context is not None else "pyllars_mod"
        if self.context is not None:
            context_name = context_name + "::" + context_name + "_mod"
        precode = ""
        precode += """
%(indent)sstatus_t init_functions(PyObject *%(name)s_mod){
%(indent)s   int status = 0;
""" % {'name': self.name, 'indent': indent, 'contextname': context_name, 'fullname': self.full_name,
       'parent_init': "pyllars%s::init();" % self.context.get_qualified_name()
       if self.context and self.context.full_name != "::" else ""}
        for func in [f for f in self.children if isinstance(f, Function)]:
            precode += """%(indent)s
%(indent)s      {
%(indent)s         static const char* const argumentnames[] = {%(argument_names)s};
%(indent)s         status |= PyModule_AddObject( %(name)s_mod, "%(func_name)s",
%(indent)s              (PyObject*)__pyllars_internal::PythonFunctionWrapper<__pyllars_internal::
%(indent)s                 is_complete< %(return_type)s >::value, %(return_type)s %(arguments)s>::
%(indent)s                 create("%(func_name)s", %(context)s::%(func_name)s, argumentnames));
%(indent)s      }""" % {'indent': indent,
                        'name': self.name.replace("::::", "::"),
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
%(indent)s""" % {'name': p.name or "pyllars", 'indent': indent} + code
            p = p.context
            depth += 1
            fullindent += indent
        if self.name != "" and self.name != "::":
            code = "namespace pyllars{\n" + code
        indent = fullindent[3:]
        header_code = header_code + code + """
%(indent)sextern PyObject* %(name)s_mod;

%(indent)sstatus_t init();
""" % {'name': self.name, 'indent': indent}
        code += """
%(indent)sPyObject* %(name)s_mod;
""" % {'name': self.name, 'indent': indent}

        code = precode + code
        code += """
%(indent)sstatus_t init(){
%(indent)s   if (%(name)s_mod) return 0;// if already initialized
%(indent)s   %(parent_init)s
%(indent)s   int status = 0;
%(indent)s   %(name)s_mod = Py_InitModule3("%(name)s", nullptr,"Module corresponding to C++ namespace %(fullname)s");
%(indent)s   if( %(name)s_mod ){
%(indent)s      status |= init_functions(%(name)s_mod);
""" % {'name': self.name, 'indent': indent, 'contextname': context_name, 'fullname': self.full_name or "pyllars",
       'parent_init': "pyllars%s::init();" % self.context.get_qualified_name()
       if self.context and self.context.full_name != "::" else ""}
        if self.context is not None and self.context.context is not None:
            code += """
%(indent)s      status |= PyModule_AddObject( %(contextname)s, "%(name)s", %(name)s_mod );
""" % {'name': self.name, 'indent': indent, 'contextname': context_name, 'fullname': self.full_name or "pyllars",
       'parent_init': "pyllars%s::init();" % self.context.get_qualified_name()
       if self.context and self.context.full_name != "::" else ""}
        code += """
%(indent)s   } else {
%(indent)s      status = -1;
%(indent)s   }
%(indent)s   return status;
%(indent)s}

%(indent)s//add init to methods to be called on intiailization
%(indent)sstatic pyllars::Initializer __initializer( init );

""" % {'name': self.name, 'indent': indent, 'contextname': context_name, 'fullname': self.full_name or "pyllars",
       'parent_init': "pyllars%s::init();" % self.context.get_qualified_name()
       if self.context and self.context.full_name != "::" else ""}

        #  closing brackets:
        for i in range(depth):
            indent = indent[:-3]
            code += indent + '}\n'
            header_code += indent + '}\n'
        if self.name != "" and self.name != "::":
            code += "}\n"
        if self.name != "" or self.name != "::":
            header_code += "\n}"
        code += "// END %s" % self.full_name
        if include_path.startswith('.'):
            guard = include_path[1:].replace('/', '__')
        else:
            guard = include_path.replace('/', '__')
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
        my_header_path = self.get_header_filename(
            path)  # os.path.join(self.get_include_parent_path(), self.name + ".hpp")
        if suffix == "":
            code = """
#include <Python.h>
#include <%(path)s/module.hpp>
#include <pyllars_classwrapper.impl>
#include <pyllars_function_wrapper.hpp>
#include "%(header)s"
""" % {'header': self.from_header, 'path': self.get_include_parent_path()} + code
        return header_code, code

    def get_qualified_name(self, iterative=False):
        if self.context is not None:
            return "::".join([self.context.get_qualified_ns_name(True), self.name])
        elif not iterative:
            return self.name
        else:
            return ""

    def get_qualified_ns_name(self, sanitized=False):
        return self.get_qualified_name(sanitized)


# noinspection PyMethodMayBeStatic
class Type(BaseElement):

    @staticmethod
    def get_module_name(context, fallback_name):
        if not context:
            return None
        parent_mod = context
        while not isinstance(parent_mod, Namespace):
            parent_mod = parent_mod.context
        return parent_mod.name if not parent_mod.is_global_namespace or not fallback_name else fallback_name

    def __init__(self, name, id_, context, is_incomplete, header_filename, scope):
        BaseElement.__init__(self)
        assert (isinstance(context, Type) or isinstance(context, Namespace) or context is None)
        assert (id_ is not None)
        assert (name is not None)
        self.name = name.replace('&lt;', '<').replace('&gt;', '>')
        self.id_ = id_
        self.context = context
        self.module_name = Type.get_module_name(context, os.path.basename(header_filename).split('.')[0] if header_filename else None)
        self.is_incomplete = is_incomplete
        self.header_filename = header_filename
        if context:
            self.full_name = context.full_name + "::" + name
        else:
            self.full_name = name
        self.qualifiers = []
        self.sanitized_name = name.replace("<", "__").replace(">", "__").replace(",", "___").replace(" ", "_"). \
            replace("::", "____")
        self.scope = scope
        self.is_incomplete = is_incomplete

    def get_qualified_ns_name(self, sanitized=False):
        return self.context.get_qualified_ns_name(sanitized) + (
        "::" + self.sanitized_name + "___ns" if self.sanitized_name != "" else "")

    def get_qualified_name(self, sanitized=False):
        name = self.name if not sanitized else self.sanitized_name
        if self.context is not None:
            return "::".join([self.context.get_qualified_name(), name]) + " " + " ".join(self.qualifiers)
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

    @abstractmethod
    def generate_code(self, path):
        pass

    def get_include_parent_path(self):
        return os.path.join(self.context.get_include_parent_path(), self.sanitized_name)

    def get_header_filename(self, path=None):
        return os.path.join(self.get_include_parent_path(), self.sanitized_name + ".hpp") if path is None else \
            os.path.join(path, self.get_include_parent_path(), self.sanitized_name + ".hpp")

    def get_body_filename(self, path=None):
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
        code += """
#include <Python.h>
#include <pyllars.hpp>
#include <pyllars_classwrapper.impl>
#include <pyllars_function_wrapper.hpp>
#include <%(headername)s>
#include "%(myheader)s"
#include "%(myparentheader)s"

using namespace __pyllars_internal;

""" % {'myheader': my_header_path, 'headername': self._header_filename, 'namespace_id': nsid,
       'myparentheader': self.context.get_header_filename(path)}
        for child in [c for c in self.children if isinstance(c, Type) if c.name != ""]:
            code += """
#include "%(header)s"
""" % {'header': os.path.join(os.path.dirname(my_header_path), child.name, child.name + ".hpp")}
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
                assert (isinstance(argument[1], Type))
            self.qualifiers = qualifiers
            self.return_type = return_type
            self.arguments = arguments
            self.scope = scope

    # TODO: operator methods

    class Constructor(Method):

        def __init__(self, clazz, scope, qualifiers, arguments):
            Class.Method.__init__(self, clazz, scope, qualifiers, None, arguments)

    class Member(object):

        def __init__(self, name, type_, scope, is_static, bit_field_size=None):
            assert (isinstance(type_, Type))
            assert (scope in Class.SCOPES)
            self.name = name
            self.type = type_
            self.is_static = is_static
            self.bit_field_size = bit_field_size
            self.scope = scope

        def is_const(self):
            return self.type.is_const()

        def is_static(self):
            return self.is_static

        def array_size(self):
            if self.type.get_array_size() is None:
                return "__pyllars_internal::UNKNOWN_SIZE"
            return self.type.get_array_size()

    def __init__(self, name, id_, header_filename, is_incomplete, is_absrtact, context=None, inherited_from=None,
                 scope=None):
        assert (context is None or isinstance(context, Namespace) or isinstance(context, Class))
        Type.__init__(self, name, id_, context, is_incomplete, header_filename, scope)
        self.id_ = id_
        self.context = context
        self._header_filename = header_filename
        self.bases = inherited_from or []
        self._methods = []
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
        self._methods.append(Class.Method(method_name, method_scope, qualifiers, return_type, method_parameters))

    def add_constructor(self, method_scope, qualifiers, method_parameters):
        """
        add method to this class
        :param qualifiers: list of qualifiers (const, static, ...)
        :param method_scope: public, protected, private
        :param method_parameters: 2-tuples of type/name pairs
        """
        self._constructors.append(Class.Constructor(self.name, method_scope, qualifiers, method_parameters))

    def add_member(self, member_name, type_, member_scope, qualifiers=None, bit_field_size=None):
        """
        add a class member
        :param type_: Type (python class type) of member
        :param bit_field_size: if bit field, specify integer size of field
        :param member_name: string name of member of this class
        :param member_scope: public, protected, private
        :param qualifiers: const, static, ...
        """
        print "################# ADDING MEMBER %s"%member_name
        self.members.append(Class.Member(member_name, type_, member_scope, "static" in (qualifiers or []), bit_field_size))

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
        if suffix == "":
            header_code += """#ifndef %(guard)s
    #define %(guard)s
    #include <pyllars_classwrapper.hpp>
    #include <%(path)s>
    #include <%(classheader)s>

    typedef %(full_class_name)s %(sname)s_Target_Type;

    """ % {'path': mod_header_path, 'classheader': self.header_filename, 'guard': guard,
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
                       (isinstance(m.type.get_core_type(), Type) and not isinstance(m.type.get_core_type(), Union) and
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
        parent_mod = self.context
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

        for method_name in set([m.name for m in self._methods if m.scope == 'public']):
            code += """
constexpr c_string %s_name = "%s";
""" % (method_name, method_name)

        def add_member_att_names(from_, code2):
            for member_ in set([m_ for m_ in from_.members if m_.scope == 'public']):
                if member_.name == "" and (isinstance(member_.type, Union) or isinstance(member_.type, Class)):
                    code2 = add_member_att_names(member_.type, code2)
                    continue
                code2 += """
constexpr c_string %s_attrname = "%s";
""" % (member_.name, member_.name)
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
   status_t status = pyllars::%(parent_init)s();
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
                if member_.type.get_core_type().name == "" and isinstance(member_.type.get_core_type(), Class) and \
                        not isinstance(member_.type.get_core_type(), Union):
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
                attr_method_name += "Attribute"

                code2 += """
       PythonClassWrapper< %(class_name)s >::%(methodname)s
         < %(member_name)s_attrname, decltype(%(class_name)s::%(member_name)s) >
          ( &%(class_name)s::%(member_name)s);
    """ % {'class_name': class_name, 'member_name': member_.name, 'member_type_name': member_.type.get_qualified_name(),
           'array_size': member_.array_size(), 'methodname': attr_method_name, 'indent': indent}
            return code2

        code = add_members(self, code, False, False)
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
                args.append(arg_type.get_qualified_name())
                kwlist.append('"%s"' % arg_name)
            if len(kwlist) > 0:
                code += """
   {
      static const char* const kwlist[] = {%(kwlist)s};
""" % {'indent': indent, 'kwlist': ", ".join(kwlist), }
            else:
                code += """
   {
      static const char* const *kwlist = nullptr;//[] = {};
"""
            code += """
      PythonClassWrapper< %(class_name)s >::%(callable)s< %(method_name2)s_name, %(return_type)s %(args)s >
         ( &%(class_name)s::%(method_name)s, kwlist);
   }
""" % {'class_name': class_name,
       'method_name': method.name,
       'method_name2': method.name, 'return_type': method.return_type.get_qualified_name(),
       'callable': call_method_name, 'args': (", " if len(args) else "") + ", ".join(args), 'indent': indent}
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
          static const char* const kwlist[] = {%(kwlist)s};
    """ % {'indent': indent, 'kwlist': ", ".join(kwlist), }
                else:
                    code += """
       {
          static const char* const kwlist[] = {};
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
                if child_.name == "" and (isinstance(child_, Union) or isinstance(child_, Class)):
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
                code2 += """
   //initialize subtype:
   status |= pyllars%(full_class_name)s___ns::%(child_name)s___ns::initialize_type();
""" % {'full_class_name': class_name.strip(), 'class_name': elem.name, 'child_name': child_.name,
       'child_fullname': child_.get_qualified_name(),
       'indent': indent}
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
        code += """status |= PythonClassWrapper< %(class_name)s >::initialize("%(name)s", "%(pyname)s",%(module_name)s, "%(full_name)s");
""" % {'class_name': class_name, 'name': self.name or "_anonymous%s" % self.id_,
       'pyname': self.name or "_anonymous%s" % self.id_,
       'module_name': module_name,
       'full_name': self.full_name.replace("::::", "::") + ("_anaonymous%s" % self.id_ if self.name == "" else ""),
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
        assert (isinstance(base_type, Type))
        Type.__init__(self, base_type.name + ext, id_, context, base_type.is_incomplete, header_filename, scope)
        self.base_type = base_type
        self.context = base_type.context
        self.ext = ext

    @abstractmethod
    def generate_code(self, path):
        raise Exception("Abstract method")

    def is_function_type(self):
        return self.base_type.is_function_type()

    def get_qualified_name(self, sanitized=False):
        if isinstance(self.base_type, FundamentalType):
            return " ".join([self.name] + self.qualifiers)
        return " ".join([self.base_type.get_qualified_name() + self.ext] + self.qualifiers)

    def get_core_type(self):
        return self.base_type.get_core_type()


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
        self.context = context

    def generate_code(self, path):
        pass

    def get_qualified_name(self, sanitized=False):
        if self.context and self.context.name == "pyllars":
            return self.name
        context = self.base_type.context
        try:
            self.base_type.context = self.context
            name = super(Typedef, self).get_qualified_name()
        finally:
            self.base_type.context = context
        return name


class FundamentalType(Type):

    TYPES = ["signed char", "unsigned char", "char", "short int", "short unsigned int", "int", "unsigned int",
             "long int", "long unsigned int", "long long int", "long long unsigned int",
             "__int128", "unsigned __int128",
             "float", "__float128", "double", "bool",
             "void"]

    def __init__(self, type_, id_, size, alignment):
        if type_ not in FundamentalType.TYPES:
            print "TYPE NOT FUNDAMENTAL: " + type_
        assert (type_ in FundamentalType.TYPES)
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
        my_header_path = self.get_header_filename(
            path)  # os.path.join(self.get_include_parent_path(), self.name + ".hpp")
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
#include <pyllars_classwrapper.impl>
#include <pyllars_function_wrapper.hpp>
#include <%(headername)s>
#include "%(myheader)s"
#include "%(myparentheader)s"

using namespace __pyllars_internal;

""" % {'myheader': my_header_path, 'headername': self._header_filename, 'namespace_id': nsid,
           'myparentheader': self.context.get_header_filename(path)}
        code += """static inline status_t initialize_type(){
   static bool inited = false;
   if (inited){
      return 0;
   }
   inited = true;
   status_t status = pyllars::%(parent_init)s();
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
""" % {'fullname': class_name, 'name': self.name, 'parent_mod': ("%s_mod" % self.module_name)
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
    def generate_code(self, path, suffix="", class_name=None):
        pass


class FunctionType(Type):
    def __init__(self, id_, return_type, arguments, header_filename, context=None, name=None, scope=None):
        assert (isinstance(return_type, Type))
        Type.__init__(self, name if name is not None else "func%s" % id_, id_, context, False, header_filename, scope)
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

    def generate_code(self, path):
        pass

    def is_function_type(self):
        return True


class Function(object):

    def __init__(self, id_, name, return_type, context, arguments=None, scope=None):
        assert (isinstance(return_type, Type))
        assert (isinstance(context, Namespace) or isinstance(context, Type))
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
        self.context = context
        self.scope = scope
        self.context.children.append(self)
        self.id_ = id_

    def generate_code(self, path):
        pass

# TODO: handle operator functions
