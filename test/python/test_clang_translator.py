from pyllars.cppparser.parser.clang_translator import ClangTranslator, NodeType

import os

RESOURCES_DIR=os.path.join(os.path.dirname(__file__), "resources")


def compare_builtintype(self, other: NodeType.BuiltinType):
    return type(other) == NodeType.BuiltinType and \
        other.node_id == self.node_id and \
        other.type_text == self.type_text


NodeType.BuiltinType.compare = compare_builtintype


def compare_typedefdecl(self, other: NodeType.TypedefDecl):
    return type(other) == NodeType.TypedefDecl and \
        other.node_id == self.node_id and \
        other.col_loc == self.col_loc and \
        other.line_loc == self.line_loc and \
        other.name == self.name and \
        other.target_name == self.target_name and \
        len(self.children) == len(other.children) and \
           (self.children[0].compare(other.children[0]) if self.children else True)


NodeType.TypedefDecl.compare = compare_typedefdecl


def compare_namespacedecl(self, other: NodeType.NamespaceDecl):
    return type(other) == NodeType.NamespaceDecl and \
            other.node_id == self.node_id  and \
            other.col_loc == self.col_loc and \
            other.line_loc == self.line_loc and \
            other.name == self.name


NodeType.NamespaceDecl.compare = compare_namespacedecl


expectation = NodeType.TranslationUnitDecl('0x186bca8', '<<invalid sloc>>', '<invalid sloc>')
child = NodeType.TypedefDecl('0x186c580', "<<invalid sloc>>", '<invalid sloc>', 'implicit', '__int128_t', '__int128')
child.children.append(NodeType.BuiltinType('0x186c240', '__int128'))
expectation.children.append(child)
child = NodeType.TypedefDecl('0x186c5e8', '<<invalid sloc>>',  '<invalid sloc>', 'implicit', '__uint128_t', 'unsigned __int128')
expectation.children.append(child)
child = NodeType.TypedefDecl('0x186c928', '<<invalid sloc>>',  '<invalid sloc>', 'implicit', '__NSConstantString', '__NSConstantString_tag')
expectation.children.append(child)
child = NodeType.TypedefDecl('0x18a6130', '<<invalid sloc>>',  '<invalid sloc>', 'implicit', '__builtin_ms_va_list', 'char *')
expectation.children.append(child)
child = NodeType.TypedefDecl('0x18a6468', '<<invalid sloc>>',  '<invalid sloc>', 'implicit', '__builtin_va_list', '__va_list_tag [1]')
expectation.children.append(child)
child = NodeType.NamespaceDecl('0x18a64b8', '</home/jrusnak/workspace/pyllars/test/python/resources/classes.hpp:4:1, line:5:1>',
                                'line:4:11', 'unused')
expectation.children.append(child)
child = NodeType.TypedefDecl('0x18a6538', '<line:7:1, col:16>', 'col:16', 'str', 'char *')
expectation.children.append(child)
child = NodeType.TypedefDecl('0x18a65a0', '<line:8:1, col:27>', 'col:27', 'const_str', 'const char *const')
expectation.children.append(child)
child = NodeType.NamespaceDecl('0x18a65f0', '<line:11:1, line:14:1>', 'line:11:11', 'outside')
expectation.children.append(child)
child = NodeType.NamespaceDecl('0x18a6830', '<line:16:1, line:68:1>', 'line:16:11', 'trial')
expectation.children.append(child)


class TestClangTranslation:

    def test_clang_translation(self):
        file_name = os.path.join(RESOURCES_DIR, "clang-check-output.example")
        with ClangTranslator(file_name=file_name) as translator:
            root = translator.translate()
            for i, child_node in enumerate(root):
                assert(expectation.children[i].compare(child_node))
                print(child_node)

