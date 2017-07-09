#!/usr/bin/env python
import glob
from distutils.core import setup
from distutils.extension import Extension

setup(name='pyllars',
      version='1.0',
      description='Pyllars, an auto-generated C++ Python interface tool',
      author='John Rusnak',
      author_email='jrusnak69@gmail.com',
      url='https://github.com/nak/pyllars',
      packages=['pyllars', 'pyllars/cppparser', 'pyllars/cppparser/generation'],
      data_files=[('pyllars/resources/pyllars', glob.glob('resources/pyllars/*.*pp'))],
      scripts=['pyllars/pyllars-generate'],
      ext_modules=[Extension('pyllars', ['resources/pyllars/pyllars.cpp'], extra_compile_args=['-std=c++14'],
                             requires=['typing'])]
     )
