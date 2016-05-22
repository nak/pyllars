#!/bin/bash
PYTHONPATH=. ./generation/processor.py  ../../examples/v8.xml && \
g++ -I. -fPIC --std=c++14 -I. -I/usr/include/python2.7 pyllars/v8/module.cpp  -L/usr/lib/x86_64-linux-gnu -lpython2.7 `find pyllars/v8 -name \*.cpp`  -c -I../../examples 
