from distutils.core import setup, Extension
import lxml.etree as et
import glob
import os.path
import sys

FNULL=open(os.devnull, 'w')
MAX_PROCESSES = 8

BLACK_LIST=['thread_db.h','ltdl.h','curses.h', 'turbojpeg.h', 'cursesw.h','cursesapp.h','kdb.h', 'gettext-po.h','cursesp.h', 'cursslk.h','gmon.h','gconv.h','alloca.h', 'link.h', 'com_err.h', 'argp.h', 'brlapi.h', 'brlapi_protocol.h', 'dialog.h', 'FlexLexer.h','tic.h','argz.h','dlg_keys.h','profile.h', 'gelf.h','gnumake.h'] + [os.path.basename(fname) for fname in glob.glob("/usr/include/wayland*.h")]

def generate_code(filename, filenamebase):
    if os.path.basename(filename) in BLACK_LIST:
        return False
    sys.stderr.write ("#####################################\n")
    sys.stderr.write ("Generating for filename %s\n"%filename)
    
    import subprocess
    
    #filename = os.path.join("to_convert",module1.name+".hpp")
    #target_namespace = '::'
    #module_name = module1.name
    module_name = "pyllars"
    outdir = os.path.join("build", "gen")
    try:
        os.makedirs(outdir)
    except:
        pass
    output_path = os.path.join(outdir, 'pyllars_core%s.xml'%filenamebase)
    import tempfile
    with tempfile.NamedTemporaryFile(mode='w+b',prefix=filenamebase, suffix='.hpp') as tmp:
        path_to_code = os.path.join(outdir,'pyllars_core%s.cpp'%filenamebase)
        tmp.write("#include <%s>"%filename)
        if os.path.exists(path_to_code):
            tmp.close()
            return True
        tmp.flush()
        args = ["castxml", "--castxml-gccxml", "--castxml-cc-gnu",
                "(", "g++", "-DNCURSES_NODEBUG", "-DMALLOC_NDEBUG", "-DBRLAPI_NO_SINGLE_SESSION", "-D_ASSERT_H_DECLS",
                "-std=c++11",  ")",
                "-o", output_path, 
                tmp.name]        
        print("Executing: '%s'"% " ".join(args))
        p = subprocess.Popen(args)
        if p.wait() != 0:
            print "ERROR PROCESSING %s"%filename
            return False
        tmp.close()
        xslt_path = os.path.join("..","pyllars","xslt", "cpp2pyllars.xslt")
        args=["xalan", "-param", "filename", "'%s'"%filename,
              "-param", "target_namespace","'::'",
              "-param", "module_name","'%s'"%module_name,
              "-in", output_path,
              "-xsl", xslt_path,
              "-out", path_to_code]
        sys.stderr.write(("Executing: '%s'\n"% " ".join(args)))
        p = subprocess.Popen(args, stdout=FNULL, stderr=FNULL)
    return p
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



modules = []
procs = []
proccount = 0

def addproc( proc, filenamebase):
    if proc != True:
        status = proc.poll()
    else:
        status = 0
    if status == 0:
        module = Extension(filenamebase,
                    include_dirs = ['.','../../pyllars'],
                    language='c++',
                    extra_compile_args=["-std=c++14",
                                         "-Wall",
                                         ],
                    extra_link_args = ["-Wl,--no-undefined",
                                       "-lrt",
                                       "-lanl",
                                       "-ldl",
                                       "-lltdl",
                                       "-lcap",
                                       "-lpng12",
                                       "-lz",
                                       "-lutil",
                                       "-ledit",
                                       "-lcurses",
                                       "/lib/x86_64-linux-gnu/libncursesw.so.5",
                                       "-lunwind",
                                       "-ludev",
                                       "-lunwind-coredump",
                                       "-lunwind-ptrace",
                                       "-lcom_err",
                                       "-lform", "/usr/lib/x86_64-linux-gnu/libformw.so.5",
                                       "-lgbm",
                                       "-lkrb5","-lpython2.7",
                                       "-lasprintf",
                                       "-ltic",
                                       "-lcrypt",
                                       "-lelf",
                                       "-lpanel",
                                       "-lpcrecpp", "-lpcre16", "-lpcre32", "-lpcre", "-lpcreposix",
                                       "-lexpat",
                                       "-lncurses",
                                       "-lpanel",
                                       "-lbz2",
                                       "-lthread_db",
                                       "-lbrlapi",
                                       "-lmenu"],
                    sources = ['build/gen/pyllars_core%s.cpp'%filenamebase],
                 )

        modules.append(module)
    elif status != None:
        sys.stderr.write("ERROR: processing %s"%filenamebase)
    return status

for filename in sorted(glob.glob("/usr/include/*.h")):
    filenamebase=os.path.basename(filename).replace(".h","")
    proc = generate_code(filename, filenamebase)
    if proc:
        procs.append((proc,filenamebase))
        proccount += 1
    if proccount > MAX_PROCESSES:
        found = False
        while not found:
            procscopy=procs
            for index,p in enumerate(procscopy):
                if addproc(*procs[index]) is not None:
                    found = True
                    proccount -=1
                    procs.remove(p)
            if found: break
            import time
            time.sleep(1)
            sys.stderr.write("Waiting...\n")

for filename in sorted(glob.glob("/usr/include/sys/*.h")):
    filenamebase="sys_"+os.path.basename(filename).replace(".h","")
    proc = generate_code(filename, filenamebase)
    if proc:
        procs.append((proc,filenamebase))
        proccount += 1
    if proccount > MAX_PROCESSES:
        found = False
        while not found:
            procscopy=procs
            for index,p in enumerate(procscopy):
                if addproc(*procs[index]) is not None:
                    found = True
                    proccount -=1
                    procs.remove(p)
            if found: break
            import time
            time.sleep(1)
            sys.stderr.write("Waiting...\n")

for proc, filenamebase in procs:
    addproc( proc, filenamebase)
    if proc != True and proc.wait() != 0:
        sys.stderr.write("ERROR: processing file %s"%filenamebase)

print "Setting up .... %d modules"%len(modules)
setup (name = 'pyllarscore',
       version = '1.0',
       description = 'Core Pyllars Module corresponding to %s'%filename,
       ext_modules = modules)
