import ply.lex as lex

tokens = (
    'implicit_explicit',
    'qualifier',
    'method_qualifier',
    'parameters',
    'reference',
    'name',
    'structured_type',
    'is_definition',
    'is_referenced',
    'throws'
)


t_reference = r"(\*|\&|\&\&)"
t_ignore = ' \t'


def t_is_definition(t):
    r'(definition)\ '
    t.value = True
    return t


def t_is_referenced(t):
    r'(referenced)\ '
    t.value = True
    return t


def t_throws(t):
    r'(throw)\ *\([\ \,\:a-zA-Z\,\*\&]*\)'
    t.throws = t.value.replace('throw', '').replace('(', '').replace(')', '').split(',')
    return t

def t_parameters(t):
    # This need to match any parameter list defn, including things like:
    # (int a, double (*func)(int p1, short & p2))
    r'\([\ \,\:a-zA-Z0-9_\,\*\&(\(.*\))]*\)'
    return t


def t_structured_type(t):
    r'(class|struct|union|typename)\ '
    t.value = t.value.strip()
    return t


# Define a rule so we can track line numbers
def t_newline(t):
    r'\n+'
    t.lexer.lineno += len(t.value)


def t_implicit_explicit(t):
    r'(implicit|explicit)\ '
    t.value = t.value.strip()
    return t


def t_qualifier(t):
    r'(const|constexpr|mutable|volatile)\ '
    t.value = t.value.strip()
    return t


def t_method_qualifier(t):
    r'(pure|virtual)\ '
    t.value = t.value.strip()
    return t


def t_name(t):
    r'([_\~\=a-zA-Z0-9\:]+)'
    return t


function_lexer = lex.lex()

