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
# $ json-gentest -C type-checker-list:list-closed-array-empty-val-basic
#

[[ "$1" =~ ^-u[0-9]+$ ]] &&
u="${1:2}" ||
u=""

diff -u$u -L list-closed-array-empty-val-basic.old <(echo \
'$ export JSON_TYPE_CHECK_ERROR_COMPLETE_TYPES=yes
$ json() { LD_LIBRARY_PATH=../lib ../src/json --terse --verbose --literal-value "$@"; }
$ json -d '\''{"type":"list","args":[{"type":"array","args":[]}]}'\'' <<< '\''null'\''
json: error: <stdin>:1:1: type check error: type mismatch: expected a value of type `{"type":"list","args":[{"type":"array","args":[]}]}'\''
json: error: <stdin>:1:1: null
json: error: <stdin>:1:1: ^
command failed: json -d '\''{"type":"list","args":[{"type":"array","args":[]}]}'\'' <<< '\''null'\''
$ json -d '\''{"type":"list","args":[{"type":"array","args":[]}]}'\'' <<< '\''false'\''
json: error: <stdin>:1:1: type check error: type mismatch: expected a value of type `{"type":"list","args":[{"type":"array","args":[]}]}'\''
json: error: <stdin>:1:1: false
json: error: <stdin>:1:1: ^
command failed: json -d '\''{"type":"list","args":[{"type":"array","args":[]}]}'\'' <<< '\''false'\''
$ json -d '\''{"type":"list","args":[{"type":"array","args":[]}]}'\'' <<< '\''true'\''
json: error: <stdin>:1:1: type check error: type mismatch: expected a value of type `{"type":"list","args":[{"type":"array","args":[]}]}'\''
json: error: <stdin>:1:1: true
json: error: <stdin>:1:1: ^
command failed: json -d '\''{"type":"list","args":[{"type":"array","args":[]}]}'\'' <<< '\''true'\''
$ json -d '\''{"type":"list","args":[{"type":"array","args":[]}]}'\'' <<< '\''123'\''
json: error: <stdin>:1:1: type check error: type mismatch: expected a value of type `{"type":"list","args":[{"type":"array","args":[]}]}'\''
json: error: <stdin>:1:1: 123
json: error: <stdin>:1:1: ^
command failed: json -d '\''{"type":"list","args":[{"type":"array","args":[]}]}'\'' <<< '\''123'\''
$ json -d '\''{"type":"list","args":[{"type":"array","args":[]}]}'\'' <<< '\''"foo"'\''
json: error: <stdin>:1:1: type check error: type mismatch: expected a value of type `{"type":"list","args":[{"type":"array","args":[]}]}'\''
json: error: <stdin>:1:1: "foo"
json: error: <stdin>:1:1: ^
command failed: json -d '\''{"type":"list","args":[{"type":"array","args":[]}]}'\'' <<< '\''"foo"'\''
$ json -d '\''{"type":"list","args":[{"type":"array","args":[]}]}'\'' <<< '\''{}'\''
json: error: <stdin>:1:1: type check error: type mismatch: expected a value of type `{"type":"list","args":[{"type":"array","args":[]}]}'\''
json: error: <stdin>:1:1: {}
json: error: <stdin>:1:1: ^
command failed: json -d '\''{"type":"list","args":[{"type":"array","args":[]}]}'\'' <<< '\''{}'\''
$ json -d '\''{"type":"list","args":[{"type":"array","args":[]}]}'\'' <<< '\''[]'\''
[]'
) -L list-closed-array-empty-val-basic.new <(
echo '$ export JSON_TYPE_CHECK_ERROR_COMPLETE_TYPES=yes'
export JSON_TYPE_CHECK_ERROR_COMPLETE_TYPES=yes 2>&1 ||
echo 'command failed: export JSON_TYPE_CHECK_ERROR_COMPLETE_TYPES=yes'

echo '$ json() { LD_LIBRARY_PATH=../lib ../src/json --terse --verbose --literal-value "$@"; }'
json() { LD_LIBRARY_PATH=../lib ../src/json --terse --verbose --literal-value "$@"; } 2>&1 ||
echo 'command failed: json() { LD_LIBRARY_PATH=../lib ../src/json --terse --verbose --literal-value "$@"; }'

echo '$ json -d '\''{"type":"list","args":[{"type":"array","args":[]}]}'\'' <<< '\''null'\'''
json -d '{"type":"list","args":[{"type":"array","args":[]}]}' <<< 'null' 2>&1 ||
echo 'command failed: json -d '\''{"type":"list","args":[{"type":"array","args":[]}]}'\'' <<< '\''null'\'''

echo '$ json -d '\''{"type":"list","args":[{"type":"array","args":[]}]}'\'' <<< '\''false'\'''
json -d '{"type":"list","args":[{"type":"array","args":[]}]}' <<< 'false' 2>&1 ||
echo 'command failed: json -d '\''{"type":"list","args":[{"type":"array","args":[]}]}'\'' <<< '\''false'\'''

echo '$ json -d '\''{"type":"list","args":[{"type":"array","args":[]}]}'\'' <<< '\''true'\'''
json -d '{"type":"list","args":[{"type":"array","args":[]}]}' <<< 'true' 2>&1 ||
echo 'command failed: json -d '\''{"type":"list","args":[{"type":"array","args":[]}]}'\'' <<< '\''true'\'''

echo '$ json -d '\''{"type":"list","args":[{"type":"array","args":[]}]}'\'' <<< '\''123'\'''
json -d '{"type":"list","args":[{"type":"array","args":[]}]}' <<< '123' 2>&1 ||
echo 'command failed: json -d '\''{"type":"list","args":[{"type":"array","args":[]}]}'\'' <<< '\''123'\'''

echo '$ json -d '\''{"type":"list","args":[{"type":"array","args":[]}]}'\'' <<< '\''"foo"'\'''
json -d '{"type":"list","args":[{"type":"array","args":[]}]}' <<< '"foo"' 2>&1 ||
echo 'command failed: json -d '\''{"type":"list","args":[{"type":"array","args":[]}]}'\'' <<< '\''"foo"'\'''

echo '$ json -d '\''{"type":"list","args":[{"type":"array","args":[]}]}'\'' <<< '\''{}'\'''
json -d '{"type":"list","args":[{"type":"array","args":[]}]}' <<< '{}' 2>&1 ||
echo 'command failed: json -d '\''{"type":"list","args":[{"type":"array","args":[]}]}'\'' <<< '\''{}'\'''

echo '$ json -d '\''{"type":"list","args":[{"type":"array","args":[]}]}'\'' <<< '\''[]'\'''
json -d '{"type":"list","args":[{"type":"array","args":[]}]}' <<< '[]' 2>&1 ||
echo 'command failed: json -d '\''{"type":"list","args":[{"type":"array","args":[]}]}'\'' <<< '\''[]'\'''
)

