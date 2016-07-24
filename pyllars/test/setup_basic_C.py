from distutils.core import setup, Extension
import lxml.etree as et
import os

opt_level=os.getenv("OPTIMIZATION_LEVEL") or "3"
module_name = "test_pyllars"
addl_sources_by_module_name={"test_pyllars": ["./to_convert/testbasic_C.cpp"]}
def get_files_to_convert(path):
    headers=[]
    for root, dirs, files in os.walk(path):
        for f in files:
            if f.endswith(".h") or f.endswith(".hpp"):
                headers.append(os.path.join(root,f))
    return headers

files_to_convert = get_files_to_convert("./to_convert")

def generate_code():
    import os.path
    import subprocess
    import sys
    from pyllars.generation import processor
    outdir = os.path.join("build", "gen")
    try:
        os.makedirs(outdir)
    except:
        pass
    output_path = os.path.join(outdir, module_name+'.xml')
    args = ["castxml", "--castxml-gccxml", "--castxml-cc-gnu",
                        "(", "g++", "-std=c++11", ")",
                        "-o", output_path, 
                        " ".join(files_to_convert)]
    print("Executing: '%s'"% " ".join(args))
    p = subprocess.Popen(args)
    if p.wait() != 0:
      sys.exit(1)
    return processor.process(output_path, outdir)

        
compilables = generate_code()
sources={}
for mod_name, compilable in compilables:
    base_mod_name = mod_name.split('.')[0]
    sources.setdefault(base_mod_name, []).append(compilable)
for base_mod_name in sources:
    sources[base_mod_name] += addl_sources_by_module_name.get(base_mod_name) or []
print "==========> %s" % sources
modules=[]
for mod, compilables in sources.iteritems():
    module = Extension(mod,
                       include_dirs = ['.','../pyllars'],
                       language='c++',
                       extra_compile_args=["-std=c++14",
                                           "-fPIC",
                                        "-O%s" % opt_level,
                                         "-Wall",
                                         ],
                       extra_link_args=["-Wl,--no-undefined", "-fPIC", "-lpython2.7"],
                       sources=compilables +["../pyllars/pyllars.cpp"],
                       )
    modules.append(module)
    try:
        setup (name = 'PyllarsBasicCTest',
               version = '1.0',
               description = 'Python bynding to namespace/global file %s' % mod,
               ext_modules = [module])
    except:
        print "Failed to compiled moduled %s" % mod
        import traceback
        traceback.print_exc()
