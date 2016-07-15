#!/bin/bash
PYTHONPATH="../pyllars" \
../pyllars/generation/processor.py  build/gen/testbasic_C.xml && \
    echo "Compiling..." && \
    g++ -g -O0 -fPIC --std=c++11  -I../pyllars -I. -I/opt/Python/include/python2.7  --shared -o test_pyllars.so `find pyllars/test_pyllars/TestStruct -name \*.cpp` testmod.cpp  ../pyllars/pyllars.cpp  -lstdc++ -Wl,--no-undefined -L/opt/Python/lib -lpython2.7 -I./to_convert ./to_convert/testbasic_C.cpp pyllars/test_pyllars/module.cpp 

