import ply.lex as lex
from ply.lex import TOKEN

tokens = (
    'implicit_explicit',
    'qualifier',
    'reference',
    'name',
    'structured_type',
    'is_definition',
    'is_referenced',
)


t_reference = r"(\*|\&|\&\&)"
t_ignore = ' \t'


def t_is_definition(t):
    r'(definition)'
    t.value = True
    return t


def t_is_referenced(t):
    r"(referenced)"
    t.value = True
    return t


def t_structured_type(t):
    r"(class|struct|union|typename)"
    return t

# Define a rule so we can track line numbers
def t_newline(t):
    r'\n+'
    t.lexer.lineno += len(t.value)


def t_implicit_explicit(t):
    r"(implicit|explicit)"
    return t

def t_qualifier(t):
    r"(const|constexpr|mutable|volatile)"
    return t


def t_name(t):
    r"([:\<\>_\~\=a-zA-Z0-9\<\>\,\ ]+)"
    return t


type_lexer = lex.lex()

