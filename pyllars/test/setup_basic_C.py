from distutils.core import setup, Extension
import os

import sys
sys.path += [os.path.join(os.getcwd(), "..")]

opt_level=os.getenv("OPTIMIZATION_LEVEL") or "0"
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
outdir = os.path.join("build", "gen")

def generate_code():
    import os.path
    import subprocess
    import sys
    from pyllars.generation import processor
    try:
        os.makedirs(outdir)
    except:
        pass
    output_path = os.path.join(outdir, module_name+'.xml')
    args = ["/usr/local/bin/castxml", "--castxml-gccxml", "--std=c++11",
                        "-o", output_path,
                        " ".join(files_to_convert)]
    print("Executing: '%s'"% " ".join(args))
    p = subprocess.Popen(args)
    if p.wait() != 0:
        sys.exit(1)
    return processor.process(output_path, outdir, class_filters=["__do_is_default_constructible_impl", ])

        
compilables = generate_code()
sources={}
for mod_name, compilable in compilables:
    base_mod_name = mod_name.split('.')[0]
    sources.setdefault(base_mod_name, []).append(compilable)
for base_mod_name in sources:
    sources[base_mod_name] += addl_sources_by_module_name.get(base_mod_name) or []
print "==========> %s" % sources.keys()
modules=[]
for mod, compilables in sources.iteritems():
    #print mod
    if mod != "test_pyllars":
        continue
    module = Extension(mod,
                   include_dirs = ['.','../pyllars', outdir, ],
                   language='c++',
                   extra_compile_args=["-std=c++14",
                                       "-fPIC",
                                       "-O%s" % opt_level,
                                       "-Wall",
                                     ],
                   extra_link_args=["-lffi", "-Wl,--no-undefined", "-fPIC", "-lpython2.7"],
                   sources=list(set(compilables +["../pyllars/pyllars.cpp", "build/gen/pyllars/pyllars/module.cpp" ])),
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
