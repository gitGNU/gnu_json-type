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
# $ json-gentest -C type-cases:31
#

[[ "$1" =~ ^-u[0-9]+$ ]] &&
u="${1:2}" ||
u=""

diff -u$u -L 31.old <(echo \
'$ json() { set -o pipefail && LD_LIBRARY_PATH=../lib ../src/json --terse --verbose --literal-value "$@" 2>&1|sed -r '\''/error:/s|(/dev/fd/)[0-9]+|\1??|'\''; }
$ json -d '\''{"type":"list","args":[{"type":"array","args":["null","null","boolean"]},{"type":"array","args":["null","null","number"]},{"type":"array","args":["null","null","string"]}]}'\'' <<< '\''0'\''
json: error: <stdin>:1:1: type check error: type mismatch: expected a value of type `{"type":"array",...}'\''
json: error: <stdin>:1:1: 0
json: error: <stdin>:1:1: ^
command failed: json -d '\''{"type":"list","args":[{"type":"array","args":["null","null","boolean"]},{"type":"array","args":["null","null","number"]},{"type":"array","args":["null","null","string"]}]}'\'' <<< '\''0'\''
$ json -d '\''{"type":"list","args":[{"type":"array","args":["null","null","boolean"]},{"type":"array","args":["null","null","number"]},{"type":"array","args":["null","null","string"]}]}'\'' <<< '\''[0]'\''
json: error: <stdin>:1:2: type check error: type mismatch: expected a value of type `"null"'\''
json: error: <stdin>:1:2: [0]
json: error: <stdin>:1:2:  ^
[
command failed: json -d '\''{"type":"list","args":[{"type":"array","args":["null","null","boolean"]},{"type":"array","args":["null","null","number"]},{"type":"array","args":["null","null","string"]}]}'\'' <<< '\''[0]'\''
$ json -d '\''{"type":"list","args":[{"type":"array","args":["null","null","boolean"]},{"type":"array","args":["null","null","number"]},{"type":"array","args":["null","null","string"]}]}'\'' <<< '\''[null]'\''
json: error: <stdin>:1:6: type check error: too few arguments
json: error: <stdin>:1:6: [null]
json: error: <stdin>:1:6:      ^
[null
command failed: json -d '\''{"type":"list","args":[{"type":"array","args":["null","null","boolean"]},{"type":"array","args":["null","null","number"]},{"type":"array","args":["null","null","string"]}]}'\'' <<< '\''[null]'\''
$ json -d '\''{"type":"list","args":[{"type":"array","args":["null","null","boolean"]},{"type":"array","args":["null","null","number"]},{"type":"array","args":["null","null","string"]}]}'\'' <<< '\''[null,0]'\''
json: error: <stdin>:1:7: type check error: type mismatch: expected a value of type `"null"'\''
json: error: <stdin>:1:7: [null,0]
json: error: <stdin>:1:7:       ^
[null,
command failed: json -d '\''{"type":"list","args":[{"type":"array","args":["null","null","boolean"]},{"type":"array","args":["null","null","number"]},{"type":"array","args":["null","null","string"]}]}'\'' <<< '\''[null,0]'\''
$ json -d '\''{"type":"list","args":[{"type":"array","args":["null","null","boolean"]},{"type":"array","args":["null","null","number"]},{"type":"array","args":["null","null","string"]}]}'\'' <<< '\''[null,null]'\''
json: error: <stdin>:1:11: type check error: too few arguments
json: error: <stdin>:1:11: [null,null]
json: error: <stdin>:1:11:           ^
[null,null
command failed: json -d '\''{"type":"list","args":[{"type":"array","args":["null","null","boolean"]},{"type":"array","args":["null","null","number"]},{"type":"array","args":["null","null","string"]}]}'\'' <<< '\''[null,null]'\''
$ json -d '\''{"type":"list","args":[{"type":"array","args":["null","null","boolean"]},{"type":"array","args":["null","null","number"]},{"type":"array","args":["null","null","string"]}]}'\'' <<< '\''[null,null,null]'\''
json: error: <stdin>:1:12: type check error: type mismatch: expected a value of type `"boolean"'\'', `"number"'\'' or `"string"'\''
json: error: <stdin>:1:12: [null,null,null]
json: error: <stdin>:1:12:            ^
[null,null,
command failed: json -d '\''{"type":"list","args":[{"type":"array","args":["null","null","boolean"]},{"type":"array","args":["null","null","number"]},{"type":"array","args":["null","null","string"]}]}'\'' <<< '\''[null,null,null]'\''
$ json -d '\''{"type":"list","args":[{"type":"array","args":["null","null","boolean"]},{"type":"array","args":["null","null","number"]},{"type":"array","args":["null","null","string"]}]}'\'' <<< '\''[null,null,false]'\''
[null,null,false]
$ json -d '\''{"type":"list","args":[{"type":"array","args":["null","null","boolean"]},{"type":"array","args":["null","null","number"]},{"type":"array","args":["null","null","string"]}]}'\'' <<< '\''[null,null,0]'\''
[null,null,0]
$ json -d '\''{"type":"list","args":[{"type":"array","args":["null","null","boolean"]},{"type":"array","args":["null","null","number"]},{"type":"array","args":["null","null","string"]}]}'\'' <<< '\''[null,null,""]'\''
[null,null,""]
$ json -d '\''{"type":"list","args":[{"type":"array","args":["null","null","boolean"]},{"type":"array","args":["null","null","number"]},{"type":"array","args":["null","null","string"]}]}'\'' <<< '\''[null,null,false,null]'\''
json: error: <stdin>:1:17: type check error: too many arguments
json: error: <stdin>:1:17: [null,null,false,null]
json: error: <stdin>:1:17:                 ^
[null,null,false
command failed: json -d '\''{"type":"list","args":[{"type":"array","args":["null","null","boolean"]},{"type":"array","args":["null","null","number"]},{"type":"array","args":["null","null","string"]}]}'\'' <<< '\''[null,null,false,null]'\''
$ json -d '\''{"type":"list","args":[{"type":"array","args":["null","null","boolean"]},{"type":"array","args":["null","null","number"]},{"type":"array","args":["null","null","string"]}]}'\'' <<< '\''[null,null,0,null]'\''
json: error: <stdin>:1:13: type check error: too many arguments
json: error: <stdin>:1:13: [null,null,0,null]
json: error: <stdin>:1:13:             ^
[null,null,0
command failed: json -d '\''{"type":"list","args":[{"type":"array","args":["null","null","boolean"]},{"type":"array","args":["null","null","number"]},{"type":"array","args":["null","null","string"]}]}'\'' <<< '\''[null,null,0,null]'\''
$ json -d '\''{"type":"list","args":[{"type":"array","args":["null","null","boolean"]},{"type":"array","args":["null","null","number"]},{"type":"array","args":["null","null","string"]}]}'\'' <<< '\''[null,null,"",null]'\''
json: error: <stdin>:1:14: type check error: too many arguments
json: error: <stdin>:1:14: [null,null,"",null]
json: error: <stdin>:1:14:              ^
[null,null,""
command failed: json -d '\''{"type":"list","args":[{"type":"array","args":["null","null","boolean"]},{"type":"array","args":["null","null","number"]},{"type":"array","args":["null","null","string"]}]}'\'' <<< '\''[null,null,"",null]'\'''
) -L 31.new <(
echo '$ json() { set -o pipefail && LD_LIBRARY_PATH=../lib ../src/json --terse --verbose --literal-value "$@" 2>&1|sed -r '\''/error:/s|(/dev/fd/)[0-9]+|\1??|'\''; }'
json() { set -o pipefail && LD_LIBRARY_PATH=../lib ../src/json --terse --verbose --literal-value "$@" 2>&1|sed -r '/error:/s|(/dev/fd/)[0-9]+|\1??|'; } 2>&1 ||
echo 'command failed: json() { set -o pipefail && LD_LIBRARY_PATH=../lib ../src/json --terse --verbose --literal-value "$@" 2>&1|sed -r '\''/error:/s|(/dev/fd/)[0-9]+|\1??|'\''; }'

echo '$ json -d '\''{"type":"list","args":[{"type":"array","args":["null","null","boolean"]},{"type":"array","args":["null","null","number"]},{"type":"array","args":["null","null","string"]}]}'\'' <<< '\''0'\'''
json -d '{"type":"list","args":[{"type":"array","args":["null","null","boolean"]},{"type":"array","args":["null","null","number"]},{"type":"array","args":["null","null","string"]}]}' <<< '0' 2>&1 ||
echo 'command failed: json -d '\''{"type":"list","args":[{"type":"array","args":["null","null","boolean"]},{"type":"array","args":["null","null","number"]},{"type":"array","args":["null","null","string"]}]}'\'' <<< '\''0'\'''

echo '$ json -d '\''{"type":"list","args":[{"type":"array","args":["null","null","boolean"]},{"type":"array","args":["null","null","number"]},{"type":"array","args":["null","null","string"]}]}'\'' <<< '\''[0]'\'''
json -d '{"type":"list","args":[{"type":"array","args":["null","null","boolean"]},{"type":"array","args":["null","null","number"]},{"type":"array","args":["null","null","string"]}]}' <<< '[0]' 2>&1 ||
echo 'command failed: json -d '\''{"type":"list","args":[{"type":"array","args":["null","null","boolean"]},{"type":"array","args":["null","null","number"]},{"type":"array","args":["null","null","string"]}]}'\'' <<< '\''[0]'\'''

echo '$ json -d '\''{"type":"list","args":[{"type":"array","args":["null","null","boolean"]},{"type":"array","args":["null","null","number"]},{"type":"array","args":["null","null","string"]}]}'\'' <<< '\''[null]'\'''
json -d '{"type":"list","args":[{"type":"array","args":["null","null","boolean"]},{"type":"array","args":["null","null","number"]},{"type":"array","args":["null","null","string"]}]}' <<< '[null]' 2>&1 ||
echo 'command failed: json -d '\''{"type":"list","args":[{"type":"array","args":["null","null","boolean"]},{"type":"array","args":["null","null","number"]},{"type":"array","args":["null","null","string"]}]}'\'' <<< '\''[null]'\'''

echo '$ json -d '\''{"type":"list","args":[{"type":"array","args":["null","null","boolean"]},{"type":"array","args":["null","null","number"]},{"type":"array","args":["null","null","string"]}]}'\'' <<< '\''[null,0]'\'''
json -d '{"type":"list","args":[{"type":"array","args":["null","null","boolean"]},{"type":"array","args":["null","null","number"]},{"type":"array","args":["null","null","string"]}]}' <<< '[null,0]' 2>&1 ||
echo 'command failed: json -d '\''{"type":"list","args":[{"type":"array","args":["null","null","boolean"]},{"type":"array","args":["null","null","number"]},{"type":"array","args":["null","null","string"]}]}'\'' <<< '\''[null,0]'\'''

echo '$ json -d '\''{"type":"list","args":[{"type":"array","args":["null","null","boolean"]},{"type":"array","args":["null","null","number"]},{"type":"array","args":["null","null","string"]}]}'\'' <<< '\''[null,null]'\'''
json -d '{"type":"list","args":[{"type":"array","args":["null","null","boolean"]},{"type":"array","args":["null","null","number"]},{"type":"array","args":["null","null","string"]}]}' <<< '[null,null]' 2>&1 ||
echo 'command failed: json -d '\''{"type":"list","args":[{"type":"array","args":["null","null","boolean"]},{"type":"array","args":["null","null","number"]},{"type":"array","args":["null","null","string"]}]}'\'' <<< '\''[null,null]'\'''

echo '$ json -d '\''{"type":"list","args":[{"type":"array","args":["null","null","boolean"]},{"type":"array","args":["null","null","number"]},{"type":"array","args":["null","null","string"]}]}'\'' <<< '\''[null,null,null]'\'''
json -d '{"type":"list","args":[{"type":"array","args":["null","null","boolean"]},{"type":"array","args":["null","null","number"]},{"type":"array","args":["null","null","string"]}]}' <<< '[null,null,null]' 2>&1 ||
echo 'command failed: json -d '\''{"type":"list","args":[{"type":"array","args":["null","null","boolean"]},{"type":"array","args":["null","null","number"]},{"type":"array","args":["null","null","string"]}]}'\'' <<< '\''[null,null,null]'\'''

echo '$ json -d '\''{"type":"list","args":[{"type":"array","args":["null","null","boolean"]},{"type":"array","args":["null","null","number"]},{"type":"array","args":["null","null","string"]}]}'\'' <<< '\''[null,null,false]'\'''
json -d '{"type":"list","args":[{"type":"array","args":["null","null","boolean"]},{"type":"array","args":["null","null","number"]},{"type":"array","args":["null","null","string"]}]}' <<< '[null,null,false]' 2>&1 ||
echo 'command failed: json -d '\''{"type":"list","args":[{"type":"array","args":["null","null","boolean"]},{"type":"array","args":["null","null","number"]},{"type":"array","args":["null","null","string"]}]}'\'' <<< '\''[null,null,false]'\'''

echo '$ json -d '\''{"type":"list","args":[{"type":"array","args":["null","null","boolean"]},{"type":"array","args":["null","null","number"]},{"type":"array","args":["null","null","string"]}]}'\'' <<< '\''[null,null,0]'\'''
json -d '{"type":"list","args":[{"type":"array","args":["null","null","boolean"]},{"type":"array","args":["null","null","number"]},{"type":"array","args":["null","null","string"]}]}' <<< '[null,null,0]' 2>&1 ||
echo 'command failed: json -d '\''{"type":"list","args":[{"type":"array","args":["null","null","boolean"]},{"type":"array","args":["null","null","number"]},{"type":"array","args":["null","null","string"]}]}'\'' <<< '\''[null,null,0]'\'''

echo '$ json -d '\''{"type":"list","args":[{"type":"array","args":["null","null","boolean"]},{"type":"array","args":["null","null","number"]},{"type":"array","args":["null","null","string"]}]}'\'' <<< '\''[null,null,""]'\'''
json -d '{"type":"list","args":[{"type":"array","args":["null","null","boolean"]},{"type":"array","args":["null","null","number"]},{"type":"array","args":["null","null","string"]}]}' <<< '[null,null,""]' 2>&1 ||
echo 'command failed: json -d '\''{"type":"list","args":[{"type":"array","args":["null","null","boolean"]},{"type":"array","args":["null","null","number"]},{"type":"array","args":["null","null","string"]}]}'\'' <<< '\''[null,null,""]'\'''

echo '$ json -d '\''{"type":"list","args":[{"type":"array","args":["null","null","boolean"]},{"type":"array","args":["null","null","number"]},{"type":"array","args":["null","null","string"]}]}'\'' <<< '\''[null,null,false,null]'\'''
json -d '{"type":"list","args":[{"type":"array","args":["null","null","boolean"]},{"type":"array","args":["null","null","number"]},{"type":"array","args":["null","null","string"]}]}' <<< '[null,null,false,null]' 2>&1 ||
echo 'command failed: json -d '\''{"type":"list","args":[{"type":"array","args":["null","null","boolean"]},{"type":"array","args":["null","null","number"]},{"type":"array","args":["null","null","string"]}]}'\'' <<< '\''[null,null,false,null]'\'''

echo '$ json -d '\''{"type":"list","args":[{"type":"array","args":["null","null","boolean"]},{"type":"array","args":["null","null","number"]},{"type":"array","args":["null","null","string"]}]}'\'' <<< '\''[null,null,0,null]'\'''
json -d '{"type":"list","args":[{"type":"array","args":["null","null","boolean"]},{"type":"array","args":["null","null","number"]},{"type":"array","args":["null","null","string"]}]}' <<< '[null,null,0,null]' 2>&1 ||
echo 'command failed: json -d '\''{"type":"list","args":[{"type":"array","args":["null","null","boolean"]},{"type":"array","args":["null","null","number"]},{"type":"array","args":["null","null","string"]}]}'\'' <<< '\''[null,null,0,null]'\'''

echo '$ json -d '\''{"type":"list","args":[{"type":"array","args":["null","null","boolean"]},{"type":"array","args":["null","null","number"]},{"type":"array","args":["null","null","string"]}]}'\'' <<< '\''[null,null,"",null]'\'''
json -d '{"type":"list","args":[{"type":"array","args":["null","null","boolean"]},{"type":"array","args":["null","null","number"]},{"type":"array","args":["null","null","string"]}]}' <<< '[null,null,"",null]' 2>&1 ||
echo 'command failed: json -d '\''{"type":"list","args":[{"type":"array","args":["null","null","boolean"]},{"type":"array","args":["null","null","number"]},{"type":"array","args":["null","null","string"]}]}'\'' <<< '\''[null,null,"",null]'\'''
)

