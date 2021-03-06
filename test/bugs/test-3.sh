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
# $ json-gentest -C bugs:3
#

[[ "$1" =~ ^-u[0-9]+$ ]] &&
u="${1:2}" ||
u=""

diff -u$u -L 3.old <(echo \
'$ json() { set -o pipefail && LD_LIBRARY_PATH=../lib ../src/json --terse --verbose --literal-value "$@" 2>&1|sed -r '\''/error:/s|(/dev/fd/)[0-9]+|\1??|'\''; }
$ json -t <(echo '\''{"type":"list","args":[{"type":"object","args":[{"name":"foo","type":"boolean"}]},{"type":"object","args":[{"name":"foo","type":{"type":"array","args":[]}}]},{"type":"object","args":[{"name":"foo","type":{"type":"array","args":["number"]}}]}]}'\'') <<< '\''{"foo":false}'\''
json: error: /dev/fd/??:1:129: type lib error: attribute error: invalid "list" type object: sort of object of array type not yet supported (the other inner array is at 1:205)
command failed: json -t <(echo '\''{"type":"list","args":[{"type":"object","args":[{"name":"foo","type":"boolean"}]},{"type":"object","args":[{"name":"foo","type":{"type":"array","args":[]}}]},{"type":"object","args":[{"name":"foo","type":{"type":"array","args":["number"]}}]}]}'\'') <<< '\''{"foo":false}'\''
$ json -t <(echo '\''{"type":"list","args":[{"type":"object","args":[{"name":"foo","type":"boolean"}]},{"type":"object","args":[{"name":"foo","type":{"type":"array","args":[]}}]},{"type":"object","args":[{"name":"foo","type":{"type":"array","args":["number"]}}]}]}'\'') <<< '\''{"foo":[]}'\''
json: error: /dev/fd/??:1:129: type lib error: attribute error: invalid "list" type object: sort of object of array type not yet supported (the other inner array is at 1:205)
command failed: json -t <(echo '\''{"type":"list","args":[{"type":"object","args":[{"name":"foo","type":"boolean"}]},{"type":"object","args":[{"name":"foo","type":{"type":"array","args":[]}}]},{"type":"object","args":[{"name":"foo","type":{"type":"array","args":["number"]}}]}]}'\'') <<< '\''{"foo":[]}'\''
$ json -t <(echo '\''{"type":"list","args":[{"type":"object","args":[{"name":"foo","type":"boolean"}]},{"type":"object","args":[{"name":"foo","type":{"type":"array","args":[]}}]},{"type":"object","args":[{"name":"foo","type":{"type":"array","args":["number"]}}]}]}'\'') <<< '\''{"foo":[1]}'\''
json: error: /dev/fd/??:1:129: type lib error: attribute error: invalid "list" type object: sort of object of array type not yet supported (the other inner array is at 1:205)
command failed: json -t <(echo '\''{"type":"list","args":[{"type":"object","args":[{"name":"foo","type":"boolean"}]},{"type":"object","args":[{"name":"foo","type":{"type":"array","args":[]}}]},{"type":"object","args":[{"name":"foo","type":{"type":"array","args":["number"]}}]}]}'\'') <<< '\''{"foo":[1]}'\'''
) -L 3.new <(
echo '$ json() { set -o pipefail && LD_LIBRARY_PATH=../lib ../src/json --terse --verbose --literal-value "$@" 2>&1|sed -r '\''/error:/s|(/dev/fd/)[0-9]+|\1??|'\''; }'
json() { set -o pipefail && LD_LIBRARY_PATH=../lib ../src/json --terse --verbose --literal-value "$@" 2>&1|sed -r '/error:/s|(/dev/fd/)[0-9]+|\1??|'; } 2>&1 ||
echo 'command failed: json() { set -o pipefail && LD_LIBRARY_PATH=../lib ../src/json --terse --verbose --literal-value "$@" 2>&1|sed -r '\''/error:/s|(/dev/fd/)[0-9]+|\1??|'\''; }'

echo '$ json -t <(echo '\''{"type":"list","args":[{"type":"object","args":[{"name":"foo","type":"boolean"}]},{"type":"object","args":[{"name":"foo","type":{"type":"array","args":[]}}]},{"type":"object","args":[{"name":"foo","type":{"type":"array","args":["number"]}}]}]}'\'') <<< '\''{"foo":false}'\'''
json -t <(echo '{"type":"list","args":[{"type":"object","args":[{"name":"foo","type":"boolean"}]},{"type":"object","args":[{"name":"foo","type":{"type":"array","args":[]}}]},{"type":"object","args":[{"name":"foo","type":{"type":"array","args":["number"]}}]}]}') <<< '{"foo":false}' 2>&1 ||
echo 'command failed: json -t <(echo '\''{"type":"list","args":[{"type":"object","args":[{"name":"foo","type":"boolean"}]},{"type":"object","args":[{"name":"foo","type":{"type":"array","args":[]}}]},{"type":"object","args":[{"name":"foo","type":{"type":"array","args":["number"]}}]}]}'\'') <<< '\''{"foo":false}'\'''

echo '$ json -t <(echo '\''{"type":"list","args":[{"type":"object","args":[{"name":"foo","type":"boolean"}]},{"type":"object","args":[{"name":"foo","type":{"type":"array","args":[]}}]},{"type":"object","args":[{"name":"foo","type":{"type":"array","args":["number"]}}]}]}'\'') <<< '\''{"foo":[]}'\'''
json -t <(echo '{"type":"list","args":[{"type":"object","args":[{"name":"foo","type":"boolean"}]},{"type":"object","args":[{"name":"foo","type":{"type":"array","args":[]}}]},{"type":"object","args":[{"name":"foo","type":{"type":"array","args":["number"]}}]}]}') <<< '{"foo":[]}' 2>&1 ||
echo 'command failed: json -t <(echo '\''{"type":"list","args":[{"type":"object","args":[{"name":"foo","type":"boolean"}]},{"type":"object","args":[{"name":"foo","type":{"type":"array","args":[]}}]},{"type":"object","args":[{"name":"foo","type":{"type":"array","args":["number"]}}]}]}'\'') <<< '\''{"foo":[]}'\'''

echo '$ json -t <(echo '\''{"type":"list","args":[{"type":"object","args":[{"name":"foo","type":"boolean"}]},{"type":"object","args":[{"name":"foo","type":{"type":"array","args":[]}}]},{"type":"object","args":[{"name":"foo","type":{"type":"array","args":["number"]}}]}]}'\'') <<< '\''{"foo":[1]}'\'''
json -t <(echo '{"type":"list","args":[{"type":"object","args":[{"name":"foo","type":"boolean"}]},{"type":"object","args":[{"name":"foo","type":{"type":"array","args":[]}}]},{"type":"object","args":[{"name":"foo","type":{"type":"array","args":["number"]}}]}]}') <<< '{"foo":[1]}' 2>&1 ||
echo 'command failed: json -t <(echo '\''{"type":"list","args":[{"type":"object","args":[{"name":"foo","type":"boolean"}]},{"type":"object","args":[{"name":"foo","type":{"type":"array","args":[]}}]},{"type":"object","args":[{"name":"foo","type":{"type":"array","args":["number"]}}]}]}'\'') <<< '\''{"foo":[1]}'\'''
)

