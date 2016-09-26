'''
Created on Jun 23, 2015

@author: jrusnak
'''
import os.path

from pyllars.generation import elements


class CPPParser(object):
    """
    Class to process a CPP XML definition into a pre-Python
    results file.  @pyllars.generation.ResultProcessor can 
    be invoked on the resulting file to produce the final Python
    """

    def __init__(self, path):
        '''
        Constructor
        '''
        assert(os.path.exists(path))
        self.filename = path
        self.root_item = None
        self.processed = {}
        self.element_lookup = {}
        self.element_name_lookup = {}
        self.class_filter = []
        self.type_lookup = {}
        self.template_instantiations = []

    def filter_class(self, classname):
        self.class_filter.append(classname)

    def parse_xml(self):
        import xml.etree.ElementTree as et
        tree = et.parse(self.filename)
        root = tree.getroot()
        # construct a lookup table based on id
        for child in root:
            self.element_lookup[child.attrib['id']] = child
            if child.attrib.get('name') is not None:
                self.element_name_lookup.get(child.attrib.get('name'),[]).append(child)
        for child in [c for c in root if c.tag=="File"]:
            self.process_xml_element(child)
        for child in [c for c in root if c.tag!="File"]:
            self.process_xml_element(child)

    def process_xml_element(self, child):
        context = child.attrib.get('context')
        if context is not None and not context in self.processed:
            self.process_xml_element(self.element_lookup[context])
        try:
            item = {"Namespace": self.process_namespace,
                "Typedef":self.process_typedef,
                "TypeAlias": self.process_typedef,
                "FundamentalType": self.process_fundamental_type,
                "ArrayType": self.process_array_type,
                "Struct": self.process_struct,
                "Class": self.process_class,
                "PointerType": self.process_pointer_type,
                "ReferenceType": self.process_reference_type,
                "FunctionType": self.process_function_type,
                "CvQualifiedType": self.process_cvqualified_type,
                "Enumeration": self.process_enumeration_type,
                "Union": self.process_union_type,
                "Field": self.process_field,
                "Variable": self.process_variable,
                "Method": self.process_method,
                "Constructor": self.process_constructor,
                "Destructor" : self.do_nothing,
                "Function": self.process_function,
                "OperatorMethod": self.process_operator_method,
                "Unimplemented": self.process_unimplemented,
                "File": self.process_file,

                }.get(child.tag, self.process_unknown)(child)
        except:
            print "!!!!!!!! ERROR IN ELEMENT %s" %child.attrib.get('id')
            raise
        #  for anonymous definitions, traverse the typedefs...
        if item is not None and item.name == "":
            for k, e in self.element_lookup.iteritems():
                if e.attrib.get('type')==child.attrib['id'] and \
                        e.tag == "Typedef":
                    item.name = e.attrib['name']
                    break
        if item is not None:
            self.processed[child.attrib['id']] = item
            qual_name = item.get_qualified_name().replace("&lt;", "<").replace("&gt;", ">").replace("&quot;", '"').strip()
            self.type_lookup[qual_name] = item
            if "<" in item.get_qualified_name() and item.get_qualified_name().replace('*', '').replace('&', '').strip().endswith('>'):
               self.template_instantiations.append(item)
        if context is None and child.attrib['id'] == "_1":
            # top level
            assert(self.root_item is None) # only one top level!
            self.root_item = item
        elif context is not None and item is not None:
            if context not in self.processed:
                self.process_xml_element(self.element_lookup[context])
            if context in self.processed:
                self.processed[context].children.append(item)
        return item

    def do_nothing(self, *args):
        pass

    def process_alias(self, alias, name, context=None):
        if self.processed.get(alias):
            return self.processed[alias]
        aliased_elem = None
        if '::' in name:
            names = name.split('::')
            try:
                ids = self.element_name_lookup[names[0]].attib.get('id')
                foundid=None
                for id_ in ids:
                    for namespace in names[1:-1]:
                        ns = [e for e in self.element_name_lookup[namespace] if e.attrib.get('context')==id_]
                        if len(ns)!=1:
                            break
                        id_ = ns[0].attrib('id')
                        foundid = id_
                if foundid is not None:
                    aliased_elem = [e for e in self.element_name_lookup.get(names[-1]) or [] if e.attrib.get('context') == foundid]
            except:
                pass
        if aliased_elem is None:
            aliased_elem = self.element_name_lookup.get(name)
        if aliased_elem is not None:
            alias_obj = self.process_xml_element(aliased_elem)
            self.processed[alias] = elements.get_alias(alias, alias_obj)
        if self.processed.get(alias) is None and context is not None:
            self.processed[alias] = elements.TypeAlias()
        return self.processed.get(alias)

    def process_file(self, element):
        filename = element.attrib.get('name')
        if filename is not None and os.path.exists(filename):
            with open(filename,'r') as file_:
                for linefull in file_:
                    for line in linefull.split(';'):
                        if (not line.strip().startswith('/') and not line.strip().startswith('*')) and \
                                line.strip().startswith('using') and '=' in line:
                            elements=line.replace('using','').replace('\t',' ').split('=')
                            if len(elements) is 2:
                                self.process_alias(elements[0].strip(), elements[1].strip(), element.get('context'))

    def process_unimplemented(self, element):
        #if element.attrib.get('type_class') == "Decltype":
        #    return elements.FundamentalType("void",
        #                                    element.attrib['id'], element.attrib.get('size'),
        #                                    element.attrib.get('align'))
        print("UNIMPLEMENTED IN CAST XML: %s" % (element.attrib.get("kind") or "not specified"))
        return None # elements.TypeAlias(element.attrib.get('id'))

    def process_namespace(self, element):
        if self.get_file(element) is None:
            return None
        return elements.Namespace(element.attrib['name'],
                                  element.attrib['id'],
                                  header_filename=self.get_file(element),
                                  context=self.get_context(element))

    def process_typedef(self, element):
        if self.get_base_type(element) is None:
            return None
        return elements.Typedef(base_type=self.get_base_type(element),
                                id_=element.attrib['id'],
                                header_filename=self.get_file(element),
                                context=self.get_context(element),
                                alias=element.attrib.get('name'),
                                scope=element.attrib.get('access'))

    def process_fundamental_type(self, element):
        return elements.FundamentalType(element.attrib['name'], element.attrib['id'], element.attrib['size'],element.attrib['align'])

    def process_enumeration_type(self, element):
        enum_values = []
        for child in [c for c in element if c.tag=="EnumValue"]:
            enum_values.append((child.attrib['name'], int(child.attrib['init'])))
        return elements.Enumeration(name=element.attrib.get('name'),
                                    id_=element.attrib.get('id'),
                                    context=self.get_context(element),
                                    header_filename=self.get_file(element),
                                    enumerators=enum_values,
                                    scope=element.attrib.get('access'))

    def process_array_type(self, element):
        size = element.attrib.get('max')
        if size == '':
            size = None
        elif size is not None:
            size = int(size) + 1
        return elements.Array(base_type=self.get_base_type(element),
                              id_=element.attrib['id'],
                              context=self.get_context(element),
                              header_filename=self.get_file(element),
                              array_size=size,
                                scope=element.attrib.get('access'))

    def process_struct(self, element):
        # workaround for bug in castxml or internal gnu inconsistency which should not instantiate these:
        name = element.attrib['name']
        if name in ["__numeric_traits_integer<float>", "__numeric_traits_integer<double>", "__numeric_traits_integer<long double>"] :
            return None
        name = name.replace("__numeric_traits_integer<float>", "__numeric_traits_floating<float>")
        name = name.replace("__numeric_traits_integer<double>", "__numeric_traits_floating<double>")
        name = name.replace("__numeric_traits_integer<long double>", "__numeric_traits_floating<long double>")
        return elements.Struct(name=name,
                               id_=element.attrib['id'],
                               header_filename=self.get_file(element),
                               is_incomplete=element.attrib.get('incomplete')!='1',
                               is_absrtact=element.attrib.get('abstract')=='1',
                               inherited_from=self.get_bases(element),
                               context=self.get_context(element),
                               scope=element.attrib.get('access'))

    def process_pointer_type(self, element):
        return elements.Pointer(base_type=self.get_base_type(element),
                                id_=element.attrib['id'],
                                header_filename=self.get_file(element),
                                context=self.get_context(element),
                                scope=element.attrib.get('access'))

    def process_reference_type(self, element):
        if self.get_base_type(element) is None:
            return None
        return elements.Reference(base_type=self.get_base_type(element),
                                  id_=element.attrib['id'],
                                  header_filename=self.get_file(element),
                                  context=self.get_context(element),
                                  scope=element.attrib.get('access'))

    def process_class(self, element):
        return elements.Class(name=element.attrib['name'], id_=element.attrib['id'],
                              header_filename=self.get_file(element),
                              is_incomplete=element.attrib.get('incomplete')!='1',
                              is_absrtact=element.attrib.get('abstract')=='1',
                              inherited_from=self.get_bases(element),
                              context=self.get_context(element),
                              scope=element.attrib.get('access'))

    def process_cvqualified_type(self, element):
        if self.get_base_type(element) is None:
            return None
        qualifiers=[]
        if element.attrib.get('const'):
            qualifiers.append('const')
        if element.attrib.get('volatile'):
            qualifiers.append('volatile')
        return elements.CvQualifiedType(base_type=self.get_base_type(element),
                                        id_=element.attrib['id'],
                                        context=self.get_context(element),
                                        header_filename=self.get_file(element),
                                        qualifiers=qualifiers,
                                        scope=element.attrib.get('access'))

    def process_function_type(self, element):
        has_varargs = 'Ellipsis' in [c.tag for c in element]
        typeid=element.attrib.get('returns')
        if typeid is None:
            type_=None
        else:
            if typeid not in self.processed:
                self.process_xml_element(self.element_lookup[typeid])
            type_ = self.processed[typeid]
        arguments = []
        for child in [c for c in element if c.tag == 'Argument']:
            arguments.append((child.attrib.get('name'), self.get_base_type(child)))
        id_ = element.attrib['id']
        parent = None
        for elem in self.element_lookup.itervalues():
            if elem.attrib.get('type') == id_:
                parent = elem
                break
        while parent is not None and parent.attrib.get('name') is None:
            id_ = parent.attrib['id']
            for elem in self.element_lookup.itervalues():
                if elem.attrib.get('type') == id_:
                    parent = elem
                    break
            break
        qualifiers = []
        if element.attrib.get('const') == '1':
            qualifiers.append('const')
        return elements.FunctionType(id_=element.attrib['id'],
                                     context = self.get_context(parent) if parent is not None else None,
                                     return_type=type_,
                                     header_filename=self.get_file(element),
                                     arguments=arguments,
                                     name=element.attrib.get('name'),
                                     qualifiers=qualifiers,
                                     has_varargs=has_varargs,
                                     scope=element.attrib.get('access'))

    def process_function(self, element):
        has_varargs = 'Ellipsis' in [c.tag for c in element]
        typeid=element.attrib.get('returns')
        if typeid is None:
            type_=None
        else:
            if typeid not in self.processed:
                self.process_xml_element(self.element_lookup[typeid])
            type_ = self.processed[typeid]
        arguments = []
        for child in [c for c in element if c.tag == 'Argument']:
            arguments.append((child.attrib.get('name'), self.get_type_from(child.attrib['type'], child.attrib.get('id'))))
        if not all(a[1] is not None for a in arguments):
            return None # have a type of argument using an unimplemented feature in castxml
        return elements.Function(id_=element.attrib['id'],
                                 name=element.attrib.get('name'),
                                 return_type=type_,
                                 context=self.get_context(element),
                                 scope=element.attrib.get('access'),
                                 arguments=arguments,
                                 header_filename=self.get_file(element),
                                 has_varargs=has_varargs
                                )

    def process_union_type(self, element):
        return elements.Union(name=element.attrib.get('name') or "",
                              id_=element.attrib['id'],
                              header_filename=self.get_file(element),
                              is_incomplete = element.attrib.get('incomplete')!='1',
                              is_absrtact=False,
                              inherited_from=self.get_bases(element),
                              context=self.get_context(element),
                              scope=element.attrib.get('access'))

    def process_unknown(self, element):
        print "UNKNOWN ELEMENT: " + element.tag

    def process_field(self, element, qualifiers=None):
        parent = self.get_type_from(element.attrib['context'], element.attrib['id'])
        if self.get_type_from(element.attrib['type'], element.attrib['id']) is None:
            return None
        parent.add_member(member_name=element.attrib['name'],
                          id_=element.attrib['id'],
                          type_=self.get_type_from(element.attrib['type'], element.attrib.get('id')),
                          member_scope=element.attrib.get('access') or 'private',
                          qualifiers=qualifiers,
                          bit_field_size=element.attrib.get('bits'))

    def process_variable(self, element):
        context = self.get_type_from(element.attrib['context'], element.attrib['id'])
        if not isinstance(context, elements.Namespace):
            self.process_field(element, ["static"])

    def process_method(self, element):
        parent = self.get_type_from(element.attrib['context'], element.attrib['id'])
        if parent.name in self.class_filter:
            return None
        qualifiers = []
        if element.attrib.get('const') == '1':
            qualifiers.append('const')
        if element.attrib.get('static'):
            qualifiers.append('static')
        arguments = []
        for child in [c for c in element if c.tag == 'Argument']:
            if self.get_type_from(child.attrib['type'], child.attrib.get('id')) is None:
                print "Unable to determine type for argument in method %s" % element.attrib.get('name')
                return None
            arguments.append((child.attrib.get('name'), self.get_type_from(child.attrib['type'], child.attrib.get('id'))))
        has_varags = 'Ellipsis' in [c.tag for c in element]

        return_id = element.attrib.get('returns')
        return_type = self.get_type_from(return_id, element.attrib.get('id')) if return_id else None
        parent.add_method(method_name=element.attrib['name'],
                          alt_name=element.attrib.get('namePy'),
                          method_scope=element.attrib.get('access') or 'private',
                          qualifiers=qualifiers,
                          return_type=return_type,
                          method_parameters=arguments,
                          with_ellipsis=has_varags)

    def process_constructor(self, element):
        if 'Ellipsis' in [c.tag for c in element]:
            return None
        parent = self.get_type_from(element.attrib['context'], element.attrib['id'])
        qualifiers = []
        if element.attrib.get('const') == '1':
            qualifiers.append('const')
        arguments = []
        for child in [c for c in element if c.tag == 'Argument']:
            if self.get_type_from(child.attrib['type'], child.attrib.get('id')) is None:
                print "Unable to determine type for argument in method %s" % element.attrib.get('name')
                return None
            arguments.append( (child.attrib.get('name'), self.get_type_from(child.attrib['type'], child.attrib.get('id'))))
        parent.add_constructor( method_scope=element.attrib.get('access') or 'private',
                                qualifiers=qualifiers,
                                method_parameters=arguments)

    def process_operator_method(self, element):
        if 'Ellipsis' in [c.tag for c in element]:
            return None
        if element.attrib.get('name')=='[]':
            parent = self.get_type_from(element.attrib['context'], element.attrib['id'])
            return_type = self.get_type_from(element.attrib.get('returns'), element.attrib['id']) if element.attrib.get('returns') else None
            arguments = []
            qualifiers = []
            if element.attrib.get('const') == '1':
                qualifiers.append('const')
            for child in [c for c in element if c.tag == 'Argument']:
                if self.get_type_from(child.attrib['type'], child.attrib.get('id')) is None:
                    print "Unable to determine type for argument in method %s" % element.attrib.get('name')
                    return None
                arguments.append((child.attrib.get('name'), self.get_type_from(child.attrib['type'], child.attrib.get('id'))))
            assert(len(arguments)==1)
            parent.add_operator_mapping(method_scope=element.attrib.get('access') or 'private',
                                        qualifiers=qualifiers,
                                        return_type=return_type,
                                        method_parameter=arguments[0])
        elif element.attrib.get('name') == "+":
            element.attrib['namePy'] = "__add__"  # rename to Python equivalent
            self.process_method(element)
        elif element.attrib.get('name') == "-":
            element.attrib['namePy'] = "__sub__"  # rename to Python equivalent
            self.process_method(element)
        elif element.attrib.get('name') == "*":
            element.attrib['namePy'] = "__mul__"  # rename to Python equivalent
            self.process_method(element)
        elif element.attrib.get('name') == "/":
            element.attrib['namePy'] = "__div__"  # rename to Python equivalent
            self.process_method(element)
        elif element.attrib.get('name') == "%":
            element.attrib['namePy'] = "__mod__"
            self.process_method(element)
        elif element.attrib.get('name') == "&":
            element.attrib['namePy'] = "__and__"
            self.process_method(element)
        elif element.attrib.get('name') == "|":
            element.attrib['namePy'] = "__or__"
            self.process_method(element)
        elif element.attrib.get('name') == "^":
            element.attrib['namePy'] = "__xor__"
            self.process_method(element)
        elif element.attrib.get('name') == ">>":
            element.attrib['namePy'] = "__rshift__"
            self.process_method(element)
        elif element.attrib.get('name') == "<<":
            element.attrib['namePy'] = "__lshift__"
            self.process_method(element)
        elif element.attrib.get('name') == "++":
            element.attrib['namePy'] = "__iadd__"  # rename to Python equivalent
            self.process_method(element)
        elif element.attrib.get('name') == "-=":
            element.attrib['namePy'] = "__isub__"  # rename to Python equivalent
            self.process_method(element)
        elif element.attrib.get('name') == "*=":
            element.attrib['namePy'] = "__imul__"  # rename to Python equivalent
            self.process_method(element)
        elif element.attrib.get('name') == "%=":
            element.attrib['namePy'] = "__imod__"  # rename to Python equivalent
            self.process_method(element)
        elif element.attrib.get('name') == ">>=":
            element.attrib['namePy'] = "__irshift__"
            self.process_method(element)
        elif element.attrib.get('name') == "<<=":
            element.attrib['namePy'] = "__ilshift__"
            self.process_method(element)
        elif element.attrib.get('name') == "&=":
            element.attrib['namePy'] = "__iand__"
            self.process_method(element)
        elif element.attrib.get('name') == "|=":
            element.attrib['namePy'] = "__ior__"
            self.process_method(element)
        elif element.attrib.get('name') == "^=":
            element.attrib['namePy'] = "__ixor__"
            self.process_method(element)
        elif element.attrib.get('name') == "~":
            element.attrib['namePy'] = "__inv__"
            self.process_method(element)
        elif element.attrib.get('name') == "=" and element.attrib.get('artificial')!='1':
            parent = self.get_type_from(element.attrib['context'], element.attrib['id'])
            return_type = self.get_type_from(element.attrib.get('returns'), element.attrib['id']) if element.attrib.get('returns') else None
            arguments = []
            qualifiers = []
            if element.attrib.get('const') == '1':
                qualifiers.append('const')
            for child in [c for c in element if c.tag == 'Argument']:
                if self.get_type_from(child.attrib['type'], child.attrib.get('id')) is None:
                    print "Unable to determine type for argument in method %s" % element.attrib.get('name')
                    return None
                arguments.append((child.attrib.get('name'), self.get_type_from(child.attrib['type'], child.attrib.get('id'))))
            assert(len(arguments)==1)
            parent.add_assigner(method_scope=element.attrib.get('access') or 'private',
                                qualifiers=qualifiers,
                                return_type=return_type,
                                method_parameter=arguments[0])

    def get_file(self, element):
        fileid = element.attrib.get('file')
        if fileid is None:
            children = element.attrib.get('members')
            if children is None: return None
            for member in children.split(' '):
                fileid = self.element_lookup[member].attrib.get('file')
                if fileid is not None:
                    header = self.element_lookup[fileid].attrib.get('name')
                    if header is not None: return header
        file_element = self.element_lookup[fileid]
        return file_element.attrib['name']

    def get_bases(self, element):
        return [self.get_type_from(base.attrib.get('type')) for base in [c for c in element if c.tag == "Base"] if base.attrib.get('type')]

    def get_context(self, element):
        context_id = element.attrib.get('context')
        if context_id:
            context = self.processed[context_id]
        else:
            context = None
        return context

    def get_base_type(self, element):
        typeid = element.attrib['type']
        return self.get_type_from(typeid,  element.attrib.get('id'))

    def get_type_from(self, typeid, parentid=None):
        if typeid == '_0':
            return None
        elif typeid not in self.processed and self.element_lookup.get(typeid) is not None:
            base_type = self.process_xml_element( self.element_lookup[typeid] )
        elif self.element_lookup.get(typeid) is not None:
            base_type = self.processed[typeid]
        else:
            raise Exception("?? %s" % self.element_lookup.get(typeid))
        return base_type
