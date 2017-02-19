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
# $ json-gentest -w7 -G bugs
#

w=7
c=""
q=""

if [[ "$1" =~ ^-w[1-9][0-9]*$ ]]; then
    w="${1:2}"
    shift
fi
if [ "$1" == "-c" ]; then
    c="c"
    shift
fi
if [ "$1" == "-q" ]; then
    q="q"
fi

p=0
f=0

for t in \
    0 \
    1 \
    2 \
    3 \
    4 \
    5 \
    6 \
    7 \
    8 \
    9 \
    10 \
    11 \
    12 \
    13 \
    14 \
    15 \
    16 \
    17 \
    18 \
    19 \
    20 \
    21 \
    22 \
    23 \
    24 \
    25 \
    26 \
    27 \
    28 \
    29 \
    30 \
    31 \
    32 \
    33
do
    test -z "$q" &&
    printf >&2 "%-$((w + 6))s " "test: bugs:$t"
    if ! bugs/test-$t.sh &>/dev/null; then
        (( f ++ ))

        test -n "$q" &&
        echo >&2 -n "test: bugs:$t "
        echo >&2 failed
    else
        (( p ++ ))

        test -z "$q" &&
        echo >&2 OK
    fi
done

if [ -z "$c" ]; then
    [ -z "$q" -o "$f" -gt 0 ] &&
    echo >&2
    echo "tests passed: $p"
    echo "tests failed: $f"
else
    echo "$p"
    echo "$f"
fi

exit $(( r != 0 ))

