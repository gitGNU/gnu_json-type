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
# $ json-gentest -C type-checker:closed-array-dict
#

[[ "$1" =~ ^-u[0-9]+$ ]] &&
u="${1:2}" ||
u=""

diff -u$u -L closed-array-dict.old <(echo \
'$ export JSON_TYPE_CHECK_ERROR_COMPLETE_TYPES=yes
$ json() { LD_LIBRARY_PATH=../lib ../src/json --terse --verbose --literal-value "$@"; }
$ json -d '\''{"type":"array","args":[{"type":"dict","args":[]}]}'\'' <<< '\''[null]'\''
json: error: <stdin>:1:2: type check error: type mismatch: expected a value of type `{"type":"dict","args":[]}'\''
json: error: <stdin>:1:2: [null]
json: error: <stdin>:1:2:  ^
[
command failed: json -d '\''{"type":"array","args":[{"type":"dict","args":[]}]}'\'' <<< '\''[null]'\''
$ json -d '\''{"type":"array","args":[{"type":"dict","args":[]}]}'\'' <<< '\''[false]'\''
json: error: <stdin>:1:2: type check error: type mismatch: expected a value of type `{"type":"dict","args":[]}'\''
json: error: <stdin>:1:2: [false]
json: error: <stdin>:1:2:  ^
[
command failed: json -d '\''{"type":"array","args":[{"type":"dict","args":[]}]}'\'' <<< '\''[false]'\''
$ json -d '\''{"type":"array","args":[{"type":"dict","args":[]}]}'\'' <<< '\''[true]'\''
json: error: <stdin>:1:2: type check error: type mismatch: expected a value of type `{"type":"dict","args":[]}'\''
json: error: <stdin>:1:2: [true]
json: error: <stdin>:1:2:  ^
[
command failed: json -d '\''{"type":"array","args":[{"type":"dict","args":[]}]}'\'' <<< '\''[true]'\''
$ json -d '\''{"type":"array","args":[{"type":"dict","args":[]}]}'\'' <<< '\''[123]'\''
json: error: <stdin>:1:2: type check error: type mismatch: expected a value of type `{"type":"dict","args":[]}'\''
json: error: <stdin>:1:2: [123]
json: error: <stdin>:1:2:  ^
[
command failed: json -d '\''{"type":"array","args":[{"type":"dict","args":[]}]}'\'' <<< '\''[123]'\''
$ json -d '\''{"type":"array","args":[{"type":"dict","args":[]}]}'\'' <<< '\''["foo"]'\''
json: error: <stdin>:1:2: type check error: type mismatch: expected a value of type `{"type":"dict","args":[]}'\''
json: error: <stdin>:1:2: ["foo"]
json: error: <stdin>:1:2:  ^
[
command failed: json -d '\''{"type":"array","args":[{"type":"dict","args":[]}]}'\'' <<< '\''["foo"]'\''
$ json -d '\''{"type":"array","args":[{"type":"dict","args":[]}]}'\'' <<< '\''[{}]'\''
[{}]
$ json -d '\''{"type":"array","args":[{"type":"dict","args":[]}]}'\'' <<< '\''[{"foo":1}]'\''
json: error: <stdin>:1:3: type check error: too many arguments
json: error: <stdin>:1:3: [{"foo":1}]
json: error: <stdin>:1:3:   ^
[{
command failed: json -d '\''{"type":"array","args":[{"type":"dict","args":[]}]}'\'' <<< '\''[{"foo":1}]'\''
$ json -d '\''{"type":"array","args":[{"type":"dict","args":[]}]}'\'' <<< '\''[{"foo":1,"bar":2}]'\''
json: error: <stdin>:1:3: type check error: too many arguments
json: error: <stdin>:1:3: [{"foo":1,"bar":2}]
json: error: <stdin>:1:3:   ^
[{
command failed: json -d '\''{"type":"array","args":[{"type":"dict","args":[]}]}'\'' <<< '\''[{"foo":1,"bar":2}]'\''
$ json -d '\''{"type":"array","args":[{"type":"dict","args":[]}]}'\'' <<< '\''[{"foo":1,"bar":2,"baz":3}]'\''
json: error: <stdin>:1:3: type check error: too many arguments
json: error: <stdin>:1:3: [{"foo":1,"bar":2,"baz":3}]
json: error: <stdin>:1:3:   ^
[{
command failed: json -d '\''{"type":"array","args":[{"type":"dict","args":[]}]}'\'' <<< '\''[{"foo":1,"bar":2,"baz":3}]'\''
$ json -d '\''{"type":"array","args":[{"type":"dict","args":[]}]}'\'' <<< '\''[[]]'\''
json: error: <stdin>:1:2: type check error: type mismatch: expected a value of type `{"type":"dict","args":[]}'\''
json: error: <stdin>:1:2: [[]]
json: error: <stdin>:1:2:  ^
[
command failed: json -d '\''{"type":"array","args":[{"type":"dict","args":[]}]}'\'' <<< '\''[[]]'\'''
) -L closed-array-dict.new <(
echo '$ export JSON_TYPE_CHECK_ERROR_COMPLETE_TYPES=yes'
export JSON_TYPE_CHECK_ERROR_COMPLETE_TYPES=yes 2>&1 ||
echo 'command failed: export JSON_TYPE_CHECK_ERROR_COMPLETE_TYPES=yes'

echo '$ json() { LD_LIBRARY_PATH=../lib ../src/json --terse --verbose --literal-value "$@"; }'
json() { LD_LIBRARY_PATH=../lib ../src/json --terse --verbose --literal-value "$@"; } 2>&1 ||
echo 'command failed: json() { LD_LIBRARY_PATH=../lib ../src/json --terse --verbose --literal-value "$@"; }'

echo '$ json -d '\''{"type":"array","args":[{"type":"dict","args":[]}]}'\'' <<< '\''[null]'\'''
json -d '{"type":"array","args":[{"type":"dict","args":[]}]}' <<< '[null]' 2>&1 ||
echo 'command failed: json -d '\''{"type":"array","args":[{"type":"dict","args":[]}]}'\'' <<< '\''[null]'\'''

echo '$ json -d '\''{"type":"array","args":[{"type":"dict","args":[]}]}'\'' <<< '\''[false]'\'''
json -d '{"type":"array","args":[{"type":"dict","args":[]}]}' <<< '[false]' 2>&1 ||
echo 'command failed: json -d '\''{"type":"array","args":[{"type":"dict","args":[]}]}'\'' <<< '\''[false]'\'''

echo '$ json -d '\''{"type":"array","args":[{"type":"dict","args":[]}]}'\'' <<< '\''[true]'\'''
json -d '{"type":"array","args":[{"type":"dict","args":[]}]}' <<< '[true]' 2>&1 ||
echo 'command failed: json -d '\''{"type":"array","args":[{"type":"dict","args":[]}]}'\'' <<< '\''[true]'\'''

echo '$ json -d '\''{"type":"array","args":[{"type":"dict","args":[]}]}'\'' <<< '\''[123]'\'''
json -d '{"type":"array","args":[{"type":"dict","args":[]}]}' <<< '[123]' 2>&1 ||
echo 'command failed: json -d '\''{"type":"array","args":[{"type":"dict","args":[]}]}'\'' <<< '\''[123]'\'''

echo '$ json -d '\''{"type":"array","args":[{"type":"dict","args":[]}]}'\'' <<< '\''["foo"]'\'''
json -d '{"type":"array","args":[{"type":"dict","args":[]}]}' <<< '["foo"]' 2>&1 ||
echo 'command failed: json -d '\''{"type":"array","args":[{"type":"dict","args":[]}]}'\'' <<< '\''["foo"]'\'''

echo '$ json -d '\''{"type":"array","args":[{"type":"dict","args":[]}]}'\'' <<< '\''[{}]'\'''
json -d '{"type":"array","args":[{"type":"dict","args":[]}]}' <<< '[{}]' 2>&1 ||
echo 'command failed: json -d '\''{"type":"array","args":[{"type":"dict","args":[]}]}'\'' <<< '\''[{}]'\'''

echo '$ json -d '\''{"type":"array","args":[{"type":"dict","args":[]}]}'\'' <<< '\''[{"foo":1}]'\'''
json -d '{"type":"array","args":[{"type":"dict","args":[]}]}' <<< '[{"foo":1}]' 2>&1 ||
echo 'command failed: json -d '\''{"type":"array","args":[{"type":"dict","args":[]}]}'\'' <<< '\''[{"foo":1}]'\'''

echo '$ json -d '\''{"type":"array","args":[{"type":"dict","args":[]}]}'\'' <<< '\''[{"foo":1,"bar":2}]'\'''
json -d '{"type":"array","args":[{"type":"dict","args":[]}]}' <<< '[{"foo":1,"bar":2}]' 2>&1 ||
echo 'command failed: json -d '\''{"type":"array","args":[{"type":"dict","args":[]}]}'\'' <<< '\''[{"foo":1,"bar":2}]'\'''

echo '$ json -d '\''{"type":"array","args":[{"type":"dict","args":[]}]}'\'' <<< '\''[{"foo":1,"bar":2,"baz":3}]'\'''
json -d '{"type":"array","args":[{"type":"dict","args":[]}]}' <<< '[{"foo":1,"bar":2,"baz":3}]' 2>&1 ||
echo 'command failed: json -d '\''{"type":"array","args":[{"type":"dict","args":[]}]}'\'' <<< '\''[{"foo":1,"bar":2,"baz":3}]'\'''

echo '$ json -d '\''{"type":"array","args":[{"type":"dict","args":[]}]}'\'' <<< '\''[[]]'\'''
json -d '{"type":"array","args":[{"type":"dict","args":[]}]}' <<< '[[]]' 2>&1 ||
echo 'command failed: json -d '\''{"type":"array","args":[{"type":"dict","args":[]}]}'\'' <<< '\''[[]]'\'''
)

