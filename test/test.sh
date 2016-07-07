#!/bin/bash

# Copyright (C) 2016  Stefan Vargyas
# 
# This file is part of Json-Type.
# 
# Json-Type is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
# 
# Json-Type is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# 
# You should have received a copy of the GNU General Public License
# along with Json-Type.  If not, see <http://www.gnu.org/licenses/>.

#
# File generated by a command like:
# $ json-gentest -w87 -R parser type-lib type-gen-def type-gen-def-gcc type-checker type-checker-list type-cases trie bugs
#

test "$1" == "-q" &&
q="q" ||
q=""

p=0
f=0

for t in \
    parser \
    type-lib \
    type-gen-def \
    type-gen-def-gcc \
    type-checker \
    type-checker-list \
    type-cases \
    type-path \
    trie \
    bugs
do
    r=($(./test-$t.sh -w87 -c ${q:+-q}))
    (( p += ${r[0]} ))
    (( f += ${r[1]} ))
done

[ -z "$q" -o "$f" -gt 0 ] &&
echo >&2

echo "test cases passed: $p"
echo "test cases failed: $f"

exit $(( f != 0 ))

