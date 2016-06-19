#!/usr/bin/env python
'''
Created on Jun 23, 2015

@author: jrusnak
'''
import os.path
from parser import CPPParser

class ResultsProcessor(object):
    '''
    Process results from file produced under XSLT transformation on 
    C++ XML description
    '''


    def __init__(self, infile):
        '''
        Constructor
        '''
        self._file = infile
        self._python_code = []
        
    @staticmethod
    def process(path):
        '''
        Process self's file to generate python code to provided path
        '''
        parser = CPPParser(path)
        parser.parse_xml()
        return parser.processed

    def processold(self, to_path):
        imports = {}
        self._curr_package = "globals"
        def add_import( package, element):
            if package not in imports:
                imports[package] = []
            if element not in imports[package]:
                imports[package].append( element)
        codetext = ""
        typedefs={}
        functypedefs={}
        functiontypes=[]
        def add_typedef( name, basictype):
            typedefs[name] = basictype
        def add_functiontype(typedef):
            functiontypes.append(typedef)
        def add_functypedef( name, basictype):
            functypedefs[name] = basictype
        def output_code(package, typedefs, functypedefs, functiontypes):
            if package.startswith('.'): package = package[1:]
            packagedir = os.path.join(to_path, package.replace(".","/"))
            if not os.path.exists(packagedir):
                os.makedirs(packagedir)
            initfile = os.path.join(packagedir,"__init__.py")
            print("Writing to %s"%initfile)
            code = self._python_code
            with open(initfile,'w') as outfile:
                count = 0
                for line in code:
                    if not line.startswith("#!!"):
                        outfile.write(line +"\n" )
                        count += 1
                    else:
                        count +=1
                        break
                code= code[count:]
                for package,items in imports.items():
                    if not package or package=="::": package = "globals"
                    if items:
                        outfile.write("from %s import %s\n"%( package, ", ".join(items)))
                outfile.write("\n")
                if typedefs:
                    outfile.write("#TYPEDEFS\n")
                    for typename, basictype in typedefs.items():
                        outfile.write("%s = %s\n"%( typename, basictype))
                    outfile.write("\n")

                if functiontypes:
                    for func in functiontypes:
                        outfile.write(func+"\n")
                if functypedefs:
                    self._python_code.append("#FUNCTION TYPEDEFS")
                    for typename, basictype in functypedefs.items():
                        outfile.write("%s = %s\n"%( typename, basictype))

                
                for line in code:
                    outfile.write(line +"\n" )
            typedefs={}
            functypedefs={}
            functiontypes=[]
            self._python_code = []
            
        for line in self._file:
            line = line.replace('\n','').replace('\r','')
            if line.startswith("===") and line.replace('=',''):
                new_package = line.replace('=','')
                if new_package == "::":
                    new_package = "globals"
                if new_package != self._curr_package:
                    output_code(self._curr_package, typedefs, functypedefs, functiontypes)
                    self._curr_package = new_package
                if codetext:
                    eval(codetext)
                    codetext = ""
            elif line.startswith("#@-"):
                if codetext:
                    eval(codetext)
                codetext = line.replace("#@-","")
            elif line.startswith("#-"):
                codetext += line.replace("#-",'')
            elif line.startswith("#@"):
                eval(line.replace("#@",""))
                if codetext:
                    eval(codetext)
                    codetext = ""
            else:
                self._python_code.append(line)
                if codetext:
                    eval(codetext)
                    codetext = ""

if __name__ == "__main__":
    import sys
    items = ResultsProcessor.process(sys.argv[1])
    for item in items.itervalues():
        code = item.generate_code(".")
        if code is not None and item.name != "":
            print "ITEM %s:  %s %s"%(item.id_,item.name, item.get_header_filename())
            header, body = code
            if not os.path.exists(item.get_include_parent_path().replace(" ","_").replace("<", "__").replace(">", "__").replace("::", "____").replace(", ", "__")):
                os.makedirs(item.get_include_parent_path().replace(" ","_").replace("<","__").replace(">","__").replace("::", "____").replace(", ","__"))
            with open(item.get_header_filename().replace("<", "__").replace(" ","_").replace(">","__").replace("::", "____").replace(", ", "__"), 'w') as f:
                f.write(header)
                print "Wrote header: %s"%f.name
            with open(item.get_body_filename().replace("<", "__").replace(" ","_").replace(">", "__").replace("::","____").replace(", ", "__"), 'w') as f:
                f.write(body)
                print "Wrote body: %s"%f.name