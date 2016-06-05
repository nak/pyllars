#!/bin/bash
PYTHONPATH="../pyllars"
../pyllars/generation/processor.py  build/gen/testbasic_C.xml && \
    echo "Compiling..." && \
    g++ -g -O0 -fPIC --std=c++11  -I../pyllars -I. -I/usr/include/python2.7 --shared -o test.so `find pyllars/TestStruct -name \*.cpp` testmod.cpp  pyllars.cpp  pyllars/module.cpp -lstdc++ -Wl,--no-undefined -L/usr/lib/x86_64-linux-gnu/ -lpython2.7 -I./to_convert ./to_convert/testbasic_C.cpp
