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
# $ json-gentest -C type-checker-list:list-open-array-basic-open-array-plain-val-array-empty
#

[[ "$1" =~ ^-u[0-9]+$ ]] &&
u="${1:2}" ||
u=""

diff -u$u -L list-open-array-basic-open-array-plain-val-array-empty.old <(echo \
'$ export JSON_TYPE_CHECK_ERROR_COMPLETE_TYPES=yes
$ json() { LD_LIBRARY_PATH=../lib ../src/json --terse --verbose --literal-value "$@"; }
$ json -d '\''{"type":"list","args":[{"type":"array","args":"type"},{"type":"array","args":{"plain":null}}]}'\'' <<< '\''[]'\''
json: error: <text>:1:78: type lib error: attribute error: invalid "list" type object: array argument ambiguity (the other is at 1:47)
command failed: json -d '\''{"type":"list","args":[{"type":"array","args":"type"},{"type":"array","args":{"plain":null}}]}'\'' <<< '\''[]'\''
$ json -d '\''{"type":"list","args":[{"type":"array","args":"type"},{"type":"array","args":{"plain":false}}]}'\'' <<< '\''[]'\''
json: error: <text>:1:78: type lib error: attribute error: invalid "list" type object: array argument ambiguity (the other is at 1:47)
command failed: json -d '\''{"type":"list","args":[{"type":"array","args":"type"},{"type":"array","args":{"plain":false}}]}'\'' <<< '\''[]'\''
$ json -d '\''{"type":"list","args":[{"type":"array","args":"type"},{"type":"array","args":{"plain":true}}]}'\'' <<< '\''[]'\''
json: error: <text>:1:78: type lib error: attribute error: invalid "list" type object: array argument ambiguity (the other is at 1:47)
command failed: json -d '\''{"type":"list","args":[{"type":"array","args":"type"},{"type":"array","args":{"plain":true}}]}'\'' <<< '\''[]'\''
$ json -d '\''{"type":"list","args":[{"type":"array","args":"type"},{"type":"array","args":{"plain":123}}]}'\'' <<< '\''[]'\''
json: error: <text>:1:78: type lib error: attribute error: invalid "list" type object: array argument ambiguity (the other is at 1:47)
command failed: json -d '\''{"type":"list","args":[{"type":"array","args":"type"},{"type":"array","args":{"plain":123}}]}'\'' <<< '\''[]'\''
$ json -d '\''{"type":"list","args":[{"type":"array","args":"type"},{"type":"array","args":{"plain":"foo"}}]}'\'' <<< '\''[]'\''
json: error: <text>:1:78: type lib error: attribute error: invalid "list" type object: array argument ambiguity (the other is at 1:47)
command failed: json -d '\''{"type":"list","args":[{"type":"array","args":"type"},{"type":"array","args":{"plain":"foo"}}]}'\'' <<< '\''[]'\''
$ json -d '\''{"type":"list","args":[{"type":"array","args":"null"},{"type":"array","args":{"plain":null}}]}'\'' <<< '\''[]'\''
json: error: <text>:1:78: type lib error: attribute error: invalid "list" type object: array argument ambiguity (the other is at 1:47)
command failed: json -d '\''{"type":"list","args":[{"type":"array","args":"null"},{"type":"array","args":{"plain":null}}]}'\'' <<< '\''[]'\''
$ json -d '\''{"type":"list","args":[{"type":"array","args":"null"},{"type":"array","args":{"plain":false}}]}'\'' <<< '\''[]'\''
[]
$ json -d '\''{"type":"list","args":[{"type":"array","args":"null"},{"type":"array","args":{"plain":true}}]}'\'' <<< '\''[]'\''
[]
$ json -d '\''{"type":"list","args":[{"type":"array","args":"null"},{"type":"array","args":{"plain":123}}]}'\'' <<< '\''[]'\''
[]
$ json -d '\''{"type":"list","args":[{"type":"array","args":"null"},{"type":"array","args":{"plain":"foo"}}]}'\'' <<< '\''[]'\''
[]
$ json -d '\''{"type":"list","args":[{"type":"array","args":"boolean"},{"type":"array","args":{"plain":null}}]}'\'' <<< '\''[]'\''
[]
$ json -d '\''{"type":"list","args":[{"type":"array","args":"boolean"},{"type":"array","args":{"plain":false}}]}'\'' <<< '\''[]'\''
json: error: <text>:1:81: type lib error: attribute error: invalid "list" type object: array argument ambiguity (the other is at 1:47)
command failed: json -d '\''{"type":"list","args":[{"type":"array","args":"boolean"},{"type":"array","args":{"plain":false}}]}'\'' <<< '\''[]'\''
$ json -d '\''{"type":"list","args":[{"type":"array","args":"boolean"},{"type":"array","args":{"plain":true}}]}'\'' <<< '\''[]'\''
json: error: <text>:1:81: type lib error: attribute error: invalid "list" type object: array argument ambiguity (the other is at 1:47)
command failed: json -d '\''{"type":"list","args":[{"type":"array","args":"boolean"},{"type":"array","args":{"plain":true}}]}'\'' <<< '\''[]'\''
$ json -d '\''{"type":"list","args":[{"type":"array","args":"boolean"},{"type":"array","args":{"plain":123}}]}'\'' <<< '\''[]'\''
[]
$ json -d '\''{"type":"list","args":[{"type":"array","args":"boolean"},{"type":"array","args":{"plain":"foo"}}]}'\'' <<< '\''[]'\''
[]
$ json -d '\''{"type":"list","args":[{"type":"array","args":"number"},{"type":"array","args":{"plain":null}}]}'\'' <<< '\''[]'\''
[]
$ json -d '\''{"type":"list","args":[{"type":"array","args":"number"},{"type":"array","args":{"plain":false}}]}'\'' <<< '\''[]'\''
[]
$ json -d '\''{"type":"list","args":[{"type":"array","args":"number"},{"type":"array","args":{"plain":true}}]}'\'' <<< '\''[]'\''
[]
$ json -d '\''{"type":"list","args":[{"type":"array","args":"number"},{"type":"array","args":{"plain":123}}]}'\'' <<< '\''[]'\''
json: error: <text>:1:80: type lib error: attribute error: invalid "list" type object: array argument ambiguity (the other is at 1:47)
command failed: json -d '\''{"type":"list","args":[{"type":"array","args":"number"},{"type":"array","args":{"plain":123}}]}'\'' <<< '\''[]'\''
$ json -d '\''{"type":"list","args":[{"type":"array","args":"number"},{"type":"array","args":{"plain":"foo"}}]}'\'' <<< '\''[]'\''
[]
$ json -d '\''{"type":"list","args":[{"type":"array","args":"string"},{"type":"array","args":{"plain":null}}]}'\'' <<< '\''[]'\''
[]
$ json -d '\''{"type":"list","args":[{"type":"array","args":"string"},{"type":"array","args":{"plain":false}}]}'\'' <<< '\''[]'\''
[]
$ json -d '\''{"type":"list","args":[{"type":"array","args":"string"},{"type":"array","args":{"plain":true}}]}'\'' <<< '\''[]'\''
[]
$ json -d '\''{"type":"list","args":[{"type":"array","args":"string"},{"type":"array","args":{"plain":123}}]}'\'' <<< '\''[]'\''
[]
$ json -d '\''{"type":"list","args":[{"type":"array","args":"string"},{"type":"array","args":{"plain":"foo"}}]}'\'' <<< '\''[]'\''
json: error: <text>:1:80: type lib error: attribute error: invalid "list" type object: array argument ambiguity (the other is at 1:47)
command failed: json -d '\''{"type":"list","args":[{"type":"array","args":"string"},{"type":"array","args":{"plain":"foo"}}]}'\'' <<< '\''[]'\''
$ json -d '\''{"type":"list","args":[{"type":"array","args":"object"},{"type":"array","args":{"plain":null}}]}'\'' <<< '\''[]'\''
[]
$ json -d '\''{"type":"list","args":[{"type":"array","args":"object"},{"type":"array","args":{"plain":false}}]}'\'' <<< '\''[]'\''
[]
$ json -d '\''{"type":"list","args":[{"type":"array","args":"object"},{"type":"array","args":{"plain":true}}]}'\'' <<< '\''[]'\''
[]
$ json -d '\''{"type":"list","args":[{"type":"array","args":"object"},{"type":"array","args":{"plain":123}}]}'\'' <<< '\''[]'\''
[]
$ json -d '\''{"type":"list","args":[{"type":"array","args":"object"},{"type":"array","args":{"plain":"foo"}}]}'\'' <<< '\''[]'\''
[]
$ json -d '\''{"type":"list","args":[{"type":"array","args":"array"},{"type":"array","args":{"plain":null}}]}'\'' <<< '\''[]'\''
[]
$ json -d '\''{"type":"list","args":[{"type":"array","args":"array"},{"type":"array","args":{"plain":false}}]}'\'' <<< '\''[]'\''
[]
$ json -d '\''{"type":"list","args":[{"type":"array","args":"array"},{"type":"array","args":{"plain":true}}]}'\'' <<< '\''[]'\''
[]
$ json -d '\''{"type":"list","args":[{"type":"array","args":"array"},{"type":"array","args":{"plain":123}}]}'\'' <<< '\''[]'\''
[]
$ json -d '\''{"type":"list","args":[{"type":"array","args":"array"},{"type":"array","args":{"plain":"foo"}}]}'\'' <<< '\''[]'\''
[]'
) -L list-open-array-basic-open-array-plain-val-array-empty.new <(
echo '$ export JSON_TYPE_CHECK_ERROR_COMPLETE_TYPES=yes'
export JSON_TYPE_CHECK_ERROR_COMPLETE_TYPES=yes 2>&1 ||
echo 'command failed: export JSON_TYPE_CHECK_ERROR_COMPLETE_TYPES=yes'

echo '$ json() { LD_LIBRARY_PATH=../lib ../src/json --terse --verbose --literal-value "$@"; }'
json() { LD_LIBRARY_PATH=../lib ../src/json --terse --verbose --literal-value "$@"; } 2>&1 ||
echo 'command failed: json() { LD_LIBRARY_PATH=../lib ../src/json --terse --verbose --literal-value "$@"; }'

echo '$ json -d '\''{"type":"list","args":[{"type":"array","args":"type"},{"type":"array","args":{"plain":null}}]}'\'' <<< '\''[]'\'''
json -d '{"type":"list","args":[{"type":"array","args":"type"},{"type":"array","args":{"plain":null}}]}' <<< '[]' 2>&1 ||
echo 'command failed: json -d '\''{"type":"list","args":[{"type":"array","args":"type"},{"type":"array","args":{"plain":null}}]}'\'' <<< '\''[]'\'''

echo '$ json -d '\''{"type":"list","args":[{"type":"array","args":"type"},{"type":"array","args":{"plain":false}}]}'\'' <<< '\''[]'\'''
json -d '{"type":"list","args":[{"type":"array","args":"type"},{"type":"array","args":{"plain":false}}]}' <<< '[]' 2>&1 ||
echo 'command failed: json -d '\''{"type":"list","args":[{"type":"array","args":"type"},{"type":"array","args":{"plain":false}}]}'\'' <<< '\''[]'\'''

echo '$ json -d '\''{"type":"list","args":[{"type":"array","args":"type"},{"type":"array","args":{"plain":true}}]}'\'' <<< '\''[]'\'''
json -d '{"type":"list","args":[{"type":"array","args":"type"},{"type":"array","args":{"plain":true}}]}' <<< '[]' 2>&1 ||
echo 'command failed: json -d '\''{"type":"list","args":[{"type":"array","args":"type"},{"type":"array","args":{"plain":true}}]}'\'' <<< '\''[]'\'''

echo '$ json -d '\''{"type":"list","args":[{"type":"array","args":"type"},{"type":"array","args":{"plain":123}}]}'\'' <<< '\''[]'\'''
json -d '{"type":"list","args":[{"type":"array","args":"type"},{"type":"array","args":{"plain":123}}]}' <<< '[]' 2>&1 ||
echo 'command failed: json -d '\''{"type":"list","args":[{"type":"array","args":"type"},{"type":"array","args":{"plain":123}}]}'\'' <<< '\''[]'\'''

echo '$ json -d '\''{"type":"list","args":[{"type":"array","args":"type"},{"type":"array","args":{"plain":"foo"}}]}'\'' <<< '\''[]'\'''
json -d '{"type":"list","args":[{"type":"array","args":"type"},{"type":"array","args":{"plain":"foo"}}]}' <<< '[]' 2>&1 ||
echo 'command failed: json -d '\''{"type":"list","args":[{"type":"array","args":"type"},{"type":"array","args":{"plain":"foo"}}]}'\'' <<< '\''[]'\'''

echo '$ json -d '\''{"type":"list","args":[{"type":"array","args":"null"},{"type":"array","args":{"plain":null}}]}'\'' <<< '\''[]'\'''
json -d '{"type":"list","args":[{"type":"array","args":"null"},{"type":"array","args":{"plain":null}}]}' <<< '[]' 2>&1 ||
echo 'command failed: json -d '\''{"type":"list","args":[{"type":"array","args":"null"},{"type":"array","args":{"plain":null}}]}'\'' <<< '\''[]'\'''

echo '$ json -d '\''{"type":"list","args":[{"type":"array","args":"null"},{"type":"array","args":{"plain":false}}]}'\'' <<< '\''[]'\'''
json -d '{"type":"list","args":[{"type":"array","args":"null"},{"type":"array","args":{"plain":false}}]}' <<< '[]' 2>&1 ||
echo 'command failed: json -d '\''{"type":"list","args":[{"type":"array","args":"null"},{"type":"array","args":{"plain":false}}]}'\'' <<< '\''[]'\'''

echo '$ json -d '\''{"type":"list","args":[{"type":"array","args":"null"},{"type":"array","args":{"plain":true}}]}'\'' <<< '\''[]'\'''
json -d '{"type":"list","args":[{"type":"array","args":"null"},{"type":"array","args":{"plain":true}}]}' <<< '[]' 2>&1 ||
echo 'command failed: json -d '\''{"type":"list","args":[{"type":"array","args":"null"},{"type":"array","args":{"plain":true}}]}'\'' <<< '\''[]'\'''

echo '$ json -d '\''{"type":"list","args":[{"type":"array","args":"null"},{"type":"array","args":{"plain":123}}]}'\'' <<< '\''[]'\'''
json -d '{"type":"list","args":[{"type":"array","args":"null"},{"type":"array","args":{"plain":123}}]}' <<< '[]' 2>&1 ||
echo 'command failed: json -d '\''{"type":"list","args":[{"type":"array","args":"null"},{"type":"array","args":{"plain":123}}]}'\'' <<< '\''[]'\'''

echo '$ json -d '\''{"type":"list","args":[{"type":"array","args":"null"},{"type":"array","args":{"plain":"foo"}}]}'\'' <<< '\''[]'\'''
json -d '{"type":"list","args":[{"type":"array","args":"null"},{"type":"array","args":{"plain":"foo"}}]}' <<< '[]' 2>&1 ||
echo 'command failed: json -d '\''{"type":"list","args":[{"type":"array","args":"null"},{"type":"array","args":{"plain":"foo"}}]}'\'' <<< '\''[]'\'''

echo '$ json -d '\''{"type":"list","args":[{"type":"array","args":"boolean"},{"type":"array","args":{"plain":null}}]}'\'' <<< '\''[]'\'''
json -d '{"type":"list","args":[{"type":"array","args":"boolean"},{"type":"array","args":{"plain":null}}]}' <<< '[]' 2>&1 ||
echo 'command failed: json -d '\''{"type":"list","args":[{"type":"array","args":"boolean"},{"type":"array","args":{"plain":null}}]}'\'' <<< '\''[]'\'''

echo '$ json -d '\''{"type":"list","args":[{"type":"array","args":"boolean"},{"type":"array","args":{"plain":false}}]}'\'' <<< '\''[]'\'''
json -d '{"type":"list","args":[{"type":"array","args":"boolean"},{"type":"array","args":{"plain":false}}]}' <<< '[]' 2>&1 ||
echo 'command failed: json -d '\''{"type":"list","args":[{"type":"array","args":"boolean"},{"type":"array","args":{"plain":false}}]}'\'' <<< '\''[]'\'''

echo '$ json -d '\''{"type":"list","args":[{"type":"array","args":"boolean"},{"type":"array","args":{"plain":true}}]}'\'' <<< '\''[]'\'''
json -d '{"type":"list","args":[{"type":"array","args":"boolean"},{"type":"array","args":{"plain":true}}]}' <<< '[]' 2>&1 ||
echo 'command failed: json -d '\''{"type":"list","args":[{"type":"array","args":"boolean"},{"type":"array","args":{"plain":true}}]}'\'' <<< '\''[]'\'''

echo '$ json -d '\''{"type":"list","args":[{"type":"array","args":"boolean"},{"type":"array","args":{"plain":123}}]}'\'' <<< '\''[]'\'''
json -d '{"type":"list","args":[{"type":"array","args":"boolean"},{"type":"array","args":{"plain":123}}]}' <<< '[]' 2>&1 ||
echo 'command failed: json -d '\''{"type":"list","args":[{"type":"array","args":"boolean"},{"type":"array","args":{"plain":123}}]}'\'' <<< '\''[]'\'''

echo '$ json -d '\''{"type":"list","args":[{"type":"array","args":"boolean"},{"type":"array","args":{"plain":"foo"}}]}'\'' <<< '\''[]'\'''
json -d '{"type":"list","args":[{"type":"array","args":"boolean"},{"type":"array","args":{"plain":"foo"}}]}' <<< '[]' 2>&1 ||
echo 'command failed: json -d '\''{"type":"list","args":[{"type":"array","args":"boolean"},{"type":"array","args":{"plain":"foo"}}]}'\'' <<< '\''[]'\'''

echo '$ json -d '\''{"type":"list","args":[{"type":"array","args":"number"},{"type":"array","args":{"plain":null}}]}'\'' <<< '\''[]'\'''
json -d '{"type":"list","args":[{"type":"array","args":"number"},{"type":"array","args":{"plain":null}}]}' <<< '[]' 2>&1 ||
echo 'command failed: json -d '\''{"type":"list","args":[{"type":"array","args":"number"},{"type":"array","args":{"plain":null}}]}'\'' <<< '\''[]'\'''

echo '$ json -d '\''{"type":"list","args":[{"type":"array","args":"number"},{"type":"array","args":{"plain":false}}]}'\'' <<< '\''[]'\'''
json -d '{"type":"list","args":[{"type":"array","args":"number"},{"type":"array","args":{"plain":false}}]}' <<< '[]' 2>&1 ||
echo 'command failed: json -d '\''{"type":"list","args":[{"type":"array","args":"number"},{"type":"array","args":{"plain":false}}]}'\'' <<< '\''[]'\'''

echo '$ json -d '\''{"type":"list","args":[{"type":"array","args":"number"},{"type":"array","args":{"plain":true}}]}'\'' <<< '\''[]'\'''
json -d '{"type":"list","args":[{"type":"array","args":"number"},{"type":"array","args":{"plain":true}}]}' <<< '[]' 2>&1 ||
echo 'command failed: json -d '\''{"type":"list","args":[{"type":"array","args":"number"},{"type":"array","args":{"plain":true}}]}'\'' <<< '\''[]'\'''

echo '$ json -d '\''{"type":"list","args":[{"type":"array","args":"number"},{"type":"array","args":{"plain":123}}]}'\'' <<< '\''[]'\'''
json -d '{"type":"list","args":[{"type":"array","args":"number"},{"type":"array","args":{"plain":123}}]}' <<< '[]' 2>&1 ||
echo 'command failed: json -d '\''{"type":"list","args":[{"type":"array","args":"number"},{"type":"array","args":{"plain":123}}]}'\'' <<< '\''[]'\'''

echo '$ json -d '\''{"type":"list","args":[{"type":"array","args":"number"},{"type":"array","args":{"plain":"foo"}}]}'\'' <<< '\''[]'\'''
json -d '{"type":"list","args":[{"type":"array","args":"number"},{"type":"array","args":{"plain":"foo"}}]}' <<< '[]' 2>&1 ||
echo 'command failed: json -d '\''{"type":"list","args":[{"type":"array","args":"number"},{"type":"array","args":{"plain":"foo"}}]}'\'' <<< '\''[]'\'''

echo '$ json -d '\''{"type":"list","args":[{"type":"array","args":"string"},{"type":"array","args":{"plain":null}}]}'\'' <<< '\''[]'\'''
json -d '{"type":"list","args":[{"type":"array","args":"string"},{"type":"array","args":{"plain":null}}]}' <<< '[]' 2>&1 ||
echo 'command failed: json -d '\''{"type":"list","args":[{"type":"array","args":"string"},{"type":"array","args":{"plain":null}}]}'\'' <<< '\''[]'\'''

echo '$ json -d '\''{"type":"list","args":[{"type":"array","args":"string"},{"type":"array","args":{"plain":false}}]}'\'' <<< '\''[]'\'''
json -d '{"type":"list","args":[{"type":"array","args":"string"},{"type":"array","args":{"plain":false}}]}' <<< '[]' 2>&1 ||
echo 'command failed: json -d '\''{"type":"list","args":[{"type":"array","args":"string"},{"type":"array","args":{"plain":false}}]}'\'' <<< '\''[]'\'''

echo '$ json -d '\''{"type":"list","args":[{"type":"array","args":"string"},{"type":"array","args":{"plain":true}}]}'\'' <<< '\''[]'\'''
json -d '{"type":"list","args":[{"type":"array","args":"string"},{"type":"array","args":{"plain":true}}]}' <<< '[]' 2>&1 ||
echo 'command failed: json -d '\''{"type":"list","args":[{"type":"array","args":"string"},{"type":"array","args":{"plain":true}}]}'\'' <<< '\''[]'\'''

echo '$ json -d '\''{"type":"list","args":[{"type":"array","args":"string"},{"type":"array","args":{"plain":123}}]}'\'' <<< '\''[]'\'''
json -d '{"type":"list","args":[{"type":"array","args":"string"},{"type":"array","args":{"plain":123}}]}' <<< '[]' 2>&1 ||
echo 'command failed: json -d '\''{"type":"list","args":[{"type":"array","args":"string"},{"type":"array","args":{"plain":123}}]}'\'' <<< '\''[]'\'''

echo '$ json -d '\''{"type":"list","args":[{"type":"array","args":"string"},{"type":"array","args":{"plain":"foo"}}]}'\'' <<< '\''[]'\'''
json -d '{"type":"list","args":[{"type":"array","args":"string"},{"type":"array","args":{"plain":"foo"}}]}' <<< '[]' 2>&1 ||
echo 'command failed: json -d '\''{"type":"list","args":[{"type":"array","args":"string"},{"type":"array","args":{"plain":"foo"}}]}'\'' <<< '\''[]'\'''

echo '$ json -d '\''{"type":"list","args":[{"type":"array","args":"object"},{"type":"array","args":{"plain":null}}]}'\'' <<< '\''[]'\'''
json -d '{"type":"list","args":[{"type":"array","args":"object"},{"type":"array","args":{"plain":null}}]}' <<< '[]' 2>&1 ||
echo 'command failed: json -d '\''{"type":"list","args":[{"type":"array","args":"object"},{"type":"array","args":{"plain":null}}]}'\'' <<< '\''[]'\'''

echo '$ json -d '\''{"type":"list","args":[{"type":"array","args":"object"},{"type":"array","args":{"plain":false}}]}'\'' <<< '\''[]'\'''
json -d '{"type":"list","args":[{"type":"array","args":"object"},{"type":"array","args":{"plain":false}}]}' <<< '[]' 2>&1 ||
echo 'command failed: json -d '\''{"type":"list","args":[{"type":"array","args":"object"},{"type":"array","args":{"plain":false}}]}'\'' <<< '\''[]'\'''

echo '$ json -d '\''{"type":"list","args":[{"type":"array","args":"object"},{"type":"array","args":{"plain":true}}]}'\'' <<< '\''[]'\'''
json -d '{"type":"list","args":[{"type":"array","args":"object"},{"type":"array","args":{"plain":true}}]}' <<< '[]' 2>&1 ||
echo 'command failed: json -d '\''{"type":"list","args":[{"type":"array","args":"object"},{"type":"array","args":{"plain":true}}]}'\'' <<< '\''[]'\'''

echo '$ json -d '\''{"type":"list","args":[{"type":"array","args":"object"},{"type":"array","args":{"plain":123}}]}'\'' <<< '\''[]'\'''
json -d '{"type":"list","args":[{"type":"array","args":"object"},{"type":"array","args":{"plain":123}}]}' <<< '[]' 2>&1 ||
echo 'command failed: json -d '\''{"type":"list","args":[{"type":"array","args":"object"},{"type":"array","args":{"plain":123}}]}'\'' <<< '\''[]'\'''

echo '$ json -d '\''{"type":"list","args":[{"type":"array","args":"object"},{"type":"array","args":{"plain":"foo"}}]}'\'' <<< '\''[]'\'''
json -d '{"type":"list","args":[{"type":"array","args":"object"},{"type":"array","args":{"plain":"foo"}}]}' <<< '[]' 2>&1 ||
echo 'command failed: json -d '\''{"type":"list","args":[{"type":"array","args":"object"},{"type":"array","args":{"plain":"foo"}}]}'\'' <<< '\''[]'\'''

echo '$ json -d '\''{"type":"list","args":[{"type":"array","args":"array"},{"type":"array","args":{"plain":null}}]}'\'' <<< '\''[]'\'''
json -d '{"type":"list","args":[{"type":"array","args":"array"},{"type":"array","args":{"plain":null}}]}' <<< '[]' 2>&1 ||
echo 'command failed: json -d '\''{"type":"list","args":[{"type":"array","args":"array"},{"type":"array","args":{"plain":null}}]}'\'' <<< '\''[]'\'''

echo '$ json -d '\''{"type":"list","args":[{"type":"array","args":"array"},{"type":"array","args":{"plain":false}}]}'\'' <<< '\''[]'\'''
json -d '{"type":"list","args":[{"type":"array","args":"array"},{"type":"array","args":{"plain":false}}]}' <<< '[]' 2>&1 ||
echo 'command failed: json -d '\''{"type":"list","args":[{"type":"array","args":"array"},{"type":"array","args":{"plain":false}}]}'\'' <<< '\''[]'\'''

echo '$ json -d '\''{"type":"list","args":[{"type":"array","args":"array"},{"type":"array","args":{"plain":true}}]}'\'' <<< '\''[]'\'''
json -d '{"type":"list","args":[{"type":"array","args":"array"},{"type":"array","args":{"plain":true}}]}' <<< '[]' 2>&1 ||
echo 'command failed: json -d '\''{"type":"list","args":[{"type":"array","args":"array"},{"type":"array","args":{"plain":true}}]}'\'' <<< '\''[]'\'''

echo '$ json -d '\''{"type":"list","args":[{"type":"array","args":"array"},{"type":"array","args":{"plain":123}}]}'\'' <<< '\''[]'\'''
json -d '{"type":"list","args":[{"type":"array","args":"array"},{"type":"array","args":{"plain":123}}]}' <<< '[]' 2>&1 ||
echo 'command failed: json -d '\''{"type":"list","args":[{"type":"array","args":"array"},{"type":"array","args":{"plain":123}}]}'\'' <<< '\''[]'\'''

echo '$ json -d '\''{"type":"list","args":[{"type":"array","args":"array"},{"type":"array","args":{"plain":"foo"}}]}'\'' <<< '\''[]'\'''
json -d '{"type":"list","args":[{"type":"array","args":"array"},{"type":"array","args":{"plain":"foo"}}]}' <<< '[]' 2>&1 ||
echo 'command failed: json -d '\''{"type":"list","args":[{"type":"array","args":"array"},{"type":"array","args":{"plain":"foo"}}]}'\'' <<< '\''[]'\'''
)

