from ..elements import Type, FundamentalType, FunctionType
import os.path
import hashlib


def sanitize(name):
    if name == "::":
        return "pyllars"
    return  name.replace("<", "__").replace(">", "__").replace(",", "___").replace(" ", "_"). \
        replace('::::','::').replace("::", "____").replace('&','_and_').replace("*","_star_").replace('c++', 'cxx'). \
        replace('+', 'X').replace('-', '_').replace('&','_and_').replace('*','_start_')


class TemplateInstantiation(object):

    def __init__(self, type, full_name, type_lookup):
        self._name, self._params = self.parse(full_name.replace('&lt;', '<').replace('&gt;', '>').replace('&quot;', '"'),
                                              type_lookup)
        self._full_name = type.context.get_qualified_name() + "::" + full_name
        self._type = type
        self.context = type.context
        self.sanitized_name = sanitize(self._full_name)

    @property
    def full_name(self):
        return self._full_name

    @property
    def name(self):
        return self._name

    @property
    def type(self):
        return self._type

    @property
    def params(self):
        return self._params

    @classmethod
    def split(cls, text):
        index = 0
        depth = 0
        params = []
        while index < len(text):
            if text[index] == '<':
                depth += 1
            elif text[index] == '>':
                depth -=1 # note this algo assumes valid format and matching <>
            elif text[index] == ',' and depth == 0:
                params.append(text[:index].strip())
                text = text[index+1:]
                index = 0
            assert(depth >= 0)
            index += 1
            if (index == len(text)) and text:
                params.append(text.strip())
        print "************************ %s" % params
        return params

    @classmethod
    def parse(cls, full_name, type_lookup):
        full_name = full_name.strip()
        index = full_name.find('<', 0)
        name = full_name[:index]
        params = cls.split(full_name[index+1:-1])
        def get_param_value(param):
            if param[0] in ['0', '1', '2', '3', '4', '5', '6', '7', '8', '9']:
                if '.' in param:
                    return float(param)
                else:
                    return int(param)
            elif param.startswith('"'):
                assert(param.endwith('"'))
                return param[1:-2]
            elif param in ['char', 'unsigned char', 'signed char',
                           'short', 'unsigned short', 'signed short',
                           'int', 'unsigned', 'unsigned int', 'signed', 'signed int',
                           'long', 'unsigned long', 'signed long',
                           'long long', 'unsigned long long', 'signed long long',
                            ]:
                return (long, param)
            elif param == 'true':
                return 'PyBool_True'
            elif param == 'false':
                return 'PyBool_False'
            elif param in ['float', 'double', 'long double']:
                return "PyTuple_Pack(2, PyFloat_Type, %s)" % param
            else:
                param = "::" + param.strip()
                if param not in type_lookup:
                    #alternatives = [t for k,t in type_lookup.items() if k.startswith(param[:-1] +',')]
                    #if len(alternatives) > 0:
                    #    return alternatives[0]
                    from . import Class
                    return Class(name=param, id_="-1", header_filename=None, is_incomplete=True, is_absrtact=False)
                return type_lookup[param]
        return name, [item for item in [get_param_value(param) for param in params] ]

    def get_include_parent_path(self, path=None):
        return self.type.get_include_parent_path( os.path.join(path or ".", "templates%s" % self.type.id_))

    def get_header_filename(self, path=None):
        return self.type.get_header_filename( os.path.join(path or ".", "templates%s" % self.type.id_))

    def get_body_filename(self, path=None):
        return self.type.get_body_filename( os.path.join(path or ".", "templates%s" % self.type.id_))

    def get_top_module_name(self):
        return self.type.get_top_module_name()


class ClassTemplateInstantiation(TemplateInstantiation):

    def __init__(self, type, full_name, type_lookup):
        super(ClassTemplateInstantiation, self).__init__(type, full_name, type_lookup)

    def generate_code(self, path):
        def gen_code(param):
            if  isinstance(param, FunctionType):
                return "&PythonFunctionWrapper2<%s>::Type" % param.get_qualified_name()
            elif isinstance(param,Type) or isinstance(param, FundamentalType):
                return "&PythonClassWrapper<%s>::Type" % param.full_name
            elif isinstance(param, int):
                return "PyInt_FromLong((long) %s)" % param
            elif isinstance(param, float):
                return "PyFloat_FromDouble((double)%s)" % param
            elif isinstance(param, basestring):
                return "PyString_FromString(\"%s\")" % param
            elif isinstance(param, tuple):
                if param[0] == long:
                    return "PyTuple_Pack(2, &PyLong_Type, PyString_FromString(\"%s\"))" % param[1]
                else:
                    return "PyTuple_Pack(2, &PyFloat_Type, PyString_FromString(\"%s\"))" % param[1]
            else:
                raise Exception("Unknown template parameter type %s" % type(param))

        code = ""
        for param in self.params:
            if isinstance(param, Type):
                if param.get_header_filename(path):
                    code += "#include \"%s\"\n" % param.get_header_filename(path)
        code += """
#include <pyllars_templates.hpp>
#include "%s"
""" % self.get_header_filename(path)
        code += """
#include "%s"
""" % self.type.get_header_filename(path)
        code += """
namespace __pyllars_internal{

   typedef const char cstring[];
   constexpr cstring %(short_name)s_name = "%(prefix_name)s";
   constexpr cstring %(short_name)s_short_name = "%(short_name)s";
   static PyObject* parameter_set[] = {(PyObject*)%(params)s, nullptr };

   int initialize_template_%(short_name)s_%(id)s(){
        PythonClassTemplate* instantiation = PythonClassTemplate::get<%(short_name)s_name, %(short_name)s_short_name>
            (pyllars%(mod)s::%(mod_name)s_mod);
        if(!instantiation){
            return -1;
        }
        int status =0;
        %(param_init)s
        if(status == 0){
            instantiation->addParameterSet(parameter_set);
        }
        return status;
   }

   static pyllars::Initializer _initializer_template_%(short_name)s_%(id)s(initialize_template_%(short_name)s_%(id)s);

}

""" % {'name': self.full_name,
       'prefix_name': self.full_name.split('<')[0],
       'id': self.type.id_,
       'short_name': self.name,
       'mod': self.type.context.get_qualified_name(),
       'mod_name': self.type.module_name,
       'param_init': "\n      ".join(["status |= pyllars" + param.context.get_qualified_name() + "::" + sanitize(param.get_name().strip()) +"___ns::initialize_type();" if
                                      param.context else
                                      "status |= PythonClassWrapper< %s >::isInitialized()?0:-1;" % param.full_name
                                      for param in [self.type] + self.params if isinstance(param, Type)]),
       'params': ",\n         (PyObject*)".join([gen_code(param) for param in [self.type] + self.params])}
        header_code = ""
        return header_code, code