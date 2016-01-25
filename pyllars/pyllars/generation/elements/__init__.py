class Namespace(object):
    """
    Class to capture namespace properties
    """

    GLOBAL = Namespace("")
    _namespaces = []

    def __init__(self, name, id_, parent=None):
        assert( parent is None or isinstance(parent, Namespace))
        self._name = name.replace("::", "")
        self._id = id_
        self._parent = parent
        self._is_generating = false

    @staticmethod
    def createNamespace(name, id_, parent):
        if parent:
            full_name = parent.full_name() + "::" + name
        else:
            full_name = name
        if full_name not in Namespace._namespaces:
            Namespace._namespaces[full_name] = Namespace(name, id_, parent)
            Namespace._namespaces[id_] = Namespace._namespaces[full_name]
        return Namespace._namespaces[full_name]

    @staticmethod
    def lookup(id_):
        return Namespace._namespaces.get(id_)

    def full_name(self):
        if self._parent:
            return self._parent.full_name + "::" + self._name
        else:
            return self._name

    def generate_code(self):
        if self._generating:
            return
        code = """
    PyObject* %(name)_mod = Py_InitModule3("%(name)s", nullptr,
	        		                 "Module corresponding to C++ namespace %(fullname)s");
""" % {'name': self._name, 'fullname': self.full_name()}
        p = self._parent
        depth = 0
        while p is not None:
            code = """namespace %s{ """%p._name + code
            p = p._parent
            depth += 1
        if self._parent:
            code += """
    PyModule_AddObject( %(parentname)s_mod, "%(name)s", %(name)_mod );
"""%{'name':self._name, 'parentname': self._parent._name}
        #TODO: Generate sub-elements
        for i in range(depth+1):
            code += '}'
        code += "# END %s"%self.full_name()
        yield code


