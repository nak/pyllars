#!/bin/bash
PYTHONPATH=. ./generation/processor.py  ../../examples/v8.xml && \
mkdir -p obj;
for file in `find pyllars/v8 -name \*.cpp`; do \
echo $file;
g++ -I. -fPIC --std=c++14 -I. -I/usr/include/python2.7 -L/usr/lib/x86_64-linux-gnu -lpython2.7 -c -I../../examples $file -o obj/`basename $file .cpp`.o;
done;
