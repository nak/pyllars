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

    def parse_xml(self):
        import xml.etree.ElementTree as et
        tree = et.parse(self.filename)
        root = tree.getroot()
        # construct a lookup table based on id
        for child in root:
            self.element_lookup[child.attrib['id']] = child
        for child in root:
            self.process_xml_element(child)

    def process_xml_element(self, child):
        context = child.attrib.get('context')
        if context is not None and not context in self.processed:
            self.process_xml_element(self.element_lookup[context])
        item = {"Namespace": self.process_namespace,
                "Typedef":self.process_typedef,
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
                "Function": self.process_function,
                }.get(child.tag, self.process_unknown)(child)
        #  for anonymous definitions, traverse the typedefs...
        if item is not None and item.name == "":
            for k, e in self.element_lookup.iteritems():
                if e.attrib.get('type')==child.attrib['id'] and \
                        e.tag == "Typedef":
                    item.name = e.attrib['name']
                    break
        if item is not None:
            self.processed[child.attrib['id']] = item
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

    def process_namespace(self, element):
        assert(self.get_file(element) is not None)
        return elements.Namespace(element.attrib['name'],
                                  element.attrib['id'],
                                  header_filename=self.get_file(element),
                                  context=self.get_context(element))

    def process_typedef(self, element):
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
        return elements.Struct(name=element.attrib['name'], id_=element.attrib['id'],
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
        return elements.FunctionType(id_=element.attrib['id'],
                                     context = self.get_context(parent) if parent is not None else None,
                                     return_type=type_,
                                     header_filename=self.get_file(element),
                                     arguments=arguments,
                                     name=parent.attrib.get('name') or "func%s"%element.attrib['id'],
                                     scope=element.attrib.get('access'))

    def process_function(self, element):
        typeid=element.attrib.get('returns')
        if typeid is None:
            type_=None
        else:
            if typeid not in self.processed:
                self.process_xml_element(self.element_lookup[typeid])
            type_ = self.processed[typeid]
        arguments = []
        for child in [c for c in element if c.tag == 'Argument']:
            arguments.append((child.attrib.get('name'), self.get_type_from(child.attrib['type'])))
        return elements.Function(id_=element.attrib['id'],
                                 name=element.attrib.get('name') or 'func%s'%element.attrib['id'],
                                 return_type=type_,
                                 context=self.get_context(element),
                                 scope=element.attrib.get('access'),
                                 arguments=arguments,
                                 header_filename=self.get_file(element),
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
        parent = self.get_type_from(element.attrib['context'])
        parent.add_member(member_name=element.attrib['name'],
                          type_=self.get_type_from(element.attrib['type']),
                          member_scope=element.attrib.get('access') or 'private',
                          qualifiers=qualifiers,
                          bit_field_size=element.attrib.get('bits'))

    def process_variable(self, element):
        context = self.get_type_from(element.attrib['context'])
        if not isinstance(context, elements.Namespace):
            self.process_field(element, ["static"])

    def process_method(self, element):
        parent = self.get_type_from(element.attrib['context'])
        qualifiers = []
        if element.attrib.get('const') == '1':
            qualifiers.append('const')
        if element.attrib.get('static'):
            qualifiers.append('static')
        arguments = []
        for child in [c for c in element if c.tag == 'Argument']:
            arguments.append( (child.attrib.get('name'), self.get_type_from(child.attrib['type'])))
        return_type = self.get_type_from(element.attrib.get('returns')) if element.attrib.get('returns') else None
        parent.add_method(method_name=element.attrib['name'],
                          method_scope=element.attrib.get('access') or 'private',
                          qualifiers=qualifiers,
                          return_type=return_type,
                          method_parameters=arguments)

    def process_constructor(self, element):
        parent = self.get_type_from(element.attrib['context'])
        qualifiers = []
        if element.attrib.get('const') == '1':
            qualifiers.append('const')
        arguments = []
        for child in [c for c in element if c.tag == 'Argument']:
            arguments.append( (child.attrib.get('name'), self.get_type_from(child.attrib['type'])))
        parent.add_constructor( method_scope=element.attrib.get('access') or 'private',
                                qualifiers=qualifiers,
                                method_parameters=arguments)

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
        bases_string = element.attrib.get('bases')
        return self.parse_types_from(bases_string)

    def parse_types_from(self, bases_string):
        if bases_string is None: return None
        base_ids = bases_string.split(' ')
        bases = []
        for baseid in base_ids:
            if baseid not in self.processed:
                self.process_xml_element(self.element_lookup[baseid])
            bases.append(self.processed[baseid])
        return bases

    def get_context(self, element):
        context_id = element.attrib.get('context')
        if context_id:
            context = self.processed[context_id]
        else:
            context = None
        return context

    def get_base_type(self, element):
        typeid = element.attrib['type']
        return self.get_type_from(typeid)

    def get_type_from(self, typeid):
        if typeid not in self.processed:
            base_type = self.process_xml_element( self.element_lookup[typeid] )
        else:
            if typeid not in self.processed:
                self.process_xml_element(self.element_lookup[typeid])
            base_type = self.processed[typeid]
        return base_type
