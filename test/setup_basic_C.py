from distutils.core import setup, Extension
import lxml.etree as et
module1 = Extension('testbasic_C',
                    include_dirs = ['.','../../pyllars'],
                    language='c++',
                    extra_compile_args=["-std=c++11",
                                         
                                         "-Wall",
                                         ],
                    extra_link_args = ["-Wl,--no-undefined"],
                    sources = ['build/gen/testbasic_C.cpp'],
                 )

def generate_code():
    import os.path
    import subprocess
    import sys
    
    filename = os.path.join("to_convert",module1.name+".hpp")
    target_namespace = '::'
    module_name = module1.name

    outdir = os.path.join("build", "gen")
    try:
        os.makedirs(outdir)
    except:
        pass
    output_path = os.path.join(outdir, module1.name+'.xml')
    args = ["castxml", "--castxml-gccxml", "--castxml-cc-gnu",
                        "(", "g++", "-std=c++11", ")",
                        "-o", output_path, 
                        filename]
    print("Executing: '%s'"% " ".join(args))
    p = subprocess.Popen(args)
    if p.wait() != 0:
      sys.exit(1)
    xslt_path = os.path.join("..","pyllars","xslt", "cpp2pyllars.xslt")
    args=["xalan", "-param", "filename", "'%s'"%filename,
          "-param", "target_namespace","'::'",
          "-param", "module_name","'%s'"%module_name,
          "-in", output_path,
          "-xsl", xslt_path,
          "-out", os.path.join(outdir, module1.name+'.cpp')]
    p = subprocess.Popen(args)
    if p.wait() != 0:
        sys.exit(1)
    #dom = et.parse(output_path)
    #print "Running %s on %s"%(xslt_path, output_path)
    #xslt = et.parse(xslt_path)
    #transform = et.XSLT(xslt)
    #newdom = transform(dom,
    #                   filename="'%s'"%filename,
    #                   target_namespace="'::'",
    #                   module_name="'%s'"%module_name)
    #output_code = os.path.join(outdir, module1.name+ '.cpp')
    #with open(output_code, 'w+b') as f:
    #    f.write(newdom)
        
generate_code()
setup (name = 'PyllarsBasicCTest',
       version = '1.0',
       description = 'This is a test package',
       ext_modules = [module1])
