from distutils.core import setup, Extension

module1 = Extension('test',
                    include_dirs = ['.'],
                    language='c++',
                    extra_compile_args=["-std=c++11",
                                         "-g", "-O0",
                                         "-Wall",
                                         ],
                    sources = ['test/test_basics.cpp'],
                 )

setup (name = 'PackageName',
       version = '1.0',
       description = 'This is a test package',
       ext_modules = [module1])
