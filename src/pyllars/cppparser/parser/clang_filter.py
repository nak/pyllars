import logging
import re
import subprocess

import sys
import tempfile

from pyllars.cppparser.parser.code_structure import *
from typing import List, Iterator, Optional
from dataclasses import dataclass, field

log = logging.getLogger(__name__)


def _get_generator_class(name: str) -> Optional[Element]:
    generator_class = globals().get(name)
    if not generator_class:
        logging.error("Did not find generator for class %s" % name)
        return None
    return generator_class


@dataclass
class Node:
    type_name: str
    node_id: str
    parent: Optional["Node"] = None
    args: List[str] = field(default_factory=list)
    children: List["Node"] = field(default_factory=list)
    
    def transform(self, src_path:str, parent=None):
        TranformedClass = _get_generator_class(self.type_name)
        if not TranformedClass:
            return
        try:
            transformed = TranformedClass(*self.args, parent=parent, tag=self.node_id)
            for child in self.children:
                transformed.append_child(child.transform(src_path, parent=transformed))
            if transformed.location.startswith("line") or transformed.location.startswith("col"):
                transformed.set_location(src_path)
        except:
            log.error("Failed to transform element")
            import traceback
            traceback.print_exc()
            raise
        return transformed


class ClangFilter:

    def __init__(self, src_path: str):
        """
        """
        self._root_node = None
        self._src_path = src_path


    @classmethod
    def parse(cls, src_path: str, flags:str):
        processor = ClangFilter(src_path)
        tmpd = tempfile.mkdtemp()
        try:
            with open(os.path.join(str(tmpd), "compile_commands.json"), mode='w') as f:
                f.write("""
                    [
                    """)
                myflags = flags[:]
                myflags.append("-I%s" % os.path.abspath(os.path.dirname(src_path)))
                f.write("""
                {
                   "directory": "%(dir)s",
                   "file": "%(file)s",
                   "command": "g++ -std=c++14 -c %(flags)s %(file)s"
                }
                """ % {'dir': os.getcwd(),
                       'file': os.path.abspath(src_path),
                       'flags': " ".join(myflags)})
                f.write("""
                ]
                """)
                f.flush()
                cmd = ["clang-check", "-ast-dump", src_path, "--extra-arg=\"-fno-color-diagnostics\"", "-p", str(tmpd)]
                proc = subprocess.run(cmd, stdout=subprocess.PIPE, stderr=subprocess.PIPE, encoding='utf-8')
                return processor.process(iter(proc.stdout.splitlines()))
        except Exception as e:
            log.error("Failed to parse C++ headers: " + str(e))

    def process(self, lines: Iterator[str]):
        nodes = {}
        for line in lines:
            if self._root_node is None:
                assert not line.startswith('|') and not line.startswith('`')
                node_name, node_id, *args = [p for p in re.split(r'( |\".*?\"|\'.*?\'|\<\<.*?\>\>|\<.*?\>)', line) if
                                             p.strip()]
                self._root_node = Node(type_name=node_name, node_id=node_id, args=args)
                nodes[node_id] = self._root_node
                continue
            parent_node = self._root_node
            assert line[0] in ['|', '`', ' ']
            line = line[2:]
            while line[0:2] in ['|-', '`-', '  ', '| ']:
                parent_node = parent_node.children[-1]
                line = line[2:]
            node_name, node_id, *args = [p for p in re.split(r'( |\".*?\"|\'.*?\'|\<\<.*?\>\>|\<.*?\>)', line) if p.strip()]
            if args[0] == 'parent':
                parent = nodes[args[1]]
                args = args[2:]
            else:
                parent = parent_node
            leaf = Node(type_name=node_name, node_id=node_id, args=args, parent=parent)
            nodes[node_id] = leaf
            parent.children.append(leaf)
        return self._root_node.transform(self._src_path)


if __name__ == "__main__":
    flags = []
    paths = sys.argv[1:]
    for index, arg in enumerate(sys.argv):
        if arg.startswith('-'):
            flags = sys.argv[index:]
            paths = sys.argv[1:index]
            break
    if paths:
        ClangFilter.parse("pyllars.spec", src_paths=paths, flags=flags)