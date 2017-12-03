from collections import OrderedDict
import pytest

import pyllars.cppparser.lexer as asl_lexer

lines = [("| | |-CXXMethodDecl 0x6aa79c8 <col:9, <invalid sloc>> col:9 implicit operator= 'class trial::Inherited &(const class trial::Inherited &)' inline noexcept-unevaluated 0x6aa79c8 ",
          [('node_type', 'CXXMethodDecl'), ('name', 'operator=')]),
         ("|   |-CXXConstructorDecl 0x27e1110 <line:14:9, col:31> col:9 ExternalDependency 'void (void)'",
          [('node_type', 'CXXConstructorDecl'), ('name', 'ExternalDependency'), ('definition', "void (void)")]),
         ("|-FunctionDecl 0x60a51e0 <<invalid sloc>> <invalid sloc> implicit operator new[] \'void *(unsigned long)\'",
          [])
]


@pytest.mark.parametrize("line", lines)
def test_type_lexer_simple_struct(line):
    line, expectations = line
    asl_lexer.lexer.input(line)
    tokens = {}
    while True:
        token = asl_lexer.lexer.token()
        if token is None:
            break
        if token.type in tokens:
            tokens[token.type] = [tokens[token.type], token]
        else:
            tokens[token.type] = token
    for expectation in expectations:
        assert tokens.get(expectation[0]).value == expectation[1]
