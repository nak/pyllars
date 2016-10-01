#!/usr/bin/env python
"""
Created on Jun 23, 2015

@author: jrusnak
"""
import os.path
from parser import CPPParser

class ResultsProcessor(object):
    """
    Process results from file produced under XSLT transformation on 
    C++ XML description
    """

    def __init__(self, infile):
        """
        Constructor
        """
        self._file = infile
        self._python_code = []
        
    @staticmethod
    def process(path, class_filters):
        """
        Process self's file to generate python code to provided path
        :param class_filters: class names to be excluded from compilation
        :param: path: file path to process
        """
        parser = CPPParser(path)
        for class_filter in class_filters:
            parser.filter_class(class_filter)
        parser.parse_xml()
        return parser.processed, parser.template_instantiations, parser.type_lookup


def process(castxml_file, build_dir, class_filters = None):
    from pyllars.generation.elements import Function, Namespace, BaseElement, Struct, Class
    from pyllars.generation.elements.templates import ClassTemplateInstantiation
    items, template_instantiations, type_lookup = ResultsProcessor.process(castxml_file, class_filters)
    compilables = []

    def process_item(item):
        #if isinstance(item, Namespace) and not item.children:
        #    raise Exception("Empty item %s" % item.get_name())
        if item.get_body_filename() is None:
            "No body for item %s" % item.get_name()
            return
        if item.get_body_filename().split('/')[-1]==".cpp": raise Exception(item.name +">>" + str(os.path.basename(item.get_header_filename() or '.hpp')=='.hpp'))
        code = item.generate_code(".")
        if code is not None and item.name != "":
            header, body = code
            if not os.path.exists(os.path.join(build_dir,item.get_include_parent_path().replace(" ","_").replace("<", "__").replace(">", "__").replace("::", "____").replace(", ", "__"))):
                os.makedirs(os.path.join(build_dir,item.get_include_parent_path().replace(" ","_").replace("<","__").replace(">","__").replace("::", "____").replace(", ","__")))
            with open(os.path.join(build_dir, item.get_header_filename().replace("<", "__").replace(" ","_").replace(">","__").replace("::", "____").replace(", ", "__")), 'w') as f:
                f.write(header)
            with open(os.path.join(build_dir,item.get_body_filename().replace("<", "__").replace(" ","_").replace(">", "__").replace("::","____").replace(", ", "__")), 'w') as f:
                f.write(body)
                top_module, is_pseudo_ns = item.get_top_module_name()
                compilables.append((top_module.split('.')[0], f.name, is_pseudo_ns))

    for item in [i for i in items.itervalues() if i.get_body_filename() is not None and
                    not(isinstance(i,Function)) and os.path.basename((i.get_header_filename() or '.hpp'))!='.hpp']:
        process_item(item)
    # Also process any newly added pseudo namespace modules
    for item in [n for n in Namespace.namespaces.values() if n.id_ == BaseElement.PSEUDO_ID]:
        process_item(item)
    grouped ={}
    class_types = {}
    for item in [t for t in template_instantiations if t is not None and (isinstance(t, Struct) or isinstance(t, Class))]:
        if item.get_name():
            base_name = item.full_name.split('<')[0]
            grouped.setdefault(base_name, []).append(item)
            class_types[base_name] = item
    for k, item in grouped.items():
        class_type = class_types[k]
        if class_type is None:
            continue
        tmpl = ClassTemplateInstantiation(class_type, k, item, type_lookup)
        try:
            process_item(tmpl)
        except:
            import traceback
            traceback.print_exc()
            print "!!Unable to process template %s" % k
            raise
    return [c for c in compilables if c[0]!="<builtin>" and c[0] != "__builtin__"]

if __name__ == "__main__":
    import sys
    process(sys.argv[1], "./build")
