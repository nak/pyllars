'''
Created on Jun 23, 2015

@author: jrusnak
'''
import os.path

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
        self._python_code = {"globals":[]}
        
    def process(self, to_path):
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
        def add_typedef( name, basictype):
            typedefs[name] = basictype
        def add_functypedef( name, basictype):
            functypedefs[name] = basictype
        functiontypes=[]
        for line in self._file:
            line = line.replace('\n','').replace('\r','')
            #print ("PROCESSING LINE %s"%line)
            if line.startswith("===") and line.replace('=',''):
                self._curr_package = line.replace('=','')
                if self._curr_package not in self._python_code:
                    self._python_code[self._curr_package] =[]
                #print("CURR PACKAGE NOW %s"%self._curr_package)
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
                #print("ADDING CODE ")
                self._python_code[self._curr_package].append(line)
                if codetext:
                    eval(codetext)
                    codetext = ""
        for package,code in self._python_code.items():
            if package.startswith('.'): package = package[1:]
            packagedir = os.path.join(to_path, package.replace(".","/"))
            if not os.path.exists(packagedir):
                os.makedirs(packagedir)
            initfile = os.path.join(packagedir,"__init__.py")
            print("Writing to %s"%initfile)
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
                    if not package: package = "globals"
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
                        outfile.write(func +"\n")
                    outfile.write("\n")
                if functypedefs:
                    outfile.write("#FUNCTION TYPEDEFS\n")
                    for typename, basictype in functypedefs.items():
                        outfile.write("%s = %s\n"%( typename, basictype))
                    outfile.write("\n")
                for line in code:
                    outfile.write(line +"\n" )