from collections import OrderedDict

import pyllars.cppparser.type_lexer as type_lexer


def test_type_lexer_simple_struct():
    type_lexer.type_lexer.input("explicit struct SomeClass")
    tokens = {}
    while True:
        token = type_lexer.type_lexer.token()
        if token is None:
            break
        if token.type in tokens:
            tokens[token.type] = [tokens[token.type], token]
        else:
            tokens[token.type] = token
    assert tokens.get('name').value == "SomeClass"
    assert tokens.get('structured_type').value == 'struct'
    assert tokens.get('implicit_explicit').value == "explicit"
    assert not tokens.get('qualifier')


def test_type_lexer_simple_struct_ptr():
    type_lexer.type_lexer.input("explicit struct SomeClass*")
    tokens = OrderedDict()
    while True:
        token = type_lexer.type_lexer.token()
        if token is None:
            break
        if token.type in tokens:
            tokens[token.type] = [tokens[token.type], token]
        else:
            tokens[token.type] = token
    assert tokens.get('name').value == "SomeClass"
    assert tokens.get('structured_type').value == 'struct'
    assert tokens.get('implicit_explicit').value == "explicit"
    assert tokens.get('reference').value == '*'
    assert not tokens.get('qualifier')


def test_type_lexer_template():
    type_lexer.type_lexer.input("implicit class TemplateClass<arg1 *, arg2>[3] const volatile")
    tokens = {}
    while True:
        token = type_lexer.type_lexer.token()
        if token is None:
            break
        if token.type in tokens:
            tokens[token.type] = [tokens[token.type], token]
        else:
            tokens[token.type] = token
    assert tokens.get('name').value == "TemplateClass<arg1 *, arg2>"
    assert tokens.get('structured_type').value == 'class'
    assert tokens.get('implicit_explicit').value == "implicit"
    assert tokens.get('array_spec').value == 3
    assert set([t.value for t in tokens.get('qualifier')]) == set(['const', 'volatile'])
