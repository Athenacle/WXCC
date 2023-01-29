#!/usr/bin/python


import sys
import time
import os
from pathlib import Path, PurePath
import re

root = None
branch = None
project = None
commit = None
ctv = None


usage = """usage:
\tpython {} -r <source root> -b <branch> -c <commit sha> -p <project name> -v <clang-tidy version>
"""
i = 1
while i < len(sys.argv):
    cur = sys.argv[i]
    if cur == "-r":
        root = sys.argv[i + 1]
        i += 1
    elif cur == "-b":
        branch = sys.argv[i + 1]
        i += 1
    elif cur == "-c":
        commit = sys.argv[i + 1]
        i += 1
    elif cur == "-p":
        project = sys.argv[i + 1]
        i += 1
    elif cur == "-v":
        ctv = sys.argv[i + 1]
        i += 1
    else:
        print(usage.format(sys.argv[0]), file=sys.stderr)
        exit(1)
    i += 1

rootP = None
if root:
    rootP = PurePath(os.path.abspath(root))


class Warning:
    level = "level"
    file = "file"
    pos_line = 0
    pos_colum = 0
    msg = ""
    check_name = ""
    check_id = ""
    codes = ()
    notes = ()

    def __init__(self, l, f, pl, pc, m, cn, ci) -> None:
        self.level = l
        if rootP:
            try:
                self.file = PurePath(f).relative_to(rootP)
            except:
                self.file = f
        else:
            self.file = f
        self.pos_line = pl
        self.pos_colum = pc
        self.msg = m
        self.check_name = cn
        self.check_id = ci

    def addCode(self, l):
        rep = l.replace("\\", "\\\\").replace('''"''', """\\"!!""")
        rep = '"' + rep + '"'
        temp = list(self.codes)
        temp.append(rep)
        self.codes = tuple(temp)

    def addNote(self, f, pl, pc, m):
        nn = Warning("note", f, pl, pc, m, "", "")
        temp = list(self.notes)
        temp.append(nn)
        self.notes = tuple(temp)

    def addNoteCode(self, c):
        self.notes[len(self.notes) - 1].addCode(c)

    def json(self):
        position = """"position":{"file":"%s","line":%s,"colum":%s}""" % (
            self.file,
            self.pos_line,
            self.pos_colum,
        )

        level = ""
        if not self.level == "note":
            level = '''"level":"%s"''' % (self.level)

        msg = '''"message":"%s"''' % (self.msg)

        check = ""
        if self.check_name == "" or self.check_id == "":
            check = ""
        else:
            check = """"check":{"name":"%s","id":"%s"}""" % (
                self.check_name,
                self.check_id,
            )

        notes = ""
        if len(self.notes) > 0:
            arr = []
            for n in self.notes:
                arr.append(n.json())
            notes = """"notes":[%s]""" % (",".join(arr))

        c = ""
        if len(self.codes) > 0:
            c = """"codes":[""" + ",".join(self.codes) + """]"""

        arr = [position, msg]
        if len(c) > 0:
            arr.append(c)
        if len(notes) > 0:
            arr.append(notes)
        if len(check) > 0:
            arr.append(check)
        if len(level) > 0:
            arr.append(level)

        out = "{" + ",".join(arr) + "}"
        return out


checks = []
checkRe = re.compile(r"(\w+)-(.+)")
for line in sys.stdin:
    l = line.strip()
    m = checkRe.search(l)
    print("skipping.. " + l, file=sys.stderr)

    if l == "":
        break
    elif not m is None:
        checks.append((m.group(1), m.group(2)))

msgRe = re.compile(r"^(\/.*)+:(\d+):(\d+):\s+(warning|error|note):\s+(.*)")
ctRe = re.compile(r"^(/.+)+/clang-tidy")
checkIdRe = re.compile(r"([^\[]+) \[(\w*)-(.*)]")
genRe = re.compile(r"\d+\s+warnings?\s+generated")
note = False
warnings = []

for line in sys.stdin:
    l = line.rstrip()
    if ctRe.search(l) or genRe.search(l):
        print("contine: " + l, file=sys.stderr)
        continue
    m = msgRe.search(l)
    if not m is None:
        file = m.group(1)
        line = m.group(2)
        colum = m.group(3)
        level = m.group(4)
        msg = m.group(5)
        check = checkIdRe.search(msg)
        if level == "note":
            note = True
            warnings[len(warnings) - 1].addNote(file, line, colum, msg)
            continue

        if not check is None:
            current = Warning(
                level, file, line, colum, check.group(
                    1), check.group(2), check.group(3)
            )
            warnings.append(current)
            note = False
        else:
            print("false!: " + l, file=sys.stderr)
    else:
        if len(warnings) > 0:
            if not note:
                warnings[len(warnings) - 1].addCode(l)
            else:
                warnings[len(warnings) - 1].addNoteCode(l)
        else:
            pass


def doPrint(msg):
    print(msg, end="")


cdict = {}

for c in checks:
    name = c[0]
    id = c[1]

    if name in cdict:
        if id in cdict[name]:
            continue
        else:
            cdict[name].append('"' + id + '"')
    else:
        cdict[name] = ['"'+id+'"']

doPrint("{")

if root:
    doPrint(""""root": "%s",""" % (rootP))
if ctv:
    doPrint(""""clang-tidy-version":"%s",""" % ctv)
if project and commit and branch:
    doPrint(
        """"project":{"name":"%s", "commit":"%s", "branch":"%s"}, """
        % (project, commit, branch)
    )

cc = []
for c in cdict:
    line = cdict[c]
    cc.append(('"%s":[' + (','.join(line)) + ']') % c)
if len(cc) > 0:
    doPrint('''"checks":{%s},''' % (",".join(cc)))

doPrint(
    """"time":"%s",""" % (time.strftime(
        "%a, %d %b %Y %H:%M:%S %z", time.localtime()))
)
doPrint(""""count": %d,""" % len(warnings))
doPrint(""""data":[""")
i = 0
while i < len(warnings) - 2:
    doPrint(warnings[i].json())
    doPrint(",")
    i += 1
doPrint(warnings[len(warnings) - 1].json())
doPrint("""]}""")
