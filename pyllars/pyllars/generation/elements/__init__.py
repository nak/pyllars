import os
from abc import abstractmethod


class BaseElement(object):

    def __init__(self):
        self.children = []


class Namespace(BaseElement):
    """
    Class to capture namespace properties
    """

    namespaces = {}

    def __init__(self, name, id_, context=None):
        BaseElement.__init__(self)
        assert (context is None or isinstance(context, Namespace))
        assert (id is not None)
        assert (name is not None)
        self.name = name.replace("::", "")
        self._id = id_
        self.context = context

        if context and context.full_name!="::":
            self.full_name = context.full_name + "::" + name
        else:
            self.full_name = name
        if self.full_name not in Namespace.namespaces:
            Namespace.namespaces[self.full_name] = self
            Namespace.namespaces[id_] = Namespace.namespaces[self.full_name]
        self._classes = {}
        if self.name == "":
            self.name = "pyllars"

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
            return os.path.join(self.context.get_include_parent_path(), self.name).replace(" ","_").replace("<", "__").\
                replace(">", "__").replace("::", "____").replace(", ", "__")

    def get_header_filename(self, path=None):
        return os.path.join(self.get_include_parent_path(), "module.hpp") if path is None else\
            os.path.join(path, self.get_include_parent_path(), "module.hpp")

    def get_body_filename(self, path=None):
        return os.path.join(self.get_include_parent_path(), "module.cpp") if path is None else\
            os.path.join(path, self.get_include_parent_path(), "module.cpp")

    def generate_code(self, path):
        include_path = os.path.join(path, self.get_include_parent_path())
        if not os.path.exists(include_path):
            os.makedirs(include_path)

        code = ""
        header_code = ""
        p = self
        depth = 0
        indent = "   "
        fullindent = indent
        while p is not None and p.name != "" and p.name != "::":
            code = """%(indent)snamespace %(name)s{
%(indent)s""" % {'name': p.name, 'indent': indent} + code
            p = p.context
            depth += 1
            fullindent += indent
        print "############!!!!!!!!!!!!!!!!!!! %s" % code
        indent = fullindent[3:]
        header_code = header_code + code + """
%(indent)sextern PyObject* %(name)s_mod;

%(indent)sstatus_t init();
""" % {'name': self.name, 'indent': indent}
        code += """
%(indent)sPyObject* %(name)s_mod;
""" % {'name': self.name, 'indent': indent}
        context_name = self.context.name if self.context is not None else "pyllars_mod"
        if self.context is not None:
            context_name = context_name + "::" + context_name + "_mod"
        code += """
%(indent)sstatus_t _init(){
%(indent)s   if (%(name)s_mod) return 0;// if already initialized
%(indent)s   %(parent_init)s
%(indent)s   int status = 0;
%(indent)s   %(name)s_mod = Py_InitModule3("%(name)s", nullptr,"Module corresponding to C++ namespace %(fullname)s");
%(indent)s   if( %(name)s_mod ){
%(indent)s      status |= PyModule_AddObject( ::%(contextname)s, "%(name)s", %(name)s_mod );
%(indent)s   } else {
%(indent)s      status = -1;
%(indent)s   }
%(indent)s   return status;
%(indent)s}

%(indent)s//add init to methods to be called on intiailization
%(indent)sstatic pyllars::Initializer __initializer( init );

""" % {'name': self.name, 'indent': indent, 'contextname': context_name, 'fullname': self.full_name,
       'parent_init': "pyllars%s::init();" % self.context.get_qualified_name() if self.context and self.context.full_name != "::" else ""}

        #  closing brackets:
        for i in range(depth):
            indent = indent[:-3]
            code += indent + '}\n'
            header_code += indent + '}\n'
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
        assert(not mod_header_path.startswith('/'))
        header_code = """#ifndef %(guard)s
#define %(guard)s
#include <%(path)s>

"""%{'path': mod_header_path, 'guard': guard} + header_code + """

#endif
"""
        code = """
#include <Python.h>
#include <%(path)s/module.hpp>

""" % {'path': self.get_include_parent_path()} + code
        return header_code, code

    def get_qualified_name(self, iterative=False):
        if self.context is not None:
            return "::".join([self.context.get_qualified_ns_name(True), self.name])
        elif not iterative:
            return "::" + self.name
        else:
            return ""

    def get_qualified_ns_name(self, sanitized = False):
        return self.get_qualified_name(sanitized)


class Type(BaseElement):
    def __init__(self, name, id_, context, is_incomplete, header_filename):
        BaseElement.__init__(self)
        assert(isinstance(context, Type) or isinstance(context, Namespace) or context is None)
        assert (id_ is not None)
        assert (name is not None)
        self.name = name.replace('&lt;', '<').replace('&gt;', '>')
        self._id = id_
        self.context = context
        self.is_incomplete = is_incomplete
        self.header_filenanme = header_filename
        if context:
            self.full_name = context.full_name + "::" + name
        else:
            self.full_name = name
        self.qualifiers = []
        self.sanitized_name = name.replace("<", "__").replace(">", "__").replace(",", "___").replace(" ", "_").\
            replace("::", "____")

    def get_qualified_ns_name(self, sanitized=False):
        return self.context.get_qualified_ns_name(sanitized) + "::" + self.sanitized_name + "___ns"

    def get_qualified_name(self, sanitized=False):
        name = self.name if not sanitized else self.sanitized_name
        if self.context is not None:
            return " ".join(self.qualifiers) + " " + "::".join([self.context.get_qualified_name(), name])
        elif isinstance(self, FundamentalType):
            return " ".join(self.qualifiers) + " " + name
        else:
            return " ".join(self.qualifiers) + " " + "::" + name

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
#        return self.context.get_include_parent_path()
        return os.path.join(self.context.get_include_parent_path(), self.sanitized_name)
#        if self.context is None or self.context.name == "::":
#            return "pyllars"
#        elif not isinstance(self.context, Namespace):
#            return self.context.context.get_include_parent_path()
#        else:
#            return os.path.join(self.context.get_include_parent_path(), self.name).\
#                replace(" ","_").replace("<", "__").replace(">", "__").replace("::", "____").replace(", ", "__")

    def get_header_filename(self, path=None):
        return os.path.join(self.get_include_parent_path(), self.sanitized_name +".hpp") if path is None else\
            os.path.join(path, self.get_include_parent_path(), self.sanitized_name +".hpp")

    def get_body_filename(self, path=None):
        return os.path.join(self.get_include_parent_path(), self.sanitized_name +".cpp") if path is None else\
            os.path.join(path, self.get_include_parent_path(), self.sanitized_name +".cpp")


class Class(Type):
    SCOPES = ["public", "protected", "private"]

    class Method(object):

        QUALIFIERS = ["static", "const"]

        def __init__(self, name, scope, qualifiers, return_type, arguments):
            assert (isinstance(return_type, Type) or return_type is None)
            print "SCOPE IS %s"%scope
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

    #  TODO: operator methods

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

    def __init__(self, name, id_, header_filename, is_incomplete, context=None, inherited_from=None):
        assert (context is None or isinstance(context, Namespace) or isinstance(context, Class))
        Type.__init__(self, name, id_, context, is_incomplete, header_filename)
        self._id = id_
        self.context = context
        self._header_filename = header_filename
        self.bases = inherited_from or []
        self._methods = []
        self._members = []
        self._members = []
        self._constructors = []

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
        :param return_type: Return type or None if void return
        :param method_scope: public, protected, private
        :param method_parameters: 2-tuples of type/name pairs
        """
        self._constructors.append(Class.Constructor(self.name, method_scope, qualifiers, method_parameters))

    def add_member(self, member_name, type_, member_scope, qualifiers=None, bit_field_size=None):
        """
        add a class member
        :param bit_field_size: if bit field, specify integer size of field
        :param member_name: string name of member of this class
        :param member_scope: public, protected, private
        :param qualifiers: const, static, ...
        """
        self._members.append(Class.Member(member_name, type_, member_scope, qualifiers, bit_field_size))

    def generate_code(self, path):
        include_path = os.path.join(path, self.get_include_parent_path())
        #  first generate #includes
        if self.context is None:
            nsid = "_1"
        else:
            nsid = self.context._id
        my_header_path = self.get_header_filename(path)#  os.path.join(self.get_include_parent_path(), self.name + ".hpp")
        code = """
#include <Python.h>
#include <pyllars.hpp>
#include <%(headername)s>
#include "%(myheader)s"
#include "%(myparentheader)s"

using namespace __pyllars_internal;

""" % {'myheader': my_header_path, 'headername': self._header_filename, 'namespace_id': nsid,
       'myparentheader': self.context.get_header_filename(path)}
        if include_path.startswith('.'):
            guard = include_path[1:].replace('/','__')
        else:
            guard = include_path.replace('/', '__')
        parent = self.context
        while parent is not None and not isinstance(self.context, Namespace):
            parent = parent.context
        mod_header_path = parent.get_header_filename() if parent is not None else "pyllars.hpp"
        assert(not mod_header_path.startswith('/'))
        indent = ""
        p = self
        namespace_decls = ""
        level = 0
        while p.context is not None:
            level += 1
            if isinstance(p.context, Namespace):
                namespace_decls = "\nnamespace %s{" % p.context.name + namespace_decls.replace('\n','\n' + indent)
            else:
                namespace_decls = "\nnamespace %s___ns{" % p.context.sanitized_name + namespace_decls.replace('\n','\n' + indent)
            indent += "   "
            p = p.context
        namespace_decls += "\n%snamespace %s___ns{\n" % (indent, self.sanitized_name)
        level += 1
        header_code = """#ifndef %(guard)s
#define %(guard)s
#include <pyllars_classwrapper.hpp>
#include <%(path)s>
#include <%(classheader)s>

typedef %(full_classname)s %(sname)s_Target_Type;

"""%{'path': mod_header_path, 'classheader': self.header_filenanme, 'guard': guard,
     'full_classname': self.get_qualified_name(), 'sname': self.sanitized_name} + namespace_decls
        header_code += """
%(indent)sstatus_t initialize_type();
%(indent)sPyTypeObject *%(classname)s_obj = &__pyllars_internal::PythonClassWrapper< %(full_classname)s >::Type;
""" % {'classname': self.sanitized_name, 'full_classname': self.sanitized_name + "_Target_Type", 'indent': indent}
        indent2 = indent[3:]
        for _ in range(level):
            header_code += "%s}\n" % indent2
            indent2 = indent2[3:]
        header_code += """

#endif
"""
        if isinstance(self.context, Namespace):
            parent_init = self.context.name + "::init"
        else:
            parent_init = self.context.get_qualified_ns_name() + "::initialize_type"
        parent_mod = self.context
        while not isinstance(parent_mod, Namespace):
            parent_mod = parent_mod.context

        # initialize type
        for base in self.bases:
            code += """
#include <%(baseheader)s>
""" % {'indent': indent,
       'baseheader': base.get_header_filename(path)
       }
        code += """
typedef const char c_string[];
"""
        for method_name in set([m.name for m in self._methods if m.scope == 'public']):
            code += """
constexpr c_string %s_name = "%s";
"""% (method_name, method_name)
        for member_name in set([m.name for m in self._members if m.scope == 'public']):
            code += """
constexpr c_string %s_attrname = "%s";
"""% (member_name, member_name)
        code += """

static inline status_t initialize_type(){
   status_t status = pyllars::%(parent_init)s();
   if (status != 0) return status;
   status |= PythonClassWrapper< %(classname)s >::initialize("%(name)s", "%(pyname)s", pyllars%(namespace_name)s::%(module_name)s, "%(full_name)s");
""" % {'classname': self.get_qualified_name(), 'name': self.name, 'pyname': self.name,
       'module_name': "%s_mod" % parent_mod.name, 'full_name': self.full_name, 'parent_init': parent_init if not parent_init.startswith("::") else parent_init[2:],
       'indent': "   ", 'namespace_name': parent_mod.get_qualified_name()}
        for member in [m for m in self._members if m.scope == 'public']:
            attr_method_name = "add"
            if member.is_const():
                attr_method_name += 'Const'
            if member.is_static:
                attr_method_name += 'Class'
            attr_method_name += "Attribute"
            code += """
   PythonClassWrapper< %(classname)s >::%(methodname)s< %(member_name)s_attrname, %(member_type_name)s >
      ( &%(classname)s::%(member_name)s, %(array_size)s);
""" % {'classname': self.get_qualified_name(), 'member_name': member.name, 'member_type_name': member.type.get_qualified_name(),
       'array_size': member.array_size(), 'methodname': attr_method_name, 'indent': indent}
        for method in [m for m in self._methods if m.scope == 'public']:
            call_method_name = "add"
            if method.is_const:
                call_method_name += 'Const'
            if method.is_static:
                call_method_name += 'Class'
            call_method_name += "Method"
            args=[]
            kwlist=[]
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
      static const char* const kwlist[] = {};
"""
            code += """
      PythonClassWrapper< %(classname)s >::%(callable)s< %(method_name2)s_name, %(return_type)s %(args)s >
         ( &%(classname)s::%(method_name)s, kwlist);
   }
""" % {'classname': self.get_qualified_name(),
       'method_name': method.name,
       'method_name2': method.name, 'return_type': method.return_type.get_qualified_name(),
       'callable': call_method_name, 'args': (", " if len(args) else "")+ ", ".join(args), 'indent': indent}
            print "################### ARGS are %s"%(", ".join(args))
        for constructor in [c for c in self._constructors if c.scope == "public"]:
            call_method_name = "add"
            if constructor.is_const:
                call_method_name += 'Const'
            call_method_name += "Constructor"
            args = []
            kwlist =[]
            for arg_name, arg_type in constructor.arguments:
                args.append(arg_type.get_qualified_name())
                kwlist.append('"%s"' % arg_name)
            if len(kwlist)> 0:
                code +="""
   {
      static const char* const kwlist[] = {%(kwlist)s};
""" % {'indent': indent, 'kwlist': ", ".join(kwlist), }
            else:
                code +="""
   {
      static const char* const kwlist[] = {};
"""
            code += """
      PythonClassWrapper< %(classname)s>::%(callable)s
      ( kwlist,
        PythonClassWrapper< %(classname)s >::create< %(args)s >);
   }
""" % {'classname': self.get_qualified_name(), 'bare_class_name': self.name,
       'callable': call_method_name, 'args': ", ".join(args), 'indent': indent}

        for child in [c for c in self.children if isinstance(c, Type)]:
            code += """
   PythonClassWrapper<%(full_classname)s>::addClassMember( "%(child_name)s",
      (PyObject*)&PythonClassWrapper< %(child_fullname)s >::Type);
""" % {'full_classname': self.get_qualified_name(), 'classname':self.name, 'child_name': child.name,
       'child_fullname': child.get_qualified_name(),
       'indent': indent}
        print ("#########################BASES ARE %s"%self.bases)
        for base in self.bases:
            code += """
   pyllars%(base_class_name)s::initialize_type();
   PythonClassWrapper<%(full_class_name)s>::addBaseClass
      (&PythonClassWrapper< %(base_class_full_name)s >::Type);
""" % {'full_class_name': self.get_qualified_name(), 'class_name': self.name,
       'base_class_name': base.get_qualified_ns_name(True),
       'base_class_full_name': base.get_qualified_name(),
       'indent': indent}
        code += """
   return status;
}

status_t pyllars%(namespace_name)s::initialize_type(){
    ::initialize_type();
}

static pyllars::Initializer _initializer(pyllars%(namespace_name)s::initialize_type);
""" % {'indent': indent, 'namespace_name': self.get_qualified_ns_name()}
        return header_code, code


class Struct(Class):
    pass


class Referencing(Type):

    def __init__(self, base_type, ext, id_, context, header_filename):
        assert(isinstance(base_type, Type))
        Type.__init__(self, base_type.name+ext, id_, context, base_type.is_incomplete, header_filename)
        self.base_type = base_type
        self.context = base_type.context
        self.ext = ext

    @abstractmethod
    def generate_code(self, path):
        raise Exception("Abstract method")

    def get_qualified_name(self, sanitized=False):
        print "???????????????? TYPE %s %s  == %s" % (self , self.base_type, " ".join(self.qualifiers + [self.base_type.get_qualified_name()+self.ext]))
        if isinstance(self.base_type, FundamentalType):
            return " ".join(self.qualifiers + [self.name])
        return " ".join(self.qualifiers + [self.base_type.get_qualified_name() + self.ext])


class Array(Referencing):

    def __init__(self, base_type, id_, context, header_filename, array_size=None):
        """
        :param base_type:
        :param array_size: None means unbounded
        :return:
        """
        assert(isinstance(base_type, Type))
        Referencing.__init__(self, base_type, "[]", id_, context, header_filename)
        assert(isinstance(base_type, Type))
        self.array_size = array_size
        self.base_type = base_type

    def generate_code(self, path):
        pass


class Pointer(Referencing):

    def __init__(self, base_type, id_, context, header_filename):
        assert(isinstance(base_type, Type))
        Referencing.__init__(self, base_type, "" if isinstance(base_type, FunctionType) else "*", id_, context, header_filename)

    def generate_code(self, path):
        pass


class Reference(Referencing):

    def __init__(self, base_type, id_, context, header_filename):
        Referencing.__init__(self, base_type, "&", id_, context, header_filename)

    def generate_code(self, path):
        pass


class Typedef(Referencing):

    def __init__(self, base_type, id_, context, alias, header_filename):
        Referencing.__init__(self, base_type, "", id_, context, header_filename)
        self.alias = alias
        self.set_qualifiers(base_type.qualifiers)
        self.context = context

    def generate_code(self, path):
        pass

    def get_qualified_name(self, sanitized=False):
        if self.context and self.context.name=="pyllars":
            return self.name
        context = self.base_type.context
        try:
            self.base_type.context = self.context
            name = super(Typedef, self).get_qualified_name()
        finally:
            self.base_type.context = context
        return name



class FundamentalType(Type):

    TYPES=["signed char", "unsigned char", "char", "short int", "short unsigned int", "int", "unsigned int",
           "long int", "long unsigned int", "long long int", "long long unsigned int",
           "__int128", "unsigned __int128",
           "float", "double", "bool",
           "void"]

    def __init__(self, type_, id_, size, alignment):
        if type_ not in FundamentalType.TYPES:
            print "TYPE NOT FUNDAMENTAL: "  + type_
        assert(type_ in FundamentalType.TYPES)
        Type.__init__(self, type_, id_, None, False, None)
        self.type_ = type_
        self.size = size
        self.alignment = alignment
        self.is_incomplete = False
        self.name = type_

    def get_qualified_name(self, sanitized=False):
        return self.name


class CvQualifiedType(Referencing):

    QUALIFIERS = ["const", "volatile"]

    def __init__(self, base_type, id_, context, qualifiers, header_filename):
        assert(len(qualifiers) >= 0)
        for qualifier in qualifiers:
            assert(qualifier in CvQualifiedType.QUALIFIERS)
        Referencing.__init__(self, base_type, "", id_, context,
                             header_filename)
        self.set_qualifiers(qualifiers)

    def is_const(self):
        return 'const' in self.qualifiers

    def generate_code(self, path):
        pass


class Enumeration(Type):

    def generate_code(self, path):
        pass

    def __init__(self, name, id_, context, enumerators, header_filename, base_type=None, is_incomplete=False):
        Type.__init__(self, name, id_, context, is_incomplete, header_filename)
        assert(base_type is None or isinstance(base_type, Type))
        self.base_type = base_type or FundamentalType("int", "internal", 16, 8)
        for value in enumerators or []:
            assert(isinstance(value[1], int))
        self.enum_values = enumerators or []


class Union(Class):

    def generate_code(self, path):
        pass


class FunctionType(Type):

    def __init__(self, id_, return_type, arguments, header_filename, context = None, name=None):
        assert(isinstance(return_type, Type))
        Type.__init__(self, name if name is not None else "func%s"%id_, id_, context, False, header_filename)
        for argument in arguments or []:
            assert(isinstance(argument, tuple))
            assert(len(argument) == 2)
            assert(isinstance(argument[1], Type))
        self.arguments = arguments or []
        for index, argument in enumerate(arguments or []):
            if argument[0] is None:
                self.arguments[index] = ("_%d"%index, arguments[index][1])
        #self.name = "func%s"%id_
        self.return_type = return_type

    def generate_code(self, path):
        pass

#    def get_qualified_name(self):
#        return self.name


class Function(object):

    def ___init__(self, id_, name, return_type, context, arguments = None):
        assert(isinstance(return_type, Type))
        assert(isinstance(context, Namespace) or isinstance(context, Type))
        for argument in arguments or []:
            assert(isinstance(argument, tuple))
            assert(len(argument)==2)
            assert(isinstance(argument[1], Type))
        self.arguments = arguments or []
        for argument, index in enumerate(self.arguments):
            if argument[0] is None:
                self.arguments[index][0] = "_%d" % index
        self.name = name
        self.return_type = return_type
        self.context = context

#TODO: handle operator functions