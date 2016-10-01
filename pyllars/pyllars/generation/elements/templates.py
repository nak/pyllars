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

    def __init__(self, type_, name, params_list, type_lookup):
        self._params_list = {}
        self._type = type_
        self._name = name
        for item in params_list:
            full_name = item.full_name
            name2, params = self.parse(full_name.replace('&lt;', '<').replace('&gt;', '>').replace('&quot;', '"'),
                                       type_lookup)
            if params is None:
                continue
            self._params_list[full_name] = params
            assert(name2 == name)
        self.context = type_.context
        self._full_name = self.context.get_qualified_name() + "::" + self._name

    def get_name(self):
        return self._name

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
    def params_list(self):
        return self._params_list

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
            if depth < 0:
                return None
            index += 1
            if (index == len(text)) and text:
                params.append(text.strip())
        #print "************************ %s" % params
        return params

    @classmethod
    def parse(cls, full_name, type_lookup):
        full_name = full_name.strip()
        index = full_name.find('<', 0)
        name = full_name[:index]
        params = cls.split(full_name[index+1:-1])
        if params is None:
            return None, None

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
        return self.type.get_include_parent_path( os.path.join(path or ".", "templates%s" % sanitize(self.name)))

    def get_header_filename(self, path=None):
        return self.type.get_header_filename( os.path.join(path or ".", "templates%s" % sanitize(self.name)))

    def get_body_filename(self, path=None):
        return self.type.get_body_filename( os.path.join(path or ".", "templates%s" % sanitize(self.name)))

    def get_top_module_name(self):
        return self._type.get_top_module_name()


class ClassTemplateInstantiation(TemplateInstantiation):

    def __init__(self, type_, name,  params_list, type_lookup):
        super(ClassTemplateInstantiation, self).__init__(type_, name, params_list, type_lookup)

    def generate_code(self, path):

        def gen_code(param):
            if isinstance(param, FunctionType):
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
        for param_list in self.params_list.values():
            for param in param_list:
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
        for param_list in self.params_list.values():
            for param in param_list:
                if isinstance(param, Type) and param.get_header_filename(path):
                    code += """
#include "%s"
""" % param.get_header_filename(path)
        code += """
namespace __pyllars_internal{

   typedef const char cstring[];
   constexpr cstring %(short_name)s_name = "%(prefix_name)s";
   constexpr cstring %(short_name)s_short_name = "%(short_name)s";
   static PyObject* parameter_sets[][%(len)s] = {
%(params)s,
      nullptr
   };

   int initialize_template_%(short_name)s(){
""" % { 'short_name':  self.name.split('::')[-1],
        'len': max([len(l) + 2 for l in self._params_list.values()]),
        'prefix_name': self.context.get_qualified_name() + "::" + self.name,
        'params' : ",\n".join(["      {(PyObject*)" +
                               ",\n         (PyObject*)".join([gen_code(param)
                                                               for param in [self.type] + params]) +
                               ", nullptr}"
                        for params in self.params_list.values()])
        }
        all_params = []
        for idx, params in enumerate(self.params_list.values()):
            all_params.append((idx, params))
        init_stmnts = []
        for idx, param_list in all_params:
            for param in [(len(self._params_list), self.type)] + param_list:
                if isinstance(param, Type):
                    init_stmnts.append(("status[%d] |= pyllars" % idx) + param.context.get_qualified_name() + "::" +
                                       sanitize(param.get_name().strip()) + "___ns::initialize_type();" if
                                       param.context else
                                       ("status[%d] |= PythonClassWrapper< %s >::isInitialized()?0:-1;" % (idx, param.full_name))
                                       )
        code += """
        PythonClassTemplate* instantiation = PythonClassTemplate::get<%(short_name)s_name, %(short_name)s_short_name>
            (pyllars%(mod)s::%(mod_name)s_mod);
        if(!instantiation){
            return -1;
        }
        int status[%(len)s +1] = {0};
        %(param_init)s
        for(size_t i = 0; i < %(len)s; ++i ){
            if(status[i] == 0){
                instantiation->addParameterSet(parameter_sets[i]);
            } else {
                printf("Unable to load parameters for template function, this template instantiation will not be available %%s",
                        "%(short_name)s");
            }
        }
        return 0;
   }

   static pyllars::Initializer _initializer_template_%(short_name)s(initialize_template_%(short_name)s);

}

""" % {'name': self.full_name,
       'len': len(self._params_list),
       'short_name': self.name.split('::')[-1],
       'mod': self.context.get_qualified_name(),
       'mod_name': self._type.module_name,
       'param_init': "\n      ".join(init_stmnts),
       }
        header_code = ""
        return header_code, code
