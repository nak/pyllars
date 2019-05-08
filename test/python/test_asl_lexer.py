import pytest

import pyllars.cppparser.parser.clang_lexer as asl_lexer

lines = [
    ("|-FunctionDecl 0x20d3e70 <line:272:1, /usr/include/sys/cdefs.h:61:27> pthread.h:272:12 pthread_detach 'int (pthread_t) throw()' extern ",
    [('node_type', 'FunctionDecl'), ('name', 'pthread_detach')]),
    ("| | |-CXXMethodDecl 0x6aa79c8 <col:9, <invalid sloc>> col:9 implicit operator= 'class trial::Inherited &(const class trial::Inherited &)' inline noexcept-unevaluated 0x6aa79c8 ",
          [('node_type', 'CXXMethodDecl'), ('name', 'operator=')]),
         ("|   |-CXXConstructorDecl 0x27e1110 <line:14:9, col:31> col:9 ExternalDependency 'void (void)'",
          [('node_type', 'CXXConstructorDecl'), ('name', 'ExternalDependency'), ('definition', "void (void)")]),
         ("|-FunctionDecl 0x60a51e0 <<invalid sloc>> <invalid sloc> implicit operator new[] \'void *(unsigned long)\'",
          []),
]


@pytest.mark.parametrize("line", lines)
def test_asl_lexer(line):
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
