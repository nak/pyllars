#!/usr/bin/env python
from pyllars.generation.parser import CPPParser

import sys

if not len(sys.argv) == 2:
    print("Usage: %s <cppxmlfile>"%sys.argv[0])
    sys.exit()

parser = CPPParser(to_path = "./")
parser.process_stream_to_py( sys.argv[1] )
