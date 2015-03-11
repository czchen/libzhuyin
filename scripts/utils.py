# -*- coding: utf-8 -*-
# vim:set et sts=4 sw=4:
#
# libzhuyin - Library to deal with zhuyin.
#
# Copyright (C) 2011 Peng Wu <alexepico@gmail.com>
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2, or (at your option)
# any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
# MA  02110-1301, USA.


import os

header = '''/* This file is generated by python scripts. Don't edit this file directly.
 */
'''


def expand_file(filename, get_table_content):
    infile = open(filename, "r")
    print(header)
    for line in infile.readlines():
        line = line.rstrip(os.linesep)
        if len(line) < 3:
            print(line)
            continue
        if line[0] == '@' and line[-1] == '@':
            tablename = line[1:-1]
            print(get_table_content(tablename))
        else:
            print(line)


def shuffle_all(instr):
    for output in shuffle_recur(instr):
        if output == instr:
            continue
        yield output


def shuffle_recur(instr):
    if len(instr) == 1:
        yield instr
    else:
        for i, ch in enumerate(instr):
            recur = instr[:i] + instr[i+1:]
            for s in shuffle_recur(recur):
                yield ch + s


if __name__ == "__main__":
    for s in shuffle_all("abc"):
        print(s)
