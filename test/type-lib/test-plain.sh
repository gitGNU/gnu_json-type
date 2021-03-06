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
# $ json-gentest -C type-lib:plain
#

[[ "$1" =~ ^-u[0-9]+$ ]] &&
u="${1:2}" ||
u=""

diff -u$u -L plain.old <(echo \
'$ json() { set -o pipefail && LD_LIBRARY_PATH=../lib ../src/json --literal-value -V -TA "$@"|LD_LIBRARY_PATH=../lib ../src/json --from-ast-print --verbose --no-error; }
$ json <<< '\''{"plain":null}'\''
{
    "plain": null
}
$ json <<< '\''{"plain":false}'\''
{
    "plain": false
}
$ json <<< '\''{"plain":true}'\''
{
    "plain": true
}
$ json <<< '\''{"plain":123}'\''
{
    "plain": 123
}
$ json <<< '\''{"plain":"foo"}'\''
{
    "plain": "foo"
}
$ json <<< '\''{"plain":"type"}'\''
{
    "plain": "type"
}
$ json <<< '\''{"plain":"null"}'\''
{
    "plain": "null"
}
$ json <<< '\''{"plain":"boolean"}'\''
{
    "plain": "boolean"
}
$ json <<< '\''{"plain":"number"}'\''
{
    "plain": "number"
}
$ json <<< '\''{"plain":"string"}'\''
{
    "plain": "string"
}
$ json <<< '\''{"plain":"object"}'\''
{
    "plain": "object"
}
$ json <<< '\''{"plain":"array"}'\''
{
    "plain": "array"
}
$ json <<< '\''{"plain":"list"}'\''
{
    "plain": "list"
}
$ json <<< '\''{"plain":"dict"}'\''
{
    "plain": "dict"
}
$ json <<< '\''{"plain":{}}'\''
json: error: <stdin>:1:10: meta error: invalid "plain" object: value must be `null'\'', a boolean, a number or a string
json: error: <stdin>:1:10: {"plain":{}}
json: error: <stdin>:1:10:          ^
command failed: json <<< '\''{"plain":{}}'\''
$ json <<< '\''{"plain":{"foo":"bar"}}'\''
json: error: <stdin>:1:10: meta error: invalid "plain" object: value must be `null'\'', a boolean, a number or a string
json: error: <stdin>:1:10: {"plain":{"foo":"bar"}}
json: error: <stdin>:1:10:          ^
command failed: json <<< '\''{"plain":{"foo":"bar"}}'\''
$ json <<< '\''{"plain":[]}'\''
json: error: <stdin>:1:10: meta error: invalid "plain" object: value must be `null'\'', a boolean, a number or a string
json: error: <stdin>:1:10: {"plain":[]}
json: error: <stdin>:1:10:          ^
command failed: json <<< '\''{"plain":[]}'\''
$ json <<< '\''{"plain":["foo","bar"]}'\''
json: error: <stdin>:1:10: meta error: invalid "plain" object: value must be `null'\'', a boolean, a number or a string
json: error: <stdin>:1:10: {"plain":["foo","bar"]}
json: error: <stdin>:1:10:          ^
command failed: json <<< '\''{"plain":["foo","bar"]}'\'''
) -L plain.new <(
echo '$ json() { set -o pipefail && LD_LIBRARY_PATH=../lib ../src/json --literal-value -V -TA "$@"|LD_LIBRARY_PATH=../lib ../src/json --from-ast-print --verbose --no-error; }'
json() { set -o pipefail && LD_LIBRARY_PATH=../lib ../src/json --literal-value -V -TA "$@"|LD_LIBRARY_PATH=../lib ../src/json --from-ast-print --verbose --no-error; } 2>&1 ||
echo 'command failed: json() { set -o pipefail && LD_LIBRARY_PATH=../lib ../src/json --literal-value -V -TA "$@"|LD_LIBRARY_PATH=../lib ../src/json --from-ast-print --verbose --no-error; }'

echo '$ json <<< '\''{"plain":null}'\'''
json <<< '{"plain":null}' 2>&1 ||
echo 'command failed: json <<< '\''{"plain":null}'\'''

echo '$ json <<< '\''{"plain":false}'\'''
json <<< '{"plain":false}' 2>&1 ||
echo 'command failed: json <<< '\''{"plain":false}'\'''

echo '$ json <<< '\''{"plain":true}'\'''
json <<< '{"plain":true}' 2>&1 ||
echo 'command failed: json <<< '\''{"plain":true}'\'''

echo '$ json <<< '\''{"plain":123}'\'''
json <<< '{"plain":123}' 2>&1 ||
echo 'command failed: json <<< '\''{"plain":123}'\'''

echo '$ json <<< '\''{"plain":"foo"}'\'''
json <<< '{"plain":"foo"}' 2>&1 ||
echo 'command failed: json <<< '\''{"plain":"foo"}'\'''

echo '$ json <<< '\''{"plain":"type"}'\'''
json <<< '{"plain":"type"}' 2>&1 ||
echo 'command failed: json <<< '\''{"plain":"type"}'\'''

echo '$ json <<< '\''{"plain":"null"}'\'''
json <<< '{"plain":"null"}' 2>&1 ||
echo 'command failed: json <<< '\''{"plain":"null"}'\'''

echo '$ json <<< '\''{"plain":"boolean"}'\'''
json <<< '{"plain":"boolean"}' 2>&1 ||
echo 'command failed: json <<< '\''{"plain":"boolean"}'\'''

echo '$ json <<< '\''{"plain":"number"}'\'''
json <<< '{"plain":"number"}' 2>&1 ||
echo 'command failed: json <<< '\''{"plain":"number"}'\'''

echo '$ json <<< '\''{"plain":"string"}'\'''
json <<< '{"plain":"string"}' 2>&1 ||
echo 'command failed: json <<< '\''{"plain":"string"}'\'''

echo '$ json <<< '\''{"plain":"object"}'\'''
json <<< '{"plain":"object"}' 2>&1 ||
echo 'command failed: json <<< '\''{"plain":"object"}'\'''

echo '$ json <<< '\''{"plain":"array"}'\'''
json <<< '{"plain":"array"}' 2>&1 ||
echo 'command failed: json <<< '\''{"plain":"array"}'\'''

echo '$ json <<< '\''{"plain":"list"}'\'''
json <<< '{"plain":"list"}' 2>&1 ||
echo 'command failed: json <<< '\''{"plain":"list"}'\'''

echo '$ json <<< '\''{"plain":"dict"}'\'''
json <<< '{"plain":"dict"}' 2>&1 ||
echo 'command failed: json <<< '\''{"plain":"dict"}'\'''

echo '$ json <<< '\''{"plain":{}}'\'''
json <<< '{"plain":{}}' 2>&1 ||
echo 'command failed: json <<< '\''{"plain":{}}'\'''

echo '$ json <<< '\''{"plain":{"foo":"bar"}}'\'''
json <<< '{"plain":{"foo":"bar"}}' 2>&1 ||
echo 'command failed: json <<< '\''{"plain":{"foo":"bar"}}'\'''

echo '$ json <<< '\''{"plain":[]}'\'''
json <<< '{"plain":[]}' 2>&1 ||
echo 'command failed: json <<< '\''{"plain":[]}'\'''

echo '$ json <<< '\''{"plain":["foo","bar"]}'\'''
json <<< '{"plain":["foo","bar"]}' 2>&1 ||
echo 'command failed: json <<< '\''{"plain":["foo","bar"]}'\'''
)

