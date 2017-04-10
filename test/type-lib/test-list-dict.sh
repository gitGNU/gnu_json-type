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
# $ json-gentest -C type-lib:list-dict
#

[[ "$1" =~ ^-u[0-9]+$ ]] &&
u="${1:2}" ||
u=""

diff -u$u -L list-dict.old <(echo \
'$ json() { set -o pipefail && LD_LIBRARY_PATH=../lib ../src/json --literal-value -V -TA "$@"|LD_LIBRARY_PATH=../lib ../src/json --from-ast-print --verbose --no-error; }
$ json <<< '\''{"type":"list","args":[{"type":"dict","args":[]}]}'\''
json: error: <stdin>:1:24: attribute error: invalid "list" type object: element is a "dict" (the "list" begins at 1:1)
json: error: <stdin>:1:24: {"type":"list","args":[{"type":"dict","args":[]}]}
json: error: <stdin>:1:24:                        ^
command failed: json <<< '\''{"type":"list","args":[{"type":"dict","args":[]}]}'\''
$ json <<< '\''{"type":"list","args":[{"type":"dict","args":[{"name":"foo","type":{"type":"dict","args":[]}}]}]}'\''
json: error: <stdin>:1:24: attribute error: invalid "list" type object: element is a "dict" (the "list" begins at 1:1)
json: error: <stdin>:1:24: {"type":"list","args":[{"type":"dict","args":[{"name":"f
json: error: <stdin>:1:24:                        ^
command failed: json <<< '\''{"type":"list","args":[{"type":"dict","args":[{"name":"foo","type":{"type":"dict","args":[]}}]}]}'\''
$ json <<< '\''{"type":"list","args":["boolean",{"type":"dict","args":[{"name":"foo","type":{"type":"dict","args":[]}}]}]}'\''
json: error: <stdin>:1:34: attribute error: invalid "list" type object: element is a "dict" (the "list" begins at 1:1)
json: error: <stdin>:1:34: "type":"list","args":["boolean",{"type":"dict","args":[{"name":"f
json: error: <stdin>:1:34:                                 ^
command failed: json <<< '\''{"type":"list","args":["boolean",{"type":"dict","args":[{"name":"foo","type":{"type":"dict","args":[]}}]}]}'\''
$ json <<< '\''{"type":"list","args":["boolean",{"type":"dict","args":[]}]}'\''
json: error: <stdin>:1:34: attribute error: invalid "list" type object: element is a "dict" (the "list" begins at 1:1)
json: error: <stdin>:1:34: "type":"list","args":["boolean",{"type":"dict","args":[]}]}
json: error: <stdin>:1:34:                                 ^
command failed: json <<< '\''{"type":"list","args":["boolean",{"type":"dict","args":[]}]}'\''
$ json <<< '\''{"type":"list","args":["boolean",{"type":"array","args":"number"},{"type":"dict","args":[]}]}'\''
json: error: <stdin>:1:67: attribute error: invalid "list" type object: element is a "dict" (the "list" begins at 1:1)
json: error: <stdin>:1:67: "type":"array","args":"number"},{"type":"dict","args":[]}]}
json: error: <stdin>:1:67:                                 ^
command failed: json <<< '\''{"type":"list","args":["boolean",{"type":"array","args":"number"},{"type":"dict","args":[]}]}'\''
$ json <<< '\''{"type":"list","args":["boolean",{"type":"array","args":"number"},{"type":"dict","args":[{"name":"foo","type":{"type":"dict","args":[]}}]}]}'\''
json: error: <stdin>:1:67: attribute error: invalid "list" type object: element is a "dict" (the "list" begins at 1:1)
json: error: <stdin>:1:67: "type":"array","args":"number"},{"type":"dict","args":[{"name":"f
json: error: <stdin>:1:67:                                 ^
command failed: json <<< '\''{"type":"list","args":["boolean",{"type":"array","args":"number"},{"type":"dict","args":[{"name":"foo","type":{"type":"dict","args":[]}}]}]}'\'''
) -L list-dict.new <(
echo '$ json() { set -o pipefail && LD_LIBRARY_PATH=../lib ../src/json --literal-value -V -TA "$@"|LD_LIBRARY_PATH=../lib ../src/json --from-ast-print --verbose --no-error; }'
json() { set -o pipefail && LD_LIBRARY_PATH=../lib ../src/json --literal-value -V -TA "$@"|LD_LIBRARY_PATH=../lib ../src/json --from-ast-print --verbose --no-error; } 2>&1 ||
echo 'command failed: json() { set -o pipefail && LD_LIBRARY_PATH=../lib ../src/json --literal-value -V -TA "$@"|LD_LIBRARY_PATH=../lib ../src/json --from-ast-print --verbose --no-error; }'

echo '$ json <<< '\''{"type":"list","args":[{"type":"dict","args":[]}]}'\'''
json <<< '{"type":"list","args":[{"type":"dict","args":[]}]}' 2>&1 ||
echo 'command failed: json <<< '\''{"type":"list","args":[{"type":"dict","args":[]}]}'\'''

echo '$ json <<< '\''{"type":"list","args":[{"type":"dict","args":[{"name":"foo","type":{"type":"dict","args":[]}}]}]}'\'''
json <<< '{"type":"list","args":[{"type":"dict","args":[{"name":"foo","type":{"type":"dict","args":[]}}]}]}' 2>&1 ||
echo 'command failed: json <<< '\''{"type":"list","args":[{"type":"dict","args":[{"name":"foo","type":{"type":"dict","args":[]}}]}]}'\'''

echo '$ json <<< '\''{"type":"list","args":["boolean",{"type":"dict","args":[{"name":"foo","type":{"type":"dict","args":[]}}]}]}'\'''
json <<< '{"type":"list","args":["boolean",{"type":"dict","args":[{"name":"foo","type":{"type":"dict","args":[]}}]}]}' 2>&1 ||
echo 'command failed: json <<< '\''{"type":"list","args":["boolean",{"type":"dict","args":[{"name":"foo","type":{"type":"dict","args":[]}}]}]}'\'''

echo '$ json <<< '\''{"type":"list","args":["boolean",{"type":"dict","args":[]}]}'\'''
json <<< '{"type":"list","args":["boolean",{"type":"dict","args":[]}]}' 2>&1 ||
echo 'command failed: json <<< '\''{"type":"list","args":["boolean",{"type":"dict","args":[]}]}'\'''

echo '$ json <<< '\''{"type":"list","args":["boolean",{"type":"array","args":"number"},{"type":"dict","args":[]}]}'\'''
json <<< '{"type":"list","args":["boolean",{"type":"array","args":"number"},{"type":"dict","args":[]}]}' 2>&1 ||
echo 'command failed: json <<< '\''{"type":"list","args":["boolean",{"type":"array","args":"number"},{"type":"dict","args":[]}]}'\'''

echo '$ json <<< '\''{"type":"list","args":["boolean",{"type":"array","args":"number"},{"type":"dict","args":[{"name":"foo","type":{"type":"dict","args":[]}}]}]}'\'''
json <<< '{"type":"list","args":["boolean",{"type":"array","args":"number"},{"type":"dict","args":[{"name":"foo","type":{"type":"dict","args":[]}}]}]}' 2>&1 ||
echo 'command failed: json <<< '\''{"type":"list","args":["boolean",{"type":"array","args":"number"},{"type":"dict","args":[{"name":"foo","type":{"type":"dict","args":[]}}]}]}'\'''
)

