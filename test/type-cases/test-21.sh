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
# $ json-gentest -C type-cases:21
#

[[ "$1" =~ ^-u[0-9]+$ ]] &&
u="${1:2}" ||
u=""

diff -u$u -L 21.old <(echo \
'$ json() { set -o pipefail && LD_LIBRARY_PATH=../lib ../src/json --terse --verbose --literal-value "$@" 2>&1|sed -r '\''/error:/s|(/dev/fd/)[0-9]+|\1??|'\''; }
$ json -d '\''{"type":"list","args":[{"type":"array","args":"number"},{"type":"array","args":["number","string"]},{"type":"array","args":["number","array"]}]}'\'' <<< '\''[0,false]'\''
json: error: <stdin>:1:4: type check error: type mismatch: expected a value of type `"number"'\'', `"string"'\'' or `"array"'\''
json: error: <stdin>:1:4: [0,false]
json: error: <stdin>:1:4:    ^
[0,
command failed: json -d '\''{"type":"list","args":[{"type":"array","args":"number"},{"type":"array","args":["number","string"]},{"type":"array","args":["number","array"]}]}'\'' <<< '\''[0,false]'\'''
) -L 21.new <(
echo '$ json() { set -o pipefail && LD_LIBRARY_PATH=../lib ../src/json --terse --verbose --literal-value "$@" 2>&1|sed -r '\''/error:/s|(/dev/fd/)[0-9]+|\1??|'\''; }'
json() { set -o pipefail && LD_LIBRARY_PATH=../lib ../src/json --terse --verbose --literal-value "$@" 2>&1|sed -r '/error:/s|(/dev/fd/)[0-9]+|\1??|'; } 2>&1 ||
echo 'command failed: json() { set -o pipefail && LD_LIBRARY_PATH=../lib ../src/json --terse --verbose --literal-value "$@" 2>&1|sed -r '\''/error:/s|(/dev/fd/)[0-9]+|\1??|'\''; }'

echo '$ json -d '\''{"type":"list","args":[{"type":"array","args":"number"},{"type":"array","args":["number","string"]},{"type":"array","args":["number","array"]}]}'\'' <<< '\''[0,false]'\'''
json -d '{"type":"list","args":[{"type":"array","args":"number"},{"type":"array","args":["number","string"]},{"type":"array","args":["number","array"]}]}' <<< '[0,false]' 2>&1 ||
echo 'command failed: json -d '\''{"type":"list","args":[{"type":"array","args":"number"},{"type":"array","args":["number","string"]},{"type":"array","args":["number","array"]}]}'\'' <<< '\''[0,false]'\'''
)

