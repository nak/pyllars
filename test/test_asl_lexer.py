from collections import OrderedDict

import pyllars.cppparser.lexer as asl_lexer


def test_type_lexer_simple_struct():
    asl_lexer.lexer.input("|   |-CXXConstructorDecl 0x27e1110 <line:14:9, col:31> col:9 ExternalDependency 'void (void)'")
    tokens = {}
    while True:
        token = asl_lexer.lexer.token()
        if token is None:
            break
        if token.type in tokens:
            tokens[token.type] = [tokens[token.type], token]
        else:
            tokens[token.type] = token
    assert tokens.get('node_type').value == "CXXConstructorDecl"
    assert tokens.get('name').value == "ExternalDependency"
    assert tokens.get('definition').value == "void (void)"
