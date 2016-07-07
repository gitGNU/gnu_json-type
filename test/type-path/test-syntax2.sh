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
# $ json-gentest -C type-path:syntax2
#

[[ "$1" =~ ^-u[0-9]+$ ]] &&
u="${1:2}" ||
u=""

diff -u$u -L syntax2.old <(echo \
'$ json() { set -o pipefail && LD_LIBRARY_PATH=../lib ../src/json --literal-value -e 64 -V -Ta "$@"|LD_LIBRARY_PATH=../lib ../src/json --from-ast-print --verbose 2>/dev/null; }
$ json <<< '\''"thiss"'\''
json: error: <stdin>:1:6: meta error: invalid type path: invalid char
json: error: <stdin>:1:6: "thiss"
json: error: <stdin>:1:6:      ^
command failed: json <<< '\''"thiss"'\''
$ json <<< '\''"thisthis"'\''
json: error: <stdin>:1:6: meta error: invalid type path: invalid char
json: error: <stdin>:1:6: "thisthis"
json: error: <stdin>:1:6:      ^
command failed: json <<< '\''"thisthis"'\''
$ json <<< '\''"this "'\''
json: error: <stdin>:1:6: meta error: invalid type path: invalid char
json: error: <stdin>:1:6: "this "
json: error: <stdin>:1:6:      ^
command failed: json <<< '\''"this "'\''
$ json <<< '\''"this\n"'\''
json: error: <stdin>:1:6: meta error: invalid type path: invalid char
json: error: <stdin>:1:6: "this\\n"
json: error: <stdin>:1:6:      ^
command failed: json <<< '\''"this\n"'\''
$ json <<< '\''"this$"'\''
json: error: <stdin>:1:6: meta error: invalid type path: unexpected char
json: error: <stdin>:1:6: "this$"
json: error: <stdin>:1:6:      ^
command failed: json <<< '\''"this$"'\''
$ json <<< '\''"this."'\''
json: error: <stdin>:1:7: meta error: invalid type path: expected key name
json: error: <stdin>:1:7: "this."
json: error: <stdin>:1:7:       ^
command failed: json <<< '\''"this."'\''
$ json <<< '\''"this["'\''
json: error: <stdin>:1:7: meta error: invalid type path: expected number
json: error: <stdin>:1:7: "this["
json: error: <stdin>:1:7:       ^
command failed: json <<< '\''"this["'\''
$ json <<< '\''"this]"'\''
json: error: <stdin>:1:6: meta error: invalid type path: unexpected char
json: error: <stdin>:1:6: "this]"
json: error: <stdin>:1:6:      ^
command failed: json <<< '\''"this]"'\'''
) -L syntax2.new <(
echo '$ json() { set -o pipefail && LD_LIBRARY_PATH=../lib ../src/json --literal-value -e 64 -V -Ta "$@"|LD_LIBRARY_PATH=../lib ../src/json --from-ast-print --verbose 2>/dev/null; }'
json() { set -o pipefail && LD_LIBRARY_PATH=../lib ../src/json --literal-value -e 64 -V -Ta "$@"|LD_LIBRARY_PATH=../lib ../src/json --from-ast-print --verbose 2>/dev/null; } 2>&1 ||
echo 'command failed: json() { set -o pipefail && LD_LIBRARY_PATH=../lib ../src/json --literal-value -e 64 -V -Ta "$@"|LD_LIBRARY_PATH=../lib ../src/json --from-ast-print --verbose 2>/dev/null; }'

echo '$ json <<< '\''"thiss"'\'''
json <<< '"thiss"' 2>&1 ||
echo 'command failed: json <<< '\''"thiss"'\'''

echo '$ json <<< '\''"thisthis"'\'''
json <<< '"thisthis"' 2>&1 ||
echo 'command failed: json <<< '\''"thisthis"'\'''

echo '$ json <<< '\''"this "'\'''
json <<< '"this "' 2>&1 ||
echo 'command failed: json <<< '\''"this "'\'''

echo '$ json <<< '\''"this\n"'\'''
json <<< '"this\n"' 2>&1 ||
echo 'command failed: json <<< '\''"this\n"'\'''

echo '$ json <<< '\''"this$"'\'''
json <<< '"this$"' 2>&1 ||
echo 'command failed: json <<< '\''"this$"'\'''

echo '$ json <<< '\''"this."'\'''
json <<< '"this."' 2>&1 ||
echo 'command failed: json <<< '\''"this."'\'''

echo '$ json <<< '\''"this["'\'''
json <<< '"this["' 2>&1 ||
echo 'command failed: json <<< '\''"this["'\'''

echo '$ json <<< '\''"this]"'\'''
json <<< '"this]"' 2>&1 ||
echo 'command failed: json <<< '\''"this]"'\'''
)

