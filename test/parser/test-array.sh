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
# $ json-gentest -C parser:array
#

[[ "$1" =~ ^-u[0-9]+$ ]] &&
u="${1:2}" ||
u=""

diff -u$u -L array.old <(echo \
'$ json0() { LD_LIBRARY_PATH=../lib ../src/json --pretty --verbose "$@"; }
$ json() { json0 --terse "$@"; }
$ json <<< '\''[]'\''
[]
$ json <<< '\''[null]'\''
[null]
$ json <<< '\''[false]'\''
[false]
$ json <<< '\''[true]'\''
[true]
$ json <<< '\''[1]'\''
[1]
$ json <<< '\''[""]'\''
[""]
$ json <<< '\''["foo"]'\''
["foo"]
$ json <<< '\''[[]]'\''
[[]]
$ json <<< '\''[[[[]]]]'\''
[[[[]]]]
$ json <<< '\''[[[[[[[[]]]]]]]]'\''
[[[[[[[[]]]]]]]]
$ json <<< '\''[[[[[[[[[[[[[[[[]]]]]]]]]]]]]]]]'\''
[[[[[[[[[[[[[[[[]]]]]]]]]]]]]]]]
$ json <<< '\''[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]'\''
[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
$ json <<< '\''[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]'\''
[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
$ json <<< '\''[null,null]'\''
[null,null]
$ json <<< '\''[null,false]'\''
[null,false]
$ json <<< '\''[null,true]'\''
[null,true]
$ json <<< '\''[null,123]'\''
[null,123]
$ json <<< '\''[null,"foo"]'\''
[null,"foo"]
$ json <<< '\''[null,[]]'\''
[null,[]]
$ json <<< '\''[false,null]'\''
[false,null]
$ json <<< '\''[true,null]'\''
[true,null]
$ json <<< '\''[123,null]'\''
[123,null]
$ json <<< '\''["foo",null]'\''
["foo",null]
$ json <<< '\''[[],null]'\''
[[],null]
$ json <<< '\''[0]'\''
[0]
$ json <<< '\''[0,1]'\''
[0,1]
$ json <<< '\''[0,1,2]'\''
[0,1,2]
$ json <<< '\''[0,1,2,3]'\''
[0,1,2,3]
$ json <<< '\''[0,1,2,3,4]'\''
[0,1,2,3,4]
$ json <<< '\''[0,1,2,3,4,5]'\''
[0,1,2,3,4,5]
$ json <<< '\''[0,1,2,3,4,5,6]'\''
[0,1,2,3,4,5,6]
$ json <<< '\''[0,1,2,3,4,5,6,7]'\''
[0,1,2,3,4,5,6,7]
$ json <<< '\''[0,1,2,3,4,5,6,7,8]'\''
[0,1,2,3,4,5,6,7,8]
$ json <<< '\''[0,1,2,3,4,5,6,7,8,9]'\''
[0,1,2,3,4,5,6,7,8,9]
$ json <<< '\''[0,1,2,3,4,5,6,7,8,9,10]'\''
[0,1,2,3,4,5,6,7,8,9,10]
$ json <<< '\''[0,1,2,3,4,5,6,7,8,9,10,11]'\''
[0,1,2,3,4,5,6,7,8,9,10,11]
$ json <<< '\''[0,1,2,3,4,5,6,7,8,9,10,11,12]'\''
[0,1,2,3,4,5,6,7,8,9,10,11,12]
$ json <<< '\''[0,1,2,3,4,5,6,7,8,9,10,11,12,13]'\''
[0,1,2,3,4,5,6,7,8,9,10,11,12,13]
$ json <<< '\''[0,1,2,3,4,5,6,7,8,9,10,11,12,13,14]'\''
[0,1,2,3,4,5,6,7,8,9,10,11,12,13,14]
$ json <<< '\''[0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15]'\''
[0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15]'
) -L array.new <(
echo '$ json0() { LD_LIBRARY_PATH=../lib ../src/json --pretty --verbose "$@"; }'
json0() { LD_LIBRARY_PATH=../lib ../src/json --pretty --verbose "$@"; } 2>&1 ||
echo 'command failed: json0() { LD_LIBRARY_PATH=../lib ../src/json --pretty --verbose "$@"; }'

echo '$ json() { json0 --terse "$@"; }'
json() { json0 --terse "$@"; } 2>&1 ||
echo 'command failed: json() { json0 --terse "$@"; }'

echo '$ json <<< '\''[]'\'''
json <<< '[]' 2>&1 ||
echo 'command failed: json <<< '\''[]'\'''

echo '$ json <<< '\''[null]'\'''
json <<< '[null]' 2>&1 ||
echo 'command failed: json <<< '\''[null]'\'''

echo '$ json <<< '\''[false]'\'''
json <<< '[false]' 2>&1 ||
echo 'command failed: json <<< '\''[false]'\'''

echo '$ json <<< '\''[true]'\'''
json <<< '[true]' 2>&1 ||
echo 'command failed: json <<< '\''[true]'\'''

echo '$ json <<< '\''[1]'\'''
json <<< '[1]' 2>&1 ||
echo 'command failed: json <<< '\''[1]'\'''

echo '$ json <<< '\''[""]'\'''
json <<< '[""]' 2>&1 ||
echo 'command failed: json <<< '\''[""]'\'''

echo '$ json <<< '\''["foo"]'\'''
json <<< '["foo"]' 2>&1 ||
echo 'command failed: json <<< '\''["foo"]'\'''

echo '$ json <<< '\''[[]]'\'''
json <<< '[[]]' 2>&1 ||
echo 'command failed: json <<< '\''[[]]'\'''

echo '$ json <<< '\''[[[[]]]]'\'''
json <<< '[[[[]]]]' 2>&1 ||
echo 'command failed: json <<< '\''[[[[]]]]'\'''

echo '$ json <<< '\''[[[[[[[[]]]]]]]]'\'''
json <<< '[[[[[[[[]]]]]]]]' 2>&1 ||
echo 'command failed: json <<< '\''[[[[[[[[]]]]]]]]'\'''

echo '$ json <<< '\''[[[[[[[[[[[[[[[[]]]]]]]]]]]]]]]]'\'''
json <<< '[[[[[[[[[[[[[[[[]]]]]]]]]]]]]]]]' 2>&1 ||
echo 'command failed: json <<< '\''[[[[[[[[[[[[[[[[]]]]]]]]]]]]]]]]'\'''

echo '$ json <<< '\''[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]'\'''
json <<< '[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]' 2>&1 ||
echo 'command failed: json <<< '\''[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]'\'''

echo '$ json <<< '\''[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]'\'''
json <<< '[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]' 2>&1 ||
echo 'command failed: json <<< '\''[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]'\'''

echo '$ json <<< '\''[null,null]'\'''
json <<< '[null,null]' 2>&1 ||
echo 'command failed: json <<< '\''[null,null]'\'''

echo '$ json <<< '\''[null,false]'\'''
json <<< '[null,false]' 2>&1 ||
echo 'command failed: json <<< '\''[null,false]'\'''

echo '$ json <<< '\''[null,true]'\'''
json <<< '[null,true]' 2>&1 ||
echo 'command failed: json <<< '\''[null,true]'\'''

echo '$ json <<< '\''[null,123]'\'''
json <<< '[null,123]' 2>&1 ||
echo 'command failed: json <<< '\''[null,123]'\'''

echo '$ json <<< '\''[null,"foo"]'\'''
json <<< '[null,"foo"]' 2>&1 ||
echo 'command failed: json <<< '\''[null,"foo"]'\'''

echo '$ json <<< '\''[null,[]]'\'''
json <<< '[null,[]]' 2>&1 ||
echo 'command failed: json <<< '\''[null,[]]'\'''

echo '$ json <<< '\''[false,null]'\'''
json <<< '[false,null]' 2>&1 ||
echo 'command failed: json <<< '\''[false,null]'\'''

echo '$ json <<< '\''[true,null]'\'''
json <<< '[true,null]' 2>&1 ||
echo 'command failed: json <<< '\''[true,null]'\'''

echo '$ json <<< '\''[123,null]'\'''
json <<< '[123,null]' 2>&1 ||
echo 'command failed: json <<< '\''[123,null]'\'''

echo '$ json <<< '\''["foo",null]'\'''
json <<< '["foo",null]' 2>&1 ||
echo 'command failed: json <<< '\''["foo",null]'\'''

echo '$ json <<< '\''[[],null]'\'''
json <<< '[[],null]' 2>&1 ||
echo 'command failed: json <<< '\''[[],null]'\'''

echo '$ json <<< '\''[0]'\'''
json <<< '[0]' 2>&1 ||
echo 'command failed: json <<< '\''[0]'\'''

echo '$ json <<< '\''[0,1]'\'''
json <<< '[0,1]' 2>&1 ||
echo 'command failed: json <<< '\''[0,1]'\'''

echo '$ json <<< '\''[0,1,2]'\'''
json <<< '[0,1,2]' 2>&1 ||
echo 'command failed: json <<< '\''[0,1,2]'\'''

echo '$ json <<< '\''[0,1,2,3]'\'''
json <<< '[0,1,2,3]' 2>&1 ||
echo 'command failed: json <<< '\''[0,1,2,3]'\'''

echo '$ json <<< '\''[0,1,2,3,4]'\'''
json <<< '[0,1,2,3,4]' 2>&1 ||
echo 'command failed: json <<< '\''[0,1,2,3,4]'\'''

echo '$ json <<< '\''[0,1,2,3,4,5]'\'''
json <<< '[0,1,2,3,4,5]' 2>&1 ||
echo 'command failed: json <<< '\''[0,1,2,3,4,5]'\'''

echo '$ json <<< '\''[0,1,2,3,4,5,6]'\'''
json <<< '[0,1,2,3,4,5,6]' 2>&1 ||
echo 'command failed: json <<< '\''[0,1,2,3,4,5,6]'\'''

echo '$ json <<< '\''[0,1,2,3,4,5,6,7]'\'''
json <<< '[0,1,2,3,4,5,6,7]' 2>&1 ||
echo 'command failed: json <<< '\''[0,1,2,3,4,5,6,7]'\'''

echo '$ json <<< '\''[0,1,2,3,4,5,6,7,8]'\'''
json <<< '[0,1,2,3,4,5,6,7,8]' 2>&1 ||
echo 'command failed: json <<< '\''[0,1,2,3,4,5,6,7,8]'\'''

echo '$ json <<< '\''[0,1,2,3,4,5,6,7,8,9]'\'''
json <<< '[0,1,2,3,4,5,6,7,8,9]' 2>&1 ||
echo 'command failed: json <<< '\''[0,1,2,3,4,5,6,7,8,9]'\'''

echo '$ json <<< '\''[0,1,2,3,4,5,6,7,8,9,10]'\'''
json <<< '[0,1,2,3,4,5,6,7,8,9,10]' 2>&1 ||
echo 'command failed: json <<< '\''[0,1,2,3,4,5,6,7,8,9,10]'\'''

echo '$ json <<< '\''[0,1,2,3,4,5,6,7,8,9,10,11]'\'''
json <<< '[0,1,2,3,4,5,6,7,8,9,10,11]' 2>&1 ||
echo 'command failed: json <<< '\''[0,1,2,3,4,5,6,7,8,9,10,11]'\'''

echo '$ json <<< '\''[0,1,2,3,4,5,6,7,8,9,10,11,12]'\'''
json <<< '[0,1,2,3,4,5,6,7,8,9,10,11,12]' 2>&1 ||
echo 'command failed: json <<< '\''[0,1,2,3,4,5,6,7,8,9,10,11,12]'\'''

echo '$ json <<< '\''[0,1,2,3,4,5,6,7,8,9,10,11,12,13]'\'''
json <<< '[0,1,2,3,4,5,6,7,8,9,10,11,12,13]' 2>&1 ||
echo 'command failed: json <<< '\''[0,1,2,3,4,5,6,7,8,9,10,11,12,13]'\'''

echo '$ json <<< '\''[0,1,2,3,4,5,6,7,8,9,10,11,12,13,14]'\'''
json <<< '[0,1,2,3,4,5,6,7,8,9,10,11,12,13,14]' 2>&1 ||
echo 'command failed: json <<< '\''[0,1,2,3,4,5,6,7,8,9,10,11,12,13,14]'\'''

echo '$ json <<< '\''[0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15]'\'''
json <<< '[0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15]' 2>&1 ||
echo 'command failed: json <<< '\''[0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15]'\'''
)

