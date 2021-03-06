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
# $ json-gentest -C type-cases:55
#

[[ "$1" =~ ^-u[0-9]+$ ]] &&
u="${1:2}" ||
u=""

diff -u$u -L 55.old <(echo \
'$ json() { set -o pipefail && LD_LIBRARY_PATH=../lib ../src/json --terse --verbose --literal-value "$@" 2>&1|sed -r '\''/error:/s|(/dev/fd/)[0-9]+|\1??|'\''; }
$ json -d '\''{"type":"list","args":[{"type":"array","args":{"type":"array","args":"string"}},"boolean"]}'\'' <<< '\''null'\''
json: error: <stdin>:1:1: type check error: type mismatch: expected a value of type `"boolean"'\'' or `{"type":"array",...}'\''
json: error: <stdin>:1:1: null
json: error: <stdin>:1:1: ^
command failed: json -d '\''{"type":"list","args":[{"type":"array","args":{"type":"array","args":"string"}},"boolean"]}'\'' <<< '\''null'\''
$ json -d '\''{"type":"list","args":[{"type":"array","args":{"type":"array","args":"string"}},"boolean"]}'\'' <<< '\''false'\''
false
$ json -d '\''{"type":"list","args":[{"type":"array","args":{"type":"array","args":"string"}},"boolean"]}'\'' <<< '\''[]'\''
[]
$ json -d '\''{"type":"list","args":[{"type":"array","args":{"type":"array","args":"string"}},"boolean"]}'\'' <<< '\''[[null]]'\''
json: error: <stdin>:1:3: type check error: type mismatch: expected a value of type `"string"'\''
json: error: <stdin>:1:3: [[null]]
json: error: <stdin>:1:3:   ^
[[
command failed: json -d '\''{"type":"list","args":[{"type":"array","args":{"type":"array","args":"string"}},"boolean"]}'\'' <<< '\''[[null]]'\''
$ json -d '\''{"type":"list","args":[{"type":"array","args":{"type":"array","args":"string"}},"boolean"]}'\'' <<< '\''[["a"]]'\''
[["a"]]
$ json -d '\''{"type":"list","args":[{"type":"array","args":{"type":"array","args":"string"}},"boolean"]}'\'' <<< '\''[["a",null]]'\''
json: error: <stdin>:1:7: type check error: type mismatch: expected a value of type `"string"'\''
json: error: <stdin>:1:7: [["a",null]]
json: error: <stdin>:1:7:       ^
[["a",
command failed: json -d '\''{"type":"list","args":[{"type":"array","args":{"type":"array","args":"string"}},"boolean"]}'\'' <<< '\''[["a",null]]'\''
$ json -d '\''{"type":"list","args":[{"type":"array","args":{"type":"array","args":"string"}},"boolean"]}'\'' <<< '\''[["a","b"]]'\''
[["a","b"]]
$ json -d '\''{"type":"list","args":[{"type":"array","args":{"type":"array","args":"string"}},"boolean"]}'\'' <<< '\''[["a","b",null]]'\''
json: error: <stdin>:1:11: type check error: type mismatch: expected a value of type `"string"'\''
json: error: <stdin>:1:11: [["a","b",null]]
json: error: <stdin>:1:11:           ^
[["a","b",
command failed: json -d '\''{"type":"list","args":[{"type":"array","args":{"type":"array","args":"string"}},"boolean"]}'\'' <<< '\''[["a","b",null]]'\''
$ json -d '\''{"type":"list","args":[{"type":"array","args":{"type":"array","args":"string"}},"boolean"]}'\'' <<< '\''[["a","b","c"]]'\''
[["a","b","c"]]'
) -L 55.new <(
echo '$ json() { set -o pipefail && LD_LIBRARY_PATH=../lib ../src/json --terse --verbose --literal-value "$@" 2>&1|sed -r '\''/error:/s|(/dev/fd/)[0-9]+|\1??|'\''; }'
json() { set -o pipefail && LD_LIBRARY_PATH=../lib ../src/json --terse --verbose --literal-value "$@" 2>&1|sed -r '/error:/s|(/dev/fd/)[0-9]+|\1??|'; } 2>&1 ||
echo 'command failed: json() { set -o pipefail && LD_LIBRARY_PATH=../lib ../src/json --terse --verbose --literal-value "$@" 2>&1|sed -r '\''/error:/s|(/dev/fd/)[0-9]+|\1??|'\''; }'

echo '$ json -d '\''{"type":"list","args":[{"type":"array","args":{"type":"array","args":"string"}},"boolean"]}'\'' <<< '\''null'\'''
json -d '{"type":"list","args":[{"type":"array","args":{"type":"array","args":"string"}},"boolean"]}' <<< 'null' 2>&1 ||
echo 'command failed: json -d '\''{"type":"list","args":[{"type":"array","args":{"type":"array","args":"string"}},"boolean"]}'\'' <<< '\''null'\'''

echo '$ json -d '\''{"type":"list","args":[{"type":"array","args":{"type":"array","args":"string"}},"boolean"]}'\'' <<< '\''false'\'''
json -d '{"type":"list","args":[{"type":"array","args":{"type":"array","args":"string"}},"boolean"]}' <<< 'false' 2>&1 ||
echo 'command failed: json -d '\''{"type":"list","args":[{"type":"array","args":{"type":"array","args":"string"}},"boolean"]}'\'' <<< '\''false'\'''

echo '$ json -d '\''{"type":"list","args":[{"type":"array","args":{"type":"array","args":"string"}},"boolean"]}'\'' <<< '\''[]'\'''
json -d '{"type":"list","args":[{"type":"array","args":{"type":"array","args":"string"}},"boolean"]}' <<< '[]' 2>&1 ||
echo 'command failed: json -d '\''{"type":"list","args":[{"type":"array","args":{"type":"array","args":"string"}},"boolean"]}'\'' <<< '\''[]'\'''

echo '$ json -d '\''{"type":"list","args":[{"type":"array","args":{"type":"array","args":"string"}},"boolean"]}'\'' <<< '\''[[null]]'\'''
json -d '{"type":"list","args":[{"type":"array","args":{"type":"array","args":"string"}},"boolean"]}' <<< '[[null]]' 2>&1 ||
echo 'command failed: json -d '\''{"type":"list","args":[{"type":"array","args":{"type":"array","args":"string"}},"boolean"]}'\'' <<< '\''[[null]]'\'''

echo '$ json -d '\''{"type":"list","args":[{"type":"array","args":{"type":"array","args":"string"}},"boolean"]}'\'' <<< '\''[["a"]]'\'''
json -d '{"type":"list","args":[{"type":"array","args":{"type":"array","args":"string"}},"boolean"]}' <<< '[["a"]]' 2>&1 ||
echo 'command failed: json -d '\''{"type":"list","args":[{"type":"array","args":{"type":"array","args":"string"}},"boolean"]}'\'' <<< '\''[["a"]]'\'''

echo '$ json -d '\''{"type":"list","args":[{"type":"array","args":{"type":"array","args":"string"}},"boolean"]}'\'' <<< '\''[["a",null]]'\'''
json -d '{"type":"list","args":[{"type":"array","args":{"type":"array","args":"string"}},"boolean"]}' <<< '[["a",null]]' 2>&1 ||
echo 'command failed: json -d '\''{"type":"list","args":[{"type":"array","args":{"type":"array","args":"string"}},"boolean"]}'\'' <<< '\''[["a",null]]'\'''

echo '$ json -d '\''{"type":"list","args":[{"type":"array","args":{"type":"array","args":"string"}},"boolean"]}'\'' <<< '\''[["a","b"]]'\'''
json -d '{"type":"list","args":[{"type":"array","args":{"type":"array","args":"string"}},"boolean"]}' <<< '[["a","b"]]' 2>&1 ||
echo 'command failed: json -d '\''{"type":"list","args":[{"type":"array","args":{"type":"array","args":"string"}},"boolean"]}'\'' <<< '\''[["a","b"]]'\'''

echo '$ json -d '\''{"type":"list","args":[{"type":"array","args":{"type":"array","args":"string"}},"boolean"]}'\'' <<< '\''[["a","b",null]]'\'''
json -d '{"type":"list","args":[{"type":"array","args":{"type":"array","args":"string"}},"boolean"]}' <<< '[["a","b",null]]' 2>&1 ||
echo 'command failed: json -d '\''{"type":"list","args":[{"type":"array","args":{"type":"array","args":"string"}},"boolean"]}'\'' <<< '\''[["a","b",null]]'\'''

echo '$ json -d '\''{"type":"list","args":[{"type":"array","args":{"type":"array","args":"string"}},"boolean"]}'\'' <<< '\''[["a","b","c"]]'\'''
json -d '{"type":"list","args":[{"type":"array","args":{"type":"array","args":"string"}},"boolean"]}' <<< '[["a","b","c"]]' 2>&1 ||
echo 'command failed: json -d '\''{"type":"list","args":[{"type":"array","args":{"type":"array","args":"string"}},"boolean"]}'\'' <<< '\''[["a","b","c"]]'\'''
)

