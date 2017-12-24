import ply.lex as lex
from ply.lex import TOKEN

tokens = (
    'node_type',
    'tag',
    'parent_tag',
    'node',
    'locator',
    'implicit_explicit',
    'qualifier',
    'method_qualifier',
    'reference',
    'definition',
    'name',
    'structured_type',
    'is_definition',
    'is_referenced',
    'access',
    'sugar',
    'float',
    'number',
    'throws',
    'keyword',
    'integer_value',
    'col',
    'line',
    'separator',
    'angle_bracket',
    'string'
)


t_reference = r"(\*|\&|\&\&)"
t_ignore = '\t '


def t_locator(t):
    r"(<?[0-9A-Za-z\_\/\-\.\\]*\:[0-9]+\:[0-9]+>?)|(<<?invalid\ sloc>?>)"
    t.value = t.value.replace("<", "").replace(">", "")
    return t


def t_structured_type(t):
    r"(class|struct|union|typename|enum)\ "
    return t


def t_definition(t):
    r'\'([a-zA-Z0-9\_\*\[\]\(\)\&(\:\:)(\.\.\.)\,\ (\<.*\>)]*)\ *(throw\(.*\))?\''
    t.value = t.value.replace("\\", "").replace("'", "")
    t.value = t.value.replace("'", "").strip()
    return t


def t_integer_value(t):
    'r([0-9]+)'
    t.value = int(t.value)
    return t


def t_node(t):
    r'([\|\ \`]+\-)'
    t.final = t.value.endswith("`-")
    index = t.lexer.lexdata.index(t.value)
    t.value = t.lexer.lexdata[:index] + t.value
    t.depth = (len(t.value)-2)//2
    return t


def t_parent_tag(t):
    r'(parent\ 0x[0-9,a-z]*)'
    t.value = t.value.replace('parent ', '')
    return t


def t_sugar(t):
    r'(sugar)'
    pass


def t_tag(t):
    r'(0x[0-9,a-z]*)'
    return t


decls = r'(TranslationUnitDecl|BuiltinType|TypedefDecl|ParmVarDecl|CXXRecordDecl|CXXRecord|CxxMethodDecl|' + \
         'ClassTemplateSpecializationDecl|ClassTemplateSpecialization|TemplateArgument|TypeAliasDecl|AccessSpecDecl|' + \
         'TemplateSpecializationType|RecordType|NamespaceDecl|PointerType|ArrayType|ConstantArrayType|' + \
         'CXXConstructorDecl|CXXDestructorDecl|CXXMethodDecl|CompoundStmt|ClassTemplateDecl|TemplateTypeParmDecl|' + \
         'LValueReferenceType|ElaboratedType|VarDecl|QualType|ImplicitCastExpr|IntegerLiteral|ReturnStmt|FloatingLiteral|' + \
         'NonTypeTemplateParmDecl|TypeAliasDecl|FunctionDecl|FieldDecl|IntegerLiteral|EnumDecl|EnumConstantDecl|LinkageSpecDecl' + \
         ')'

@TOKEN(decls)
def t_node_type(t):
    return t


def t_float(t):
    r'([0-9]+\.[0-9]*([-+][0-9]*)?)'
    return t


def t_is_definition(t):
    r'(definition)'
    t.value = True
    return t


def t_is_referenced(t):
    r"(referenced)"
    t.value = True
    return t

def t_access(t):
    r'(public|protected|private)'
    return t


# Define a rule so we can track line numbers
def t_newline(t):
    r'\n+'
    t.lexer.lineno += len(t.value)


def t_implicit_explicit(t):
    r"(implicit|explicit)"
    return t

def t_throws(t):
    r'(throw\()[.\ ]*\)'
    return t


def t_qualifier(t):
    r'(const|constexpr|mutable|volatile|extern|static|explicit|implicit)\ '
    t.value = t.value.strip()
    return t


def t_method_qualifier(t):
    r'(pure|virtual)'
    return t


def t_number(t):
    r'([+-]?[0-9]+\.?[0-9]*((e\+|e\-)?[0-9]+)?)'
    return t


def t_keyword(t):
    r'(inline|noexcept-unevaluated|default_delete|default|trivial|used)'
    return t


def t_col(t):
    r'(col\:[0-9]*)'
    return t


def t_line(t):
    r'(line\:[0-9]*:[0-9]*)'
    return t


def t_name(t):
    r'[_\~a-zA-Z].([_\~\=a-zA-Z0-9\<\>\,]*(\[\])?)'
    t.value = t.value.strip()
    return t


def t_separator(t):
    r'(\:)'
    return t


def t_angle_bracket(t):
    "[<>,]+"
    return t


def t_string(t):
    "\".*\""
    return t

def t_error(t):
    return t

lexer = lex.lex()

