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
# $ json-gentest -C type-checker-list:list-closed-array-basic-val-array-basic-basic
#

[[ "$1" =~ ^-u[0-9]+$ ]] &&
u="${1:2}" ||
u=""

diff -u$u -L list-closed-array-basic-val-array-basic-basic.old <(echo \
'$ export JSON_TYPE_CHECK_ERROR_COMPLETE_TYPES=yes
$ json() { LD_LIBRARY_PATH=../lib ../src/json --terse --verbose --literal-value "$@"; }
$ json -d '\''{"type":"list","args":[{"type":"array","args":["type"]}]}'\'' <<< '\''[[{}],null]'\''
json: error: <stdin>:1:6: type check error: too many arguments
json: error: <stdin>:1:6: [[{}],null]
json: error: <stdin>:1:6:      ^
[[{}]
command failed: json -d '\''{"type":"list","args":[{"type":"array","args":["type"]}]}'\'' <<< '\''[[{}],null]'\''
$ json -d '\''{"type":"list","args":[{"type":"array","args":["type"]}]}'\'' <<< '\''[[{}],false]'\''
json: error: <stdin>:1:6: type check error: too many arguments
json: error: <stdin>:1:6: [[{}],false]
json: error: <stdin>:1:6:      ^
[[{}]
command failed: json -d '\''{"type":"list","args":[{"type":"array","args":["type"]}]}'\'' <<< '\''[[{}],false]'\''
$ json -d '\''{"type":"list","args":[{"type":"array","args":["type"]}]}'\'' <<< '\''[[{}],true]'\''
json: error: <stdin>:1:6: type check error: too many arguments
json: error: <stdin>:1:6: [[{}],true]
json: error: <stdin>:1:6:      ^
[[{}]
command failed: json -d '\''{"type":"list","args":[{"type":"array","args":["type"]}]}'\'' <<< '\''[[{}],true]'\''
$ json -d '\''{"type":"list","args":[{"type":"array","args":["type"]}]}'\'' <<< '\''[[{}],123]'\''
json: error: <stdin>:1:6: type check error: too many arguments
json: error: <stdin>:1:6: [[{}],123]
json: error: <stdin>:1:6:      ^
[[{}]
command failed: json -d '\''{"type":"list","args":[{"type":"array","args":["type"]}]}'\'' <<< '\''[[{}],123]'\''
$ json -d '\''{"type":"list","args":[{"type":"array","args":["type"]}]}'\'' <<< '\''[[{}],"foo"]'\''
json: error: <stdin>:1:6: type check error: too many arguments
json: error: <stdin>:1:6: [[{}],"foo"]
json: error: <stdin>:1:6:      ^
[[{}]
command failed: json -d '\''{"type":"list","args":[{"type":"array","args":["type"]}]}'\'' <<< '\''[[{}],"foo"]'\''
$ json -d '\''{"type":"list","args":[{"type":"array","args":["type"]}]}'\'' <<< '\''[[{}],{}]'\''
json: error: <stdin>:1:6: type check error: too many arguments
json: error: <stdin>:1:6: [[{}],{}]
json: error: <stdin>:1:6:      ^
[[{}]
command failed: json -d '\''{"type":"list","args":[{"type":"array","args":["type"]}]}'\'' <<< '\''[[{}],{}]'\''
$ json -d '\''{"type":"list","args":[{"type":"array","args":["type"]}]}'\'' <<< '\''[[{}],[]]'\''
json: error: <stdin>:1:6: type check error: too many arguments
json: error: <stdin>:1:6: [[{}],[]]
json: error: <stdin>:1:6:      ^
[[{}]
command failed: json -d '\''{"type":"list","args":[{"type":"array","args":["type"]}]}'\'' <<< '\''[[{}],[]]'\''
$ json -d '\''{"type":"list","args":[{"type":"array","args":["null"]}]}'\'' <<< '\''[null,null]'\''
json: error: <stdin>:1:6: type check error: too many arguments
json: error: <stdin>:1:6: [null,null]
json: error: <stdin>:1:6:      ^
[null
command failed: json -d '\''{"type":"list","args":[{"type":"array","args":["null"]}]}'\'' <<< '\''[null,null]'\''
$ json -d '\''{"type":"list","args":[{"type":"array","args":["null"]}]}'\'' <<< '\''[null,false]'\''
json: error: <stdin>:1:6: type check error: too many arguments
json: error: <stdin>:1:6: [null,false]
json: error: <stdin>:1:6:      ^
[null
command failed: json -d '\''{"type":"list","args":[{"type":"array","args":["null"]}]}'\'' <<< '\''[null,false]'\''
$ json -d '\''{"type":"list","args":[{"type":"array","args":["null"]}]}'\'' <<< '\''[null,true]'\''
json: error: <stdin>:1:6: type check error: too many arguments
json: error: <stdin>:1:6: [null,true]
json: error: <stdin>:1:6:      ^
[null
command failed: json -d '\''{"type":"list","args":[{"type":"array","args":["null"]}]}'\'' <<< '\''[null,true]'\''
$ json -d '\''{"type":"list","args":[{"type":"array","args":["null"]}]}'\'' <<< '\''[null,123]'\''
json: error: <stdin>:1:6: type check error: too many arguments
json: error: <stdin>:1:6: [null,123]
json: error: <stdin>:1:6:      ^
[null
command failed: json -d '\''{"type":"list","args":[{"type":"array","args":["null"]}]}'\'' <<< '\''[null,123]'\''
$ json -d '\''{"type":"list","args":[{"type":"array","args":["null"]}]}'\'' <<< '\''[null,"foo"]'\''
json: error: <stdin>:1:6: type check error: too many arguments
json: error: <stdin>:1:6: [null,"foo"]
json: error: <stdin>:1:6:      ^
[null
command failed: json -d '\''{"type":"list","args":[{"type":"array","args":["null"]}]}'\'' <<< '\''[null,"foo"]'\''
$ json -d '\''{"type":"list","args":[{"type":"array","args":["null"]}]}'\'' <<< '\''[null,{}]'\''
json: error: <stdin>:1:6: type check error: too many arguments
json: error: <stdin>:1:6: [null,{}]
json: error: <stdin>:1:6:      ^
[null
command failed: json -d '\''{"type":"list","args":[{"type":"array","args":["null"]}]}'\'' <<< '\''[null,{}]'\''
$ json -d '\''{"type":"list","args":[{"type":"array","args":["null"]}]}'\'' <<< '\''[null,[]]'\''
json: error: <stdin>:1:6: type check error: too many arguments
json: error: <stdin>:1:6: [null,[]]
json: error: <stdin>:1:6:      ^
[null
command failed: json -d '\''{"type":"list","args":[{"type":"array","args":["null"]}]}'\'' <<< '\''[null,[]]'\''
$ json -d '\''{"type":"list","args":[{"type":"array","args":["boolean"]}]}'\'' <<< '\''[false,null]'\''
json: error: <stdin>:1:7: type check error: too many arguments
json: error: <stdin>:1:7: [false,null]
json: error: <stdin>:1:7:       ^
[false
command failed: json -d '\''{"type":"list","args":[{"type":"array","args":["boolean"]}]}'\'' <<< '\''[false,null]'\''
$ json -d '\''{"type":"list","args":[{"type":"array","args":["boolean"]}]}'\'' <<< '\''[false,false]'\''
json: error: <stdin>:1:7: type check error: too many arguments
json: error: <stdin>:1:7: [false,false]
json: error: <stdin>:1:7:       ^
[false
command failed: json -d '\''{"type":"list","args":[{"type":"array","args":["boolean"]}]}'\'' <<< '\''[false,false]'\''
$ json -d '\''{"type":"list","args":[{"type":"array","args":["boolean"]}]}'\'' <<< '\''[false,true]'\''
json: error: <stdin>:1:7: type check error: too many arguments
json: error: <stdin>:1:7: [false,true]
json: error: <stdin>:1:7:       ^
[false
command failed: json -d '\''{"type":"list","args":[{"type":"array","args":["boolean"]}]}'\'' <<< '\''[false,true]'\''
$ json -d '\''{"type":"list","args":[{"type":"array","args":["boolean"]}]}'\'' <<< '\''[false,123]'\''
json: error: <stdin>:1:7: type check error: too many arguments
json: error: <stdin>:1:7: [false,123]
json: error: <stdin>:1:7:       ^
[false
command failed: json -d '\''{"type":"list","args":[{"type":"array","args":["boolean"]}]}'\'' <<< '\''[false,123]'\''
$ json -d '\''{"type":"list","args":[{"type":"array","args":["boolean"]}]}'\'' <<< '\''[false,"foo"]'\''
json: error: <stdin>:1:7: type check error: too many arguments
json: error: <stdin>:1:7: [false,"foo"]
json: error: <stdin>:1:7:       ^
[false
command failed: json -d '\''{"type":"list","args":[{"type":"array","args":["boolean"]}]}'\'' <<< '\''[false,"foo"]'\''
$ json -d '\''{"type":"list","args":[{"type":"array","args":["boolean"]}]}'\'' <<< '\''[false,{}]'\''
json: error: <stdin>:1:7: type check error: too many arguments
json: error: <stdin>:1:7: [false,{}]
json: error: <stdin>:1:7:       ^
[false
command failed: json -d '\''{"type":"list","args":[{"type":"array","args":["boolean"]}]}'\'' <<< '\''[false,{}]'\''
$ json -d '\''{"type":"list","args":[{"type":"array","args":["boolean"]}]}'\'' <<< '\''[false,[]]'\''
json: error: <stdin>:1:7: type check error: too many arguments
json: error: <stdin>:1:7: [false,[]]
json: error: <stdin>:1:7:       ^
[false
command failed: json -d '\''{"type":"list","args":[{"type":"array","args":["boolean"]}]}'\'' <<< '\''[false,[]]'\''
$ json -d '\''{"type":"list","args":[{"type":"array","args":["number"]}]}'\'' <<< '\''[456,null]'\''
json: error: <stdin>:1:5: type check error: too many arguments
json: error: <stdin>:1:5: [456,null]
json: error: <stdin>:1:5:     ^
[456
command failed: json -d '\''{"type":"list","args":[{"type":"array","args":["number"]}]}'\'' <<< '\''[456,null]'\''
$ json -d '\''{"type":"list","args":[{"type":"array","args":["number"]}]}'\'' <<< '\''[456,false]'\''
json: error: <stdin>:1:5: type check error: too many arguments
json: error: <stdin>:1:5: [456,false]
json: error: <stdin>:1:5:     ^
[456
command failed: json -d '\''{"type":"list","args":[{"type":"array","args":["number"]}]}'\'' <<< '\''[456,false]'\''
$ json -d '\''{"type":"list","args":[{"type":"array","args":["number"]}]}'\'' <<< '\''[456,true]'\''
json: error: <stdin>:1:5: type check error: too many arguments
json: error: <stdin>:1:5: [456,true]
json: error: <stdin>:1:5:     ^
[456
command failed: json -d '\''{"type":"list","args":[{"type":"array","args":["number"]}]}'\'' <<< '\''[456,true]'\''
$ json -d '\''{"type":"list","args":[{"type":"array","args":["number"]}]}'\'' <<< '\''[456,123]'\''
json: error: <stdin>:1:5: type check error: too many arguments
json: error: <stdin>:1:5: [456,123]
json: error: <stdin>:1:5:     ^
[456
command failed: json -d '\''{"type":"list","args":[{"type":"array","args":["number"]}]}'\'' <<< '\''[456,123]'\''
$ json -d '\''{"type":"list","args":[{"type":"array","args":["number"]}]}'\'' <<< '\''[456,"foo"]'\''
json: error: <stdin>:1:5: type check error: too many arguments
json: error: <stdin>:1:5: [456,"foo"]
json: error: <stdin>:1:5:     ^
[456
command failed: json -d '\''{"type":"list","args":[{"type":"array","args":["number"]}]}'\'' <<< '\''[456,"foo"]'\''
$ json -d '\''{"type":"list","args":[{"type":"array","args":["number"]}]}'\'' <<< '\''[456,{}]'\''
json: error: <stdin>:1:5: type check error: too many arguments
json: error: <stdin>:1:5: [456,{}]
json: error: <stdin>:1:5:     ^
[456
command failed: json -d '\''{"type":"list","args":[{"type":"array","args":["number"]}]}'\'' <<< '\''[456,{}]'\''
$ json -d '\''{"type":"list","args":[{"type":"array","args":["number"]}]}'\'' <<< '\''[456,[]]'\''
json: error: <stdin>:1:5: type check error: too many arguments
json: error: <stdin>:1:5: [456,[]]
json: error: <stdin>:1:5:     ^
[456
command failed: json -d '\''{"type":"list","args":[{"type":"array","args":["number"]}]}'\'' <<< '\''[456,[]]'\''
$ json -d '\''{"type":"list","args":[{"type":"array","args":["string"]}]}'\'' <<< '\''["bar",null]'\''
json: error: <stdin>:1:7: type check error: too many arguments
json: error: <stdin>:1:7: ["bar",null]
json: error: <stdin>:1:7:       ^
["bar"
command failed: json -d '\''{"type":"list","args":[{"type":"array","args":["string"]}]}'\'' <<< '\''["bar",null]'\''
$ json -d '\''{"type":"list","args":[{"type":"array","args":["string"]}]}'\'' <<< '\''["bar",false]'\''
json: error: <stdin>:1:7: type check error: too many arguments
json: error: <stdin>:1:7: ["bar",false]
json: error: <stdin>:1:7:       ^
["bar"
command failed: json -d '\''{"type":"list","args":[{"type":"array","args":["string"]}]}'\'' <<< '\''["bar",false]'\''
$ json -d '\''{"type":"list","args":[{"type":"array","args":["string"]}]}'\'' <<< '\''["bar",true]'\''
json: error: <stdin>:1:7: type check error: too many arguments
json: error: <stdin>:1:7: ["bar",true]
json: error: <stdin>:1:7:       ^
["bar"
command failed: json -d '\''{"type":"list","args":[{"type":"array","args":["string"]}]}'\'' <<< '\''["bar",true]'\''
$ json -d '\''{"type":"list","args":[{"type":"array","args":["string"]}]}'\'' <<< '\''["bar",123]'\''
json: error: <stdin>:1:7: type check error: too many arguments
json: error: <stdin>:1:7: ["bar",123]
json: error: <stdin>:1:7:       ^
["bar"
command failed: json -d '\''{"type":"list","args":[{"type":"array","args":["string"]}]}'\'' <<< '\''["bar",123]'\''
$ json -d '\''{"type":"list","args":[{"type":"array","args":["string"]}]}'\'' <<< '\''["bar","foo"]'\''
json: error: <stdin>:1:7: type check error: too many arguments
json: error: <stdin>:1:7: ["bar","foo"]
json: error: <stdin>:1:7:       ^
["bar"
command failed: json -d '\''{"type":"list","args":[{"type":"array","args":["string"]}]}'\'' <<< '\''["bar","foo"]'\''
$ json -d '\''{"type":"list","args":[{"type":"array","args":["string"]}]}'\'' <<< '\''["bar",{}]'\''
json: error: <stdin>:1:7: type check error: too many arguments
json: error: <stdin>:1:7: ["bar",{}]
json: error: <stdin>:1:7:       ^
["bar"
command failed: json -d '\''{"type":"list","args":[{"type":"array","args":["string"]}]}'\'' <<< '\''["bar",{}]'\''
$ json -d '\''{"type":"list","args":[{"type":"array","args":["string"]}]}'\'' <<< '\''["bar",[]]'\''
json: error: <stdin>:1:7: type check error: too many arguments
json: error: <stdin>:1:7: ["bar",[]]
json: error: <stdin>:1:7:       ^
["bar"
command failed: json -d '\''{"type":"list","args":[{"type":"array","args":["string"]}]}'\'' <<< '\''["bar",[]]'\''
$ json -d '\''{"type":"list","args":[{"type":"array","args":["object"]}]}'\'' <<< '\''[{},null]'\''
json: error: <stdin>:1:4: type check error: too many arguments
json: error: <stdin>:1:4: [{},null]
json: error: <stdin>:1:4:    ^
[{}
command failed: json -d '\''{"type":"list","args":[{"type":"array","args":["object"]}]}'\'' <<< '\''[{},null]'\''
$ json -d '\''{"type":"list","args":[{"type":"array","args":["object"]}]}'\'' <<< '\''[{},false]'\''
json: error: <stdin>:1:4: type check error: too many arguments
json: error: <stdin>:1:4: [{},false]
json: error: <stdin>:1:4:    ^
[{}
command failed: json -d '\''{"type":"list","args":[{"type":"array","args":["object"]}]}'\'' <<< '\''[{},false]'\''
$ json -d '\''{"type":"list","args":[{"type":"array","args":["object"]}]}'\'' <<< '\''[{},true]'\''
json: error: <stdin>:1:4: type check error: too many arguments
json: error: <stdin>:1:4: [{},true]
json: error: <stdin>:1:4:    ^
[{}
command failed: json -d '\''{"type":"list","args":[{"type":"array","args":["object"]}]}'\'' <<< '\''[{},true]'\''
$ json -d '\''{"type":"list","args":[{"type":"array","args":["object"]}]}'\'' <<< '\''[{},123]'\''
json: error: <stdin>:1:4: type check error: too many arguments
json: error: <stdin>:1:4: [{},123]
json: error: <stdin>:1:4:    ^
[{}
command failed: json -d '\''{"type":"list","args":[{"type":"array","args":["object"]}]}'\'' <<< '\''[{},123]'\''
$ json -d '\''{"type":"list","args":[{"type":"array","args":["object"]}]}'\'' <<< '\''[{},"foo"]'\''
json: error: <stdin>:1:4: type check error: too many arguments
json: error: <stdin>:1:4: [{},"foo"]
json: error: <stdin>:1:4:    ^
[{}
command failed: json -d '\''{"type":"list","args":[{"type":"array","args":["object"]}]}'\'' <<< '\''[{},"foo"]'\''
$ json -d '\''{"type":"list","args":[{"type":"array","args":["object"]}]}'\'' <<< '\''[{},{}]'\''
json: error: <stdin>:1:4: type check error: too many arguments
json: error: <stdin>:1:4: [{},{}]
json: error: <stdin>:1:4:    ^
[{}
command failed: json -d '\''{"type":"list","args":[{"type":"array","args":["object"]}]}'\'' <<< '\''[{},{}]'\''
$ json -d '\''{"type":"list","args":[{"type":"array","args":["object"]}]}'\'' <<< '\''[{},[]]'\''
json: error: <stdin>:1:4: type check error: too many arguments
json: error: <stdin>:1:4: [{},[]]
json: error: <stdin>:1:4:    ^
[{}
command failed: json -d '\''{"type":"list","args":[{"type":"array","args":["object"]}]}'\'' <<< '\''[{},[]]'\''
$ json -d '\''{"type":"list","args":[{"type":"array","args":["array"]}]}'\'' <<< '\''[[],null]'\''
json: error: <stdin>:1:4: type check error: too many arguments
json: error: <stdin>:1:4: [[],null]
json: error: <stdin>:1:4:    ^
[[]
command failed: json -d '\''{"type":"list","args":[{"type":"array","args":["array"]}]}'\'' <<< '\''[[],null]'\''
$ json -d '\''{"type":"list","args":[{"type":"array","args":["array"]}]}'\'' <<< '\''[[],false]'\''
json: error: <stdin>:1:4: type check error: too many arguments
json: error: <stdin>:1:4: [[],false]
json: error: <stdin>:1:4:    ^
[[]
command failed: json -d '\''{"type":"list","args":[{"type":"array","args":["array"]}]}'\'' <<< '\''[[],false]'\''
$ json -d '\''{"type":"list","args":[{"type":"array","args":["array"]}]}'\'' <<< '\''[[],true]'\''
json: error: <stdin>:1:4: type check error: too many arguments
json: error: <stdin>:1:4: [[],true]
json: error: <stdin>:1:4:    ^
[[]
command failed: json -d '\''{"type":"list","args":[{"type":"array","args":["array"]}]}'\'' <<< '\''[[],true]'\''
$ json -d '\''{"type":"list","args":[{"type":"array","args":["array"]}]}'\'' <<< '\''[[],123]'\''
json: error: <stdin>:1:4: type check error: too many arguments
json: error: <stdin>:1:4: [[],123]
json: error: <stdin>:1:4:    ^
[[]
command failed: json -d '\''{"type":"list","args":[{"type":"array","args":["array"]}]}'\'' <<< '\''[[],123]'\''
$ json -d '\''{"type":"list","args":[{"type":"array","args":["array"]}]}'\'' <<< '\''[[],"foo"]'\''
json: error: <stdin>:1:4: type check error: too many arguments
json: error: <stdin>:1:4: [[],"foo"]
json: error: <stdin>:1:4:    ^
[[]
command failed: json -d '\''{"type":"list","args":[{"type":"array","args":["array"]}]}'\'' <<< '\''[[],"foo"]'\''
$ json -d '\''{"type":"list","args":[{"type":"array","args":["array"]}]}'\'' <<< '\''[[],{}]'\''
json: error: <stdin>:1:4: type check error: too many arguments
json: error: <stdin>:1:4: [[],{}]
json: error: <stdin>:1:4:    ^
[[]
command failed: json -d '\''{"type":"list","args":[{"type":"array","args":["array"]}]}'\'' <<< '\''[[],{}]'\''
$ json -d '\''{"type":"list","args":[{"type":"array","args":["array"]}]}'\'' <<< '\''[[],[]]'\''
json: error: <stdin>:1:4: type check error: too many arguments
json: error: <stdin>:1:4: [[],[]]
json: error: <stdin>:1:4:    ^
[[]
command failed: json -d '\''{"type":"list","args":[{"type":"array","args":["array"]}]}'\'' <<< '\''[[],[]]'\'''
) -L list-closed-array-basic-val-array-basic-basic.new <(
echo '$ export JSON_TYPE_CHECK_ERROR_COMPLETE_TYPES=yes'
export JSON_TYPE_CHECK_ERROR_COMPLETE_TYPES=yes 2>&1 ||
echo 'command failed: export JSON_TYPE_CHECK_ERROR_COMPLETE_TYPES=yes'

echo '$ json() { LD_LIBRARY_PATH=../lib ../src/json --terse --verbose --literal-value "$@"; }'
json() { LD_LIBRARY_PATH=../lib ../src/json --terse --verbose --literal-value "$@"; } 2>&1 ||
echo 'command failed: json() { LD_LIBRARY_PATH=../lib ../src/json --terse --verbose --literal-value "$@"; }'

echo '$ json -d '\''{"type":"list","args":[{"type":"array","args":["type"]}]}'\'' <<< '\''[[{}],null]'\'''
json -d '{"type":"list","args":[{"type":"array","args":["type"]}]}' <<< '[[{}],null]' 2>&1 ||
echo 'command failed: json -d '\''{"type":"list","args":[{"type":"array","args":["type"]}]}'\'' <<< '\''[[{}],null]'\'''

echo '$ json -d '\''{"type":"list","args":[{"type":"array","args":["type"]}]}'\'' <<< '\''[[{}],false]'\'''
json -d '{"type":"list","args":[{"type":"array","args":["type"]}]}' <<< '[[{}],false]' 2>&1 ||
echo 'command failed: json -d '\''{"type":"list","args":[{"type":"array","args":["type"]}]}'\'' <<< '\''[[{}],false]'\'''

echo '$ json -d '\''{"type":"list","args":[{"type":"array","args":["type"]}]}'\'' <<< '\''[[{}],true]'\'''
json -d '{"type":"list","args":[{"type":"array","args":["type"]}]}' <<< '[[{}],true]' 2>&1 ||
echo 'command failed: json -d '\''{"type":"list","args":[{"type":"array","args":["type"]}]}'\'' <<< '\''[[{}],true]'\'''

echo '$ json -d '\''{"type":"list","args":[{"type":"array","args":["type"]}]}'\'' <<< '\''[[{}],123]'\'''
json -d '{"type":"list","args":[{"type":"array","args":["type"]}]}' <<< '[[{}],123]' 2>&1 ||
echo 'command failed: json -d '\''{"type":"list","args":[{"type":"array","args":["type"]}]}'\'' <<< '\''[[{}],123]'\'''

echo '$ json -d '\''{"type":"list","args":[{"type":"array","args":["type"]}]}'\'' <<< '\''[[{}],"foo"]'\'''
json -d '{"type":"list","args":[{"type":"array","args":["type"]}]}' <<< '[[{}],"foo"]' 2>&1 ||
echo 'command failed: json -d '\''{"type":"list","args":[{"type":"array","args":["type"]}]}'\'' <<< '\''[[{}],"foo"]'\'''

echo '$ json -d '\''{"type":"list","args":[{"type":"array","args":["type"]}]}'\'' <<< '\''[[{}],{}]'\'''
json -d '{"type":"list","args":[{"type":"array","args":["type"]}]}' <<< '[[{}],{}]' 2>&1 ||
echo 'command failed: json -d '\''{"type":"list","args":[{"type":"array","args":["type"]}]}'\'' <<< '\''[[{}],{}]'\'''

echo '$ json -d '\''{"type":"list","args":[{"type":"array","args":["type"]}]}'\'' <<< '\''[[{}],[]]'\'''
json -d '{"type":"list","args":[{"type":"array","args":["type"]}]}' <<< '[[{}],[]]' 2>&1 ||
echo 'command failed: json -d '\''{"type":"list","args":[{"type":"array","args":["type"]}]}'\'' <<< '\''[[{}],[]]'\'''

echo '$ json -d '\''{"type":"list","args":[{"type":"array","args":["null"]}]}'\'' <<< '\''[null,null]'\'''
json -d '{"type":"list","args":[{"type":"array","args":["null"]}]}' <<< '[null,null]' 2>&1 ||
echo 'command failed: json -d '\''{"type":"list","args":[{"type":"array","args":["null"]}]}'\'' <<< '\''[null,null]'\'''

echo '$ json -d '\''{"type":"list","args":[{"type":"array","args":["null"]}]}'\'' <<< '\''[null,false]'\'''
json -d '{"type":"list","args":[{"type":"array","args":["null"]}]}' <<< '[null,false]' 2>&1 ||
echo 'command failed: json -d '\''{"type":"list","args":[{"type":"array","args":["null"]}]}'\'' <<< '\''[null,false]'\'''

echo '$ json -d '\''{"type":"list","args":[{"type":"array","args":["null"]}]}'\'' <<< '\''[null,true]'\'''
json -d '{"type":"list","args":[{"type":"array","args":["null"]}]}' <<< '[null,true]' 2>&1 ||
echo 'command failed: json -d '\''{"type":"list","args":[{"type":"array","args":["null"]}]}'\'' <<< '\''[null,true]'\'''

echo '$ json -d '\''{"type":"list","args":[{"type":"array","args":["null"]}]}'\'' <<< '\''[null,123]'\'''
json -d '{"type":"list","args":[{"type":"array","args":["null"]}]}' <<< '[null,123]' 2>&1 ||
echo 'command failed: json -d '\''{"type":"list","args":[{"type":"array","args":["null"]}]}'\'' <<< '\''[null,123]'\'''

echo '$ json -d '\''{"type":"list","args":[{"type":"array","args":["null"]}]}'\'' <<< '\''[null,"foo"]'\'''
json -d '{"type":"list","args":[{"type":"array","args":["null"]}]}' <<< '[null,"foo"]' 2>&1 ||
echo 'command failed: json -d '\''{"type":"list","args":[{"type":"array","args":["null"]}]}'\'' <<< '\''[null,"foo"]'\'''

echo '$ json -d '\''{"type":"list","args":[{"type":"array","args":["null"]}]}'\'' <<< '\''[null,{}]'\'''
json -d '{"type":"list","args":[{"type":"array","args":["null"]}]}' <<< '[null,{}]' 2>&1 ||
echo 'command failed: json -d '\''{"type":"list","args":[{"type":"array","args":["null"]}]}'\'' <<< '\''[null,{}]'\'''

echo '$ json -d '\''{"type":"list","args":[{"type":"array","args":["null"]}]}'\'' <<< '\''[null,[]]'\'''
json -d '{"type":"list","args":[{"type":"array","args":["null"]}]}' <<< '[null,[]]' 2>&1 ||
echo 'command failed: json -d '\''{"type":"list","args":[{"type":"array","args":["null"]}]}'\'' <<< '\''[null,[]]'\'''

echo '$ json -d '\''{"type":"list","args":[{"type":"array","args":["boolean"]}]}'\'' <<< '\''[false,null]'\'''
json -d '{"type":"list","args":[{"type":"array","args":["boolean"]}]}' <<< '[false,null]' 2>&1 ||
echo 'command failed: json -d '\''{"type":"list","args":[{"type":"array","args":["boolean"]}]}'\'' <<< '\''[false,null]'\'''

echo '$ json -d '\''{"type":"list","args":[{"type":"array","args":["boolean"]}]}'\'' <<< '\''[false,false]'\'''
json -d '{"type":"list","args":[{"type":"array","args":["boolean"]}]}' <<< '[false,false]' 2>&1 ||
echo 'command failed: json -d '\''{"type":"list","args":[{"type":"array","args":["boolean"]}]}'\'' <<< '\''[false,false]'\'''

echo '$ json -d '\''{"type":"list","args":[{"type":"array","args":["boolean"]}]}'\'' <<< '\''[false,true]'\'''
json -d '{"type":"list","args":[{"type":"array","args":["boolean"]}]}' <<< '[false,true]' 2>&1 ||
echo 'command failed: json -d '\''{"type":"list","args":[{"type":"array","args":["boolean"]}]}'\'' <<< '\''[false,true]'\'''

echo '$ json -d '\''{"type":"list","args":[{"type":"array","args":["boolean"]}]}'\'' <<< '\''[false,123]'\'''
json -d '{"type":"list","args":[{"type":"array","args":["boolean"]}]}' <<< '[false,123]' 2>&1 ||
echo 'command failed: json -d '\''{"type":"list","args":[{"type":"array","args":["boolean"]}]}'\'' <<< '\''[false,123]'\'''

echo '$ json -d '\''{"type":"list","args":[{"type":"array","args":["boolean"]}]}'\'' <<< '\''[false,"foo"]'\'''
json -d '{"type":"list","args":[{"type":"array","args":["boolean"]}]}' <<< '[false,"foo"]' 2>&1 ||
echo 'command failed: json -d '\''{"type":"list","args":[{"type":"array","args":["boolean"]}]}'\'' <<< '\''[false,"foo"]'\'''

echo '$ json -d '\''{"type":"list","args":[{"type":"array","args":["boolean"]}]}'\'' <<< '\''[false,{}]'\'''
json -d '{"type":"list","args":[{"type":"array","args":["boolean"]}]}' <<< '[false,{}]' 2>&1 ||
echo 'command failed: json -d '\''{"type":"list","args":[{"type":"array","args":["boolean"]}]}'\'' <<< '\''[false,{}]'\'''

echo '$ json -d '\''{"type":"list","args":[{"type":"array","args":["boolean"]}]}'\'' <<< '\''[false,[]]'\'''
json -d '{"type":"list","args":[{"type":"array","args":["boolean"]}]}' <<< '[false,[]]' 2>&1 ||
echo 'command failed: json -d '\''{"type":"list","args":[{"type":"array","args":["boolean"]}]}'\'' <<< '\''[false,[]]'\'''

echo '$ json -d '\''{"type":"list","args":[{"type":"array","args":["number"]}]}'\'' <<< '\''[456,null]'\'''
json -d '{"type":"list","args":[{"type":"array","args":["number"]}]}' <<< '[456,null]' 2>&1 ||
echo 'command failed: json -d '\''{"type":"list","args":[{"type":"array","args":["number"]}]}'\'' <<< '\''[456,null]'\'''

echo '$ json -d '\''{"type":"list","args":[{"type":"array","args":["number"]}]}'\'' <<< '\''[456,false]'\'''
json -d '{"type":"list","args":[{"type":"array","args":["number"]}]}' <<< '[456,false]' 2>&1 ||
echo 'command failed: json -d '\''{"type":"list","args":[{"type":"array","args":["number"]}]}'\'' <<< '\''[456,false]'\'''

echo '$ json -d '\''{"type":"list","args":[{"type":"array","args":["number"]}]}'\'' <<< '\''[456,true]'\'''
json -d '{"type":"list","args":[{"type":"array","args":["number"]}]}' <<< '[456,true]' 2>&1 ||
echo 'command failed: json -d '\''{"type":"list","args":[{"type":"array","args":["number"]}]}'\'' <<< '\''[456,true]'\'''

echo '$ json -d '\''{"type":"list","args":[{"type":"array","args":["number"]}]}'\'' <<< '\''[456,123]'\'''
json -d '{"type":"list","args":[{"type":"array","args":["number"]}]}' <<< '[456,123]' 2>&1 ||
echo 'command failed: json -d '\''{"type":"list","args":[{"type":"array","args":["number"]}]}'\'' <<< '\''[456,123]'\'''

echo '$ json -d '\''{"type":"list","args":[{"type":"array","args":["number"]}]}'\'' <<< '\''[456,"foo"]'\'''
json -d '{"type":"list","args":[{"type":"array","args":["number"]}]}' <<< '[456,"foo"]' 2>&1 ||
echo 'command failed: json -d '\''{"type":"list","args":[{"type":"array","args":["number"]}]}'\'' <<< '\''[456,"foo"]'\'''

echo '$ json -d '\''{"type":"list","args":[{"type":"array","args":["number"]}]}'\'' <<< '\''[456,{}]'\'''
json -d '{"type":"list","args":[{"type":"array","args":["number"]}]}' <<< '[456,{}]' 2>&1 ||
echo 'command failed: json -d '\''{"type":"list","args":[{"type":"array","args":["number"]}]}'\'' <<< '\''[456,{}]'\'''

echo '$ json -d '\''{"type":"list","args":[{"type":"array","args":["number"]}]}'\'' <<< '\''[456,[]]'\'''
json -d '{"type":"list","args":[{"type":"array","args":["number"]}]}' <<< '[456,[]]' 2>&1 ||
echo 'command failed: json -d '\''{"type":"list","args":[{"type":"array","args":["number"]}]}'\'' <<< '\''[456,[]]'\'''

echo '$ json -d '\''{"type":"list","args":[{"type":"array","args":["string"]}]}'\'' <<< '\''["bar",null]'\'''
json -d '{"type":"list","args":[{"type":"array","args":["string"]}]}' <<< '["bar",null]' 2>&1 ||
echo 'command failed: json -d '\''{"type":"list","args":[{"type":"array","args":["string"]}]}'\'' <<< '\''["bar",null]'\'''

echo '$ json -d '\''{"type":"list","args":[{"type":"array","args":["string"]}]}'\'' <<< '\''["bar",false]'\'''
json -d '{"type":"list","args":[{"type":"array","args":["string"]}]}' <<< '["bar",false]' 2>&1 ||
echo 'command failed: json -d '\''{"type":"list","args":[{"type":"array","args":["string"]}]}'\'' <<< '\''["bar",false]'\'''

echo '$ json -d '\''{"type":"list","args":[{"type":"array","args":["string"]}]}'\'' <<< '\''["bar",true]'\'''
json -d '{"type":"list","args":[{"type":"array","args":["string"]}]}' <<< '["bar",true]' 2>&1 ||
echo 'command failed: json -d '\''{"type":"list","args":[{"type":"array","args":["string"]}]}'\'' <<< '\''["bar",true]'\'''

echo '$ json -d '\''{"type":"list","args":[{"type":"array","args":["string"]}]}'\'' <<< '\''["bar",123]'\'''
json -d '{"type":"list","args":[{"type":"array","args":["string"]}]}' <<< '["bar",123]' 2>&1 ||
echo 'command failed: json -d '\''{"type":"list","args":[{"type":"array","args":["string"]}]}'\'' <<< '\''["bar",123]'\'''

echo '$ json -d '\''{"type":"list","args":[{"type":"array","args":["string"]}]}'\'' <<< '\''["bar","foo"]'\'''
json -d '{"type":"list","args":[{"type":"array","args":["string"]}]}' <<< '["bar","foo"]' 2>&1 ||
echo 'command failed: json -d '\''{"type":"list","args":[{"type":"array","args":["string"]}]}'\'' <<< '\''["bar","foo"]'\'''

echo '$ json -d '\''{"type":"list","args":[{"type":"array","args":["string"]}]}'\'' <<< '\''["bar",{}]'\'''
json -d '{"type":"list","args":[{"type":"array","args":["string"]}]}' <<< '["bar",{}]' 2>&1 ||
echo 'command failed: json -d '\''{"type":"list","args":[{"type":"array","args":["string"]}]}'\'' <<< '\''["bar",{}]'\'''

echo '$ json -d '\''{"type":"list","args":[{"type":"array","args":["string"]}]}'\'' <<< '\''["bar",[]]'\'''
json -d '{"type":"list","args":[{"type":"array","args":["string"]}]}' <<< '["bar",[]]' 2>&1 ||
echo 'command failed: json -d '\''{"type":"list","args":[{"type":"array","args":["string"]}]}'\'' <<< '\''["bar",[]]'\'''

echo '$ json -d '\''{"type":"list","args":[{"type":"array","args":["object"]}]}'\'' <<< '\''[{},null]'\'''
json -d '{"type":"list","args":[{"type":"array","args":["object"]}]}' <<< '[{},null]' 2>&1 ||
echo 'command failed: json -d '\''{"type":"list","args":[{"type":"array","args":["object"]}]}'\'' <<< '\''[{},null]'\'''

echo '$ json -d '\''{"type":"list","args":[{"type":"array","args":["object"]}]}'\'' <<< '\''[{},false]'\'''
json -d '{"type":"list","args":[{"type":"array","args":["object"]}]}' <<< '[{},false]' 2>&1 ||
echo 'command failed: json -d '\''{"type":"list","args":[{"type":"array","args":["object"]}]}'\'' <<< '\''[{},false]'\'''

echo '$ json -d '\''{"type":"list","args":[{"type":"array","args":["object"]}]}'\'' <<< '\''[{},true]'\'''
json -d '{"type":"list","args":[{"type":"array","args":["object"]}]}' <<< '[{},true]' 2>&1 ||
echo 'command failed: json -d '\''{"type":"list","args":[{"type":"array","args":["object"]}]}'\'' <<< '\''[{},true]'\'''

echo '$ json -d '\''{"type":"list","args":[{"type":"array","args":["object"]}]}'\'' <<< '\''[{},123]'\'''
json -d '{"type":"list","args":[{"type":"array","args":["object"]}]}' <<< '[{},123]' 2>&1 ||
echo 'command failed: json -d '\''{"type":"list","args":[{"type":"array","args":["object"]}]}'\'' <<< '\''[{},123]'\'''

echo '$ json -d '\''{"type":"list","args":[{"type":"array","args":["object"]}]}'\'' <<< '\''[{},"foo"]'\'''
json -d '{"type":"list","args":[{"type":"array","args":["object"]}]}' <<< '[{},"foo"]' 2>&1 ||
echo 'command failed: json -d '\''{"type":"list","args":[{"type":"array","args":["object"]}]}'\'' <<< '\''[{},"foo"]'\'''

echo '$ json -d '\''{"type":"list","args":[{"type":"array","args":["object"]}]}'\'' <<< '\''[{},{}]'\'''
json -d '{"type":"list","args":[{"type":"array","args":["object"]}]}' <<< '[{},{}]' 2>&1 ||
echo 'command failed: json -d '\''{"type":"list","args":[{"type":"array","args":["object"]}]}'\'' <<< '\''[{},{}]'\'''

echo '$ json -d '\''{"type":"list","args":[{"type":"array","args":["object"]}]}'\'' <<< '\''[{},[]]'\'''
json -d '{"type":"list","args":[{"type":"array","args":["object"]}]}' <<< '[{},[]]' 2>&1 ||
echo 'command failed: json -d '\''{"type":"list","args":[{"type":"array","args":["object"]}]}'\'' <<< '\''[{},[]]'\'''

echo '$ json -d '\''{"type":"list","args":[{"type":"array","args":["array"]}]}'\'' <<< '\''[[],null]'\'''
json -d '{"type":"list","args":[{"type":"array","args":["array"]}]}' <<< '[[],null]' 2>&1 ||
echo 'command failed: json -d '\''{"type":"list","args":[{"type":"array","args":["array"]}]}'\'' <<< '\''[[],null]'\'''

echo '$ json -d '\''{"type":"list","args":[{"type":"array","args":["array"]}]}'\'' <<< '\''[[],false]'\'''
json -d '{"type":"list","args":[{"type":"array","args":["array"]}]}' <<< '[[],false]' 2>&1 ||
echo 'command failed: json -d '\''{"type":"list","args":[{"type":"array","args":["array"]}]}'\'' <<< '\''[[],false]'\'''

echo '$ json -d '\''{"type":"list","args":[{"type":"array","args":["array"]}]}'\'' <<< '\''[[],true]'\'''
json -d '{"type":"list","args":[{"type":"array","args":["array"]}]}' <<< '[[],true]' 2>&1 ||
echo 'command failed: json -d '\''{"type":"list","args":[{"type":"array","args":["array"]}]}'\'' <<< '\''[[],true]'\'''

echo '$ json -d '\''{"type":"list","args":[{"type":"array","args":["array"]}]}'\'' <<< '\''[[],123]'\'''
json -d '{"type":"list","args":[{"type":"array","args":["array"]}]}' <<< '[[],123]' 2>&1 ||
echo 'command failed: json -d '\''{"type":"list","args":[{"type":"array","args":["array"]}]}'\'' <<< '\''[[],123]'\'''

echo '$ json -d '\''{"type":"list","args":[{"type":"array","args":["array"]}]}'\'' <<< '\''[[],"foo"]'\'''
json -d '{"type":"list","args":[{"type":"array","args":["array"]}]}' <<< '[[],"foo"]' 2>&1 ||
echo 'command failed: json -d '\''{"type":"list","args":[{"type":"array","args":["array"]}]}'\'' <<< '\''[[],"foo"]'\'''

echo '$ json -d '\''{"type":"list","args":[{"type":"array","args":["array"]}]}'\'' <<< '\''[[],{}]'\'''
json -d '{"type":"list","args":[{"type":"array","args":["array"]}]}' <<< '[[],{}]' 2>&1 ||
echo 'command failed: json -d '\''{"type":"list","args":[{"type":"array","args":["array"]}]}'\'' <<< '\''[[],{}]'\'''

echo '$ json -d '\''{"type":"list","args":[{"type":"array","args":["array"]}]}'\'' <<< '\''[[],[]]'\'''
json -d '{"type":"list","args":[{"type":"array","args":["array"]}]}' <<< '[[],[]]' 2>&1 ||
echo 'command failed: json -d '\''{"type":"list","args":[{"type":"array","args":["array"]}]}'\'' <<< '\''[[],[]]'\'''
)

