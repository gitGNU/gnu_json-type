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
# $ json-gentest -C type-lib:plain2
#

[[ "$1" =~ ^-u[0-9]+$ ]] &&
u="${1:2}" ||
u=""

diff -u$u -L plain2.old <(echo \
'$ json() { set -o pipefail && LD_LIBRARY_PATH=../lib ../src/json --literal-value -V -TA "$@"|LD_LIBRARY_PATH=../lib ../src/json --from-ast-print --verbose --no-error; }
$ json <<< '\''{"plain":null,"foo":"bar"}'\''
json: error: <stdin>:1:1: meta error: invalid "plain" object: arguments not of size one
json: error: <stdin>:1:1: {"plain":null,"foo":"bar"}
json: error: <stdin>:1:1: ^
command failed: json <<< '\''{"plain":null,"foo":"bar"}'\''
$ json <<< '\''{"plain":false,"foo":"bar"}'\''
json: error: <stdin>:1:1: meta error: invalid "plain" object: arguments not of size one
json: error: <stdin>:1:1: {"plain":false,"foo":"bar"}
json: error: <stdin>:1:1: ^
command failed: json <<< '\''{"plain":false,"foo":"bar"}'\''
$ json <<< '\''{"plain":true,"foo":"bar"}'\''
json: error: <stdin>:1:1: meta error: invalid "plain" object: arguments not of size one
json: error: <stdin>:1:1: {"plain":true,"foo":"bar"}
json: error: <stdin>:1:1: ^
command failed: json <<< '\''{"plain":true,"foo":"bar"}'\''
$ json <<< '\''{"plain":123,"foo":"bar"}'\''
json: error: <stdin>:1:1: meta error: invalid "plain" object: arguments not of size one
json: error: <stdin>:1:1: {"plain":123,"foo":"bar"}
json: error: <stdin>:1:1: ^
command failed: json <<< '\''{"plain":123,"foo":"bar"}'\''
$ json <<< '\''{"plain":"foo","foo":"bar"}'\''
json: error: <stdin>:1:1: meta error: invalid "plain" object: arguments not of size one
json: error: <stdin>:1:1: {"plain":"foo","foo":"bar"}
json: error: <stdin>:1:1: ^
command failed: json <<< '\''{"plain":"foo","foo":"bar"}'\''
$ json <<< '\''{"plain":"type","foo":"bar"}'\''
json: error: <stdin>:1:1: meta error: invalid "plain" object: arguments not of size one
json: error: <stdin>:1:1: {"plain":"type","foo":"bar"}
json: error: <stdin>:1:1: ^
command failed: json <<< '\''{"plain":"type","foo":"bar"}'\''
$ json <<< '\''{"plain":"null","foo":"bar"}'\''
json: error: <stdin>:1:1: meta error: invalid "plain" object: arguments not of size one
json: error: <stdin>:1:1: {"plain":"null","foo":"bar"}
json: error: <stdin>:1:1: ^
command failed: json <<< '\''{"plain":"null","foo":"bar"}'\''
$ json <<< '\''{"plain":"boolean","foo":"bar"}'\''
json: error: <stdin>:1:1: meta error: invalid "plain" object: arguments not of size one
json: error: <stdin>:1:1: {"plain":"boolean","foo":"bar"}
json: error: <stdin>:1:1: ^
command failed: json <<< '\''{"plain":"boolean","foo":"bar"}'\''
$ json <<< '\''{"plain":"number","foo":"bar"}'\''
json: error: <stdin>:1:1: meta error: invalid "plain" object: arguments not of size one
json: error: <stdin>:1:1: {"plain":"number","foo":"bar"}
json: error: <stdin>:1:1: ^
command failed: json <<< '\''{"plain":"number","foo":"bar"}'\''
$ json <<< '\''{"plain":"string","foo":"bar"}'\''
json: error: <stdin>:1:1: meta error: invalid "plain" object: arguments not of size one
json: error: <stdin>:1:1: {"plain":"string","foo":"bar"}
json: error: <stdin>:1:1: ^
command failed: json <<< '\''{"plain":"string","foo":"bar"}'\''
$ json <<< '\''{"plain":"object","foo":"bar"}'\''
json: error: <stdin>:1:1: meta error: invalid "plain" object: arguments not of size one
json: error: <stdin>:1:1: {"plain":"object","foo":"bar"}
json: error: <stdin>:1:1: ^
command failed: json <<< '\''{"plain":"object","foo":"bar"}'\''
$ json <<< '\''{"plain":"array","foo":"bar"}'\''
json: error: <stdin>:1:1: meta error: invalid "plain" object: arguments not of size one
json: error: <stdin>:1:1: {"plain":"array","foo":"bar"}
json: error: <stdin>:1:1: ^
command failed: json <<< '\''{"plain":"array","foo":"bar"}'\''
$ json <<< '\''{"plain":"list","foo":"bar"}'\''
json: error: <stdin>:1:1: meta error: invalid "plain" object: arguments not of size one
json: error: <stdin>:1:1: {"plain":"list","foo":"bar"}
json: error: <stdin>:1:1: ^
command failed: json <<< '\''{"plain":"list","foo":"bar"}'\''
$ json <<< '\''{"plain":"dict","foo":"bar"}'\''
json: error: <stdin>:1:1: meta error: invalid "plain" object: arguments not of size one
json: error: <stdin>:1:1: {"plain":"dict","foo":"bar"}
json: error: <stdin>:1:1: ^
command failed: json <<< '\''{"plain":"dict","foo":"bar"}'\''
$ json <<< '\''{"plain":{},"foo":"bar"}'\''
json: error: <stdin>:1:1: meta error: invalid "plain" object: arguments not of size one
json: error: <stdin>:1:1: {"plain":{},"foo":"bar"}
json: error: <stdin>:1:1: ^
command failed: json <<< '\''{"plain":{},"foo":"bar"}'\''
$ json <<< '\''{"plain":{"foo":"bar"},"foo":"bar"}'\''
json: error: <stdin>:1:1: meta error: invalid "plain" object: arguments not of size one
json: error: <stdin>:1:1: {"plain":{"foo":"bar"},"foo":"bar
json: error: <stdin>:1:1: ^
command failed: json <<< '\''{"plain":{"foo":"bar"},"foo":"bar"}'\''
$ json <<< '\''{"plain":[],"foo":"bar"}'\''
json: error: <stdin>:1:1: meta error: invalid "plain" object: arguments not of size one
json: error: <stdin>:1:1: {"plain":[],"foo":"bar"}
json: error: <stdin>:1:1: ^
command failed: json <<< '\''{"plain":[],"foo":"bar"}'\''
$ json <<< '\''{"plain":["foo","bar"],"foo":"bar"}'\''
json: error: <stdin>:1:1: meta error: invalid "plain" object: arguments not of size one
json: error: <stdin>:1:1: {"plain":["foo","bar"],"foo":"bar
json: error: <stdin>:1:1: ^
command failed: json <<< '\''{"plain":["foo","bar"],"foo":"bar"}'\'''
) -L plain2.new <(
echo '$ json() { set -o pipefail && LD_LIBRARY_PATH=../lib ../src/json --literal-value -V -TA "$@"|LD_LIBRARY_PATH=../lib ../src/json --from-ast-print --verbose --no-error; }'
json() { set -o pipefail && LD_LIBRARY_PATH=../lib ../src/json --literal-value -V -TA "$@"|LD_LIBRARY_PATH=../lib ../src/json --from-ast-print --verbose --no-error; } 2>&1 ||
echo 'command failed: json() { set -o pipefail && LD_LIBRARY_PATH=../lib ../src/json --literal-value -V -TA "$@"|LD_LIBRARY_PATH=../lib ../src/json --from-ast-print --verbose --no-error; }'

echo '$ json <<< '\''{"plain":null,"foo":"bar"}'\'''
json <<< '{"plain":null,"foo":"bar"}' 2>&1 ||
echo 'command failed: json <<< '\''{"plain":null,"foo":"bar"}'\'''

echo '$ json <<< '\''{"plain":false,"foo":"bar"}'\'''
json <<< '{"plain":false,"foo":"bar"}' 2>&1 ||
echo 'command failed: json <<< '\''{"plain":false,"foo":"bar"}'\'''

echo '$ json <<< '\''{"plain":true,"foo":"bar"}'\'''
json <<< '{"plain":true,"foo":"bar"}' 2>&1 ||
echo 'command failed: json <<< '\''{"plain":true,"foo":"bar"}'\'''

echo '$ json <<< '\''{"plain":123,"foo":"bar"}'\'''
json <<< '{"plain":123,"foo":"bar"}' 2>&1 ||
echo 'command failed: json <<< '\''{"plain":123,"foo":"bar"}'\'''

echo '$ json <<< '\''{"plain":"foo","foo":"bar"}'\'''
json <<< '{"plain":"foo","foo":"bar"}' 2>&1 ||
echo 'command failed: json <<< '\''{"plain":"foo","foo":"bar"}'\'''

echo '$ json <<< '\''{"plain":"type","foo":"bar"}'\'''
json <<< '{"plain":"type","foo":"bar"}' 2>&1 ||
echo 'command failed: json <<< '\''{"plain":"type","foo":"bar"}'\'''

echo '$ json <<< '\''{"plain":"null","foo":"bar"}'\'''
json <<< '{"plain":"null","foo":"bar"}' 2>&1 ||
echo 'command failed: json <<< '\''{"plain":"null","foo":"bar"}'\'''

echo '$ json <<< '\''{"plain":"boolean","foo":"bar"}'\'''
json <<< '{"plain":"boolean","foo":"bar"}' 2>&1 ||
echo 'command failed: json <<< '\''{"plain":"boolean","foo":"bar"}'\'''

echo '$ json <<< '\''{"plain":"number","foo":"bar"}'\'''
json <<< '{"plain":"number","foo":"bar"}' 2>&1 ||
echo 'command failed: json <<< '\''{"plain":"number","foo":"bar"}'\'''

echo '$ json <<< '\''{"plain":"string","foo":"bar"}'\'''
json <<< '{"plain":"string","foo":"bar"}' 2>&1 ||
echo 'command failed: json <<< '\''{"plain":"string","foo":"bar"}'\'''

echo '$ json <<< '\''{"plain":"object","foo":"bar"}'\'''
json <<< '{"plain":"object","foo":"bar"}' 2>&1 ||
echo 'command failed: json <<< '\''{"plain":"object","foo":"bar"}'\'''

echo '$ json <<< '\''{"plain":"array","foo":"bar"}'\'''
json <<< '{"plain":"array","foo":"bar"}' 2>&1 ||
echo 'command failed: json <<< '\''{"plain":"array","foo":"bar"}'\'''

echo '$ json <<< '\''{"plain":"list","foo":"bar"}'\'''
json <<< '{"plain":"list","foo":"bar"}' 2>&1 ||
echo 'command failed: json <<< '\''{"plain":"list","foo":"bar"}'\'''

echo '$ json <<< '\''{"plain":"dict","foo":"bar"}'\'''
json <<< '{"plain":"dict","foo":"bar"}' 2>&1 ||
echo 'command failed: json <<< '\''{"plain":"dict","foo":"bar"}'\'''

echo '$ json <<< '\''{"plain":{},"foo":"bar"}'\'''
json <<< '{"plain":{},"foo":"bar"}' 2>&1 ||
echo 'command failed: json <<< '\''{"plain":{},"foo":"bar"}'\'''

echo '$ json <<< '\''{"plain":{"foo":"bar"},"foo":"bar"}'\'''
json <<< '{"plain":{"foo":"bar"},"foo":"bar"}' 2>&1 ||
echo 'command failed: json <<< '\''{"plain":{"foo":"bar"},"foo":"bar"}'\'''

echo '$ json <<< '\''{"plain":[],"foo":"bar"}'\'''
json <<< '{"plain":[],"foo":"bar"}' 2>&1 ||
echo 'command failed: json <<< '\''{"plain":[],"foo":"bar"}'\'''

echo '$ json <<< '\''{"plain":["foo","bar"],"foo":"bar"}'\'''
json <<< '{"plain":["foo","bar"],"foo":"bar"}' 2>&1 ||
echo 'command failed: json <<< '\''{"plain":["foo","bar"],"foo":"bar"}'\'''
)

