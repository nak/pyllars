from collections import namedtuple
from typing import List

from ..parser import Element, UnresolvedElement


class ClangOutputReader(object):

    LineData = namedtuple('LineData', ['declaration', 'tag', 'location', 'location_start', 'content'])

    TYPE_CONTINUE = b'|'[0]
    TYPE_LAST = b'`'[0]
    TYPE_BLOCK = b' '[0]

    DIRECTIVE_NEW_SUBELEMENT = b'-'[0]
    DIRECTIVE_LOWER_LEVEL = b' '[0]

    def __init__(self, parent_tag):
        self._node = None
        self._element = None
        self._children = []
        self._parent_tag = parent_tag

    def parse(self, line):
        if not line.strip():
            return

        type = line[0]
        directive = line[1]
        if type not in [self.TYPE_BLOCK, self.TYPE_CONTINUE, self.TYPE_LAST]:
            self._element = self._resolve(line, parent_tag=self._parent_tag)
            return

        if directive == self.DIRECTIVE_NEW_SUBELEMENT:
            element = self._resolve(line[2:], self._parent_tag)
            self._children.append(element)
            self._node = ClangOutputReader(self._element.tag)
            self._node._element = element
        elif directive == self.DIRECTIVE_LOWER_LEVEL:
            self._node.parse(line[2:])
        else:
            raise Exception("Invalid hiearchy in clang output.  Aborting")

    @staticmethod
    def _resolve(line, parent_tag):
        r = r"(?P<declaration>[a-z,A-Z]+)\ (?P<tag>0x[0-9,a-f]+)\ *" \
        "(?P<location>\<.*\>)?\ *" \
        "(?P<location_start>(\<invalid\ sloc\>|line\:[0-9]+\:[0-9]+|col\:[0-9]+))?\ *" \
        "(?P<content>.*)"
        import re
        reg = re.compile(r)
        match = reg.match(str(line, 'utf-8'))
        line_data = ClangOutputReader.LineData(**match.groupdict())
        from .clang_lexer import lexer
        lexer.input(line_data.content)
        token = lexer.token()
        content = {}
        while token:
            if token.type == 'error':
                print("ERROR processing content: %s" % line_data.content)
                return UnresolvedElement(name="errored", defining_scope=None)
            if token.type in content:
                # more than one token of same type, so convert to List
                if not isinstance(content[token.type], List):
                    content[token.type] = [content[token.type]]
                content[token.type].append(token.value)
            elif token.type in {'qualifier'}:  # treat these always as lists
                content[token.type] = [token.value]
            else:
                content[token.type] = token.value
            token = lexer.token()
        if 'name' not in content:
            content['name'] = None
        return Element.parse(line_data.declaration, line_data.tag, parent_tag=parent_tag, **content)


def parse_files(src_paths: List[str], include_paths: List[str]):
    import os.path
    import tempfile
    import subprocess
    Element.reset()
    includes = " ".join(['-I%s' % path for path in include_paths])
    tmpd = tempfile.mkdtemp()
    try:
        with open(os.path.join(str(tmpd), "compile_commands.json"), mode='w') as f:
            f.write("""
                [
                """)
            for index, src_path in enumerate(src_paths):
                    f.write("""
                {
                   "directory": "%(dir)s",
                   "file": "%(file)s",
                   "command": "g++ -std=c++14 -c %(includes)s %(file)s"
                }
                """ % {'dir': os.path.dirname(src_path),
                       'file':os.path.basename(src_path),
                       'includes': includes})
                    if index != (len(src_paths) - 1):
                        f.write(',')
            f.write("""
            ]
            """)
            f.flush()
        cmd = ["clang-check", "-ast-dump"] + src_paths + ["--extra-arg=\"-fno-color-diagnostics\"", "-p", str(tmpd)]
        proc = subprocess.Popen(cmd, stdout=subprocess.PIPE)
        reader = ClangOutputReader()
        for line in iter(proc.stdout.readline, None):
            reader.parse(line)
            if not line:
                break
        return Element.parse(proc.stdout, include_paths)
    finally:
        import shutil
        shutil.rmtree(str(tmpd))
