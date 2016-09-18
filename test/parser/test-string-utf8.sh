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
# $ json-gentest -C parser:string-utf8
#

[[ "$1" =~ ^-u[0-9]+$ ]] &&
u="${1:2}" ||
u=""

diff -u$u -L string-utf8.old <(echo \
'$ json0() { LD_LIBRARY_PATH=../lib ../src/json --pretty --verbose "$@"; }
$ json() { json0 --literal-value --validate-utf8 "$@"; }
$ echo -e '\''"\xc0"'\''|json
json: error: <stdin>:1:2: lex error: invalid utf-8 encoding
json: error: <stdin>:1:2: "\xc0"
json: error: <stdin>:1:2:  ^
command failed: echo -e '\''"\xc0"'\''|json
$ echo -e '\''"\xc1"'\''|json
json: error: <stdin>:1:2: lex error: invalid utf-8 encoding
json: error: <stdin>:1:2: "\xc1"
json: error: <stdin>:1:2:  ^
command failed: echo -e '\''"\xc1"'\''|json
$ echo -e '\''"\xc2"'\''|json
json: error: <stdin>:1:3: lex error: invalid utf-8 encoding
json: error: <stdin>:1:3: "\xc2"
json: error: <stdin>:1:3:      ^
command failed: echo -e '\''"\xc2"'\''|json
$ echo -e '\''"\xdf"'\''|json
json: error: <stdin>:1:3: lex error: invalid utf-8 encoding
json: error: <stdin>:1:3: "\xdf"
json: error: <stdin>:1:3:      ^
command failed: echo -e '\''"\xdf"'\''|json
$ echo -e '\''"\xc2\x7f"'\''|json
json: error: <stdin>:1:3: lex error: invalid string literal
json: error: <stdin>:1:3: "\xc2\x7f"
json: error: <stdin>:1:3:      ^
command failed: echo -e '\''"\xc2\x7f"'\''|json
$ echo -e '\''"\xdf\x7f"'\''|json
json: error: <stdin>:1:3: lex error: invalid string literal
json: error: <stdin>:1:3: "\xdf\x7f"
json: error: <stdin>:1:3:      ^
command failed: echo -e '\''"\xdf\x7f"'\''|json
$ echo -e '\''"\xc2\xc0"'\''|json
json: error: <stdin>:1:3: lex error: invalid utf-8 encoding
json: error: <stdin>:1:3: "\xc2\xc0"
json: error: <stdin>:1:3:      ^
command failed: echo -e '\''"\xc2\xc0"'\''|json
$ echo -e '\''"\xdf\xc0"'\''|json
json: error: <stdin>:1:3: lex error: invalid utf-8 encoding
json: error: <stdin>:1:3: "\xdf\xc0"
json: error: <stdin>:1:3:      ^
command failed: echo -e '\''"\xdf\xc0"'\''|json
$ echo -e '\''"\xe0"'\''|json
json: error: <stdin>:1:3: lex error: invalid utf-8 encoding
json: error: <stdin>:1:3: "\xe0"
json: error: <stdin>:1:3:      ^
command failed: echo -e '\''"\xe0"'\''|json
$ echo -e '\''"\xe0\x9f"'\''|json
json: error: <stdin>:1:3: lex error: invalid utf-8 encoding
json: error: <stdin>:1:3: "\xe0\x9f"
json: error: <stdin>:1:3:      ^
command failed: echo -e '\''"\xe0\x9f"'\''|json
$ echo -e '\''"\xe0\xc0"'\''|json
json: error: <stdin>:1:3: lex error: invalid utf-8 encoding
json: error: <stdin>:1:3: "\xe0\xc0"
json: error: <stdin>:1:3:      ^
command failed: echo -e '\''"\xe0\xc0"'\''|json
$ echo -e '\''"\xe0\xa0"'\''|json
json: error: <stdin>:1:4: lex error: invalid utf-8 encoding
json: error: <stdin>:1:4: "\xe0\xa0"
json: error: <stdin>:1:4:          ^
command failed: echo -e '\''"\xe0\xa0"'\''|json
$ echo -e '\''"\xe0\xbf"'\''|json
json: error: <stdin>:1:4: lex error: invalid utf-8 encoding
json: error: <stdin>:1:4: "\xe0\xbf"
json: error: <stdin>:1:4:          ^
command failed: echo -e '\''"\xe0\xbf"'\''|json
$ echo -e '\''"\xe0\xa0\x7f"'\''|json
json: error: <stdin>:1:4: lex error: invalid string literal
json: error: <stdin>:1:4: "\xe0\xa0\x7f"
json: error: <stdin>:1:4:          ^
command failed: echo -e '\''"\xe0\xa0\x7f"'\''|json
$ echo -e '\''"\xe0\xbf\x7f"'\''|json
json: error: <stdin>:1:4: lex error: invalid string literal
json: error: <stdin>:1:4: "\xe0\xbf\x7f"
json: error: <stdin>:1:4:          ^
command failed: echo -e '\''"\xe0\xbf\x7f"'\''|json
$ echo -e '\''"\xe0\xa0\xc0"'\''|json
json: error: <stdin>:1:4: lex error: invalid utf-8 encoding
json: error: <stdin>:1:4: "\xe0\xa0\xc0"
json: error: <stdin>:1:4:          ^
command failed: echo -e '\''"\xe0\xa0\xc0"'\''|json
$ echo -e '\''"\xe0\xbf\xc0"'\''|json
json: error: <stdin>:1:4: lex error: invalid utf-8 encoding
json: error: <stdin>:1:4: "\xe0\xbf\xc0"
json: error: <stdin>:1:4:          ^
command failed: echo -e '\''"\xe0\xbf\xc0"'\''|json
$ echo -e '\''"\xe1"'\''|json
json: error: <stdin>:1:3: lex error: invalid utf-8 encoding
json: error: <stdin>:1:3: "\xe1"
json: error: <stdin>:1:3:      ^
command failed: echo -e '\''"\xe1"'\''|json
$ echo -e '\''"\xe1\x7f"'\''|json
json: error: <stdin>:1:3: lex error: invalid string literal
json: error: <stdin>:1:3: "\xe1\x7f"
json: error: <stdin>:1:3:      ^
command failed: echo -e '\''"\xe1\x7f"'\''|json
$ echo -e '\''"\xe1\xc0"'\''|json
json: error: <stdin>:1:3: lex error: invalid utf-8 encoding
json: error: <stdin>:1:3: "\xe1\xc0"
json: error: <stdin>:1:3:      ^
command failed: echo -e '\''"\xe1\xc0"'\''|json
$ echo -e '\''"\xe1\x80"'\''|json
json: error: <stdin>:1:4: lex error: invalid utf-8 encoding
json: error: <stdin>:1:4: "\xe1\x80"
json: error: <stdin>:1:4:          ^
command failed: echo -e '\''"\xe1\x80"'\''|json
$ echo -e '\''"\xe1\xbf"'\''|json
json: error: <stdin>:1:4: lex error: invalid utf-8 encoding
json: error: <stdin>:1:4: "\xe1\xbf"
json: error: <stdin>:1:4:          ^
command failed: echo -e '\''"\xe1\xbf"'\''|json
$ echo -e '\''"\xe1\x80\x7f"'\''|json
json: error: <stdin>:1:4: lex error: invalid string literal
json: error: <stdin>:1:4: "\xe1\x80\x7f"
json: error: <stdin>:1:4:          ^
command failed: echo -e '\''"\xe1\x80\x7f"'\''|json
$ echo -e '\''"\xe1\xbf\x7f"'\''|json
json: error: <stdin>:1:4: lex error: invalid string literal
json: error: <stdin>:1:4: "\xe1\xbf\x7f"
json: error: <stdin>:1:4:          ^
command failed: echo -e '\''"\xe1\xbf\x7f"'\''|json
$ echo -e '\''"\xe1\x80\xc0"'\''|json
json: error: <stdin>:1:4: lex error: invalid utf-8 encoding
json: error: <stdin>:1:4: "\xe1\x80\xc0"
json: error: <stdin>:1:4:          ^
command failed: echo -e '\''"\xe1\x80\xc0"'\''|json
$ echo -e '\''"\xe1\xbf\xc0"'\''|json
json: error: <stdin>:1:4: lex error: invalid utf-8 encoding
json: error: <stdin>:1:4: "\xe1\xbf\xc0"
json: error: <stdin>:1:4:          ^
command failed: echo -e '\''"\xe1\xbf\xc0"'\''|json
$ echo -e '\''"\xec"'\''|json
json: error: <stdin>:1:3: lex error: invalid utf-8 encoding
json: error: <stdin>:1:3: "\xec"
json: error: <stdin>:1:3:      ^
command failed: echo -e '\''"\xec"'\''|json
$ echo -e '\''"\xec\x7f"'\''|json
json: error: <stdin>:1:3: lex error: invalid string literal
json: error: <stdin>:1:3: "\xec\x7f"
json: error: <stdin>:1:3:      ^
command failed: echo -e '\''"\xec\x7f"'\''|json
$ echo -e '\''"\xec\xc0"'\''|json
json: error: <stdin>:1:3: lex error: invalid utf-8 encoding
json: error: <stdin>:1:3: "\xec\xc0"
json: error: <stdin>:1:3:      ^
command failed: echo -e '\''"\xec\xc0"'\''|json
$ echo -e '\''"\xec\x80"'\''|json
json: error: <stdin>:1:4: lex error: invalid utf-8 encoding
json: error: <stdin>:1:4: "\xec\x80"
json: error: <stdin>:1:4:          ^
command failed: echo -e '\''"\xec\x80"'\''|json
$ echo -e '\''"\xec\xbf"'\''|json
json: error: <stdin>:1:4: lex error: invalid utf-8 encoding
json: error: <stdin>:1:4: "\xec\xbf"
json: error: <stdin>:1:4:          ^
command failed: echo -e '\''"\xec\xbf"'\''|json
$ echo -e '\''"\xec\x80\x7f"'\''|json
json: error: <stdin>:1:4: lex error: invalid string literal
json: error: <stdin>:1:4: "\xec\x80\x7f"
json: error: <stdin>:1:4:          ^
command failed: echo -e '\''"\xec\x80\x7f"'\''|json
$ echo -e '\''"\xec\xbf\x7f"'\''|json
json: error: <stdin>:1:4: lex error: invalid string literal
json: error: <stdin>:1:4: "\xec\xbf\x7f"
json: error: <stdin>:1:4:          ^
command failed: echo -e '\''"\xec\xbf\x7f"'\''|json
$ echo -e '\''"\xec\x80\xc0"'\''|json
json: error: <stdin>:1:4: lex error: invalid utf-8 encoding
json: error: <stdin>:1:4: "\xec\x80\xc0"
json: error: <stdin>:1:4:          ^
command failed: echo -e '\''"\xec\x80\xc0"'\''|json
$ echo -e '\''"\xec\xbf\xc0"'\''|json
json: error: <stdin>:1:4: lex error: invalid utf-8 encoding
json: error: <stdin>:1:4: "\xec\xbf\xc0"
json: error: <stdin>:1:4:          ^
command failed: echo -e '\''"\xec\xbf\xc0"'\''|json
$ echo -e '\''"\xed"'\''|json
json: error: <stdin>:1:3: lex error: invalid utf-8 encoding
json: error: <stdin>:1:3: "\xed"
json: error: <stdin>:1:3:      ^
command failed: echo -e '\''"\xed"'\''|json
$ echo -e '\''"\xed\x7f"'\''|json
json: error: <stdin>:1:3: lex error: invalid string literal
json: error: <stdin>:1:3: "\xed\x7f"
json: error: <stdin>:1:3:      ^
command failed: echo -e '\''"\xed\x7f"'\''|json
$ echo -e '\''"\xed\xa0"'\''|json
json: error: <stdin>:1:4: lex error: invalid utf-8 encoding
json: error: <stdin>:1:4: "\xed\xa0"
json: error: <stdin>:1:4:          ^
command failed: echo -e '\''"\xed\xa0"'\''|json
$ echo -e '\''"\xed\x80"'\''|json
json: error: <stdin>:1:4: lex error: invalid utf-8 encoding
json: error: <stdin>:1:4: "\xed\x80"
json: error: <stdin>:1:4:          ^
command failed: echo -e '\''"\xed\x80"'\''|json
$ echo -e '\''"\xed\x9f"'\''|json
json: error: <stdin>:1:4: lex error: invalid utf-8 encoding
json: error: <stdin>:1:4: "\xed\x9f"
json: error: <stdin>:1:4:          ^
command failed: echo -e '\''"\xed\x9f"'\''|json
$ echo -e '\''"\xed\x80\x7f"'\''|json
json: error: <stdin>:1:4: lex error: invalid string literal
json: error: <stdin>:1:4: "\xed\x80\x7f"
json: error: <stdin>:1:4:          ^
command failed: echo -e '\''"\xed\x80\x7f"'\''|json
$ echo -e '\''"\xed\x9f\x7f"'\''|json
json: error: <stdin>:1:4: lex error: invalid string literal
json: error: <stdin>:1:4: "\xed\x9f\x7f"
json: error: <stdin>:1:4:          ^
command failed: echo -e '\''"\xed\x9f\x7f"'\''|json
$ echo -e '\''"\xed\x80\xc0"'\''|json
json: error: <stdin>:1:4: lex error: invalid utf-8 encoding
json: error: <stdin>:1:4: "\xed\x80\xc0"
json: error: <stdin>:1:4:          ^
command failed: echo -e '\''"\xed\x80\xc0"'\''|json
$ echo -e '\''"\xed\x9f\xc0"'\''|json
json: error: <stdin>:1:4: lex error: invalid utf-8 encoding
json: error: <stdin>:1:4: "\xed\x9f\xc0"
json: error: <stdin>:1:4:          ^
command failed: echo -e '\''"\xed\x9f\xc0"'\''|json
$ echo -e '\''"\xee"'\''|json
json: error: <stdin>:1:3: lex error: invalid utf-8 encoding
json: error: <stdin>:1:3: "\xee"
json: error: <stdin>:1:3:      ^
command failed: echo -e '\''"\xee"'\''|json
$ echo -e '\''"\xee\x7f"'\''|json
json: error: <stdin>:1:3: lex error: invalid string literal
json: error: <stdin>:1:3: "\xee\x7f"
json: error: <stdin>:1:3:      ^
command failed: echo -e '\''"\xee\x7f"'\''|json
$ echo -e '\''"\xee\xc0"'\''|json
json: error: <stdin>:1:3: lex error: invalid utf-8 encoding
json: error: <stdin>:1:3: "\xee\xc0"
json: error: <stdin>:1:3:      ^
command failed: echo -e '\''"\xee\xc0"'\''|json
$ echo -e '\''"\xee\x80"'\''|json
json: error: <stdin>:1:4: lex error: invalid utf-8 encoding
json: error: <stdin>:1:4: "\xee\x80"
json: error: <stdin>:1:4:          ^
command failed: echo -e '\''"\xee\x80"'\''|json
$ echo -e '\''"\xee\xbf"'\''|json
json: error: <stdin>:1:4: lex error: invalid utf-8 encoding
json: error: <stdin>:1:4: "\xee\xbf"
json: error: <stdin>:1:4:          ^
command failed: echo -e '\''"\xee\xbf"'\''|json
$ echo -e '\''"\xee\x80\x7f"'\''|json
json: error: <stdin>:1:4: lex error: invalid string literal
json: error: <stdin>:1:4: "\xee\x80\x7f"
json: error: <stdin>:1:4:          ^
command failed: echo -e '\''"\xee\x80\x7f"'\''|json
$ echo -e '\''"\xee\xbf\x7f"'\''|json
json: error: <stdin>:1:4: lex error: invalid string literal
json: error: <stdin>:1:4: "\xee\xbf\x7f"
json: error: <stdin>:1:4:          ^
command failed: echo -e '\''"\xee\xbf\x7f"'\''|json
$ echo -e '\''"\xee\x80\xc0"'\''|json
json: error: <stdin>:1:4: lex error: invalid utf-8 encoding
json: error: <stdin>:1:4: "\xee\x80\xc0"
json: error: <stdin>:1:4:          ^
command failed: echo -e '\''"\xee\x80\xc0"'\''|json
$ echo -e '\''"\xee\xbf\xc0"'\''|json
json: error: <stdin>:1:4: lex error: invalid utf-8 encoding
json: error: <stdin>:1:4: "\xee\xbf\xc0"
json: error: <stdin>:1:4:          ^
command failed: echo -e '\''"\xee\xbf\xc0"'\''|json
$ echo -e '\''"\xef"'\''|json
json: error: <stdin>:1:3: lex error: invalid utf-8 encoding
json: error: <stdin>:1:3: "\xef"
json: error: <stdin>:1:3:      ^
command failed: echo -e '\''"\xef"'\''|json
$ echo -e '\''"\xef\x7f"'\''|json
json: error: <stdin>:1:3: lex error: invalid string literal
json: error: <stdin>:1:3: "\xef\x7f"
json: error: <stdin>:1:3:      ^
command failed: echo -e '\''"\xef\x7f"'\''|json
$ echo -e '\''"\xef\xc0"'\''|json
json: error: <stdin>:1:3: lex error: invalid utf-8 encoding
json: error: <stdin>:1:3: "\xef\xc0"
json: error: <stdin>:1:3:      ^
command failed: echo -e '\''"\xef\xc0"'\''|json
$ echo -e '\''"\xef\x80"'\''|json
json: error: <stdin>:1:4: lex error: invalid utf-8 encoding
json: error: <stdin>:1:4: "\xef\x80"
json: error: <stdin>:1:4:          ^
command failed: echo -e '\''"\xef\x80"'\''|json
$ echo -e '\''"\xef\xbf"'\''|json
json: error: <stdin>:1:4: lex error: invalid utf-8 encoding
json: error: <stdin>:1:4: "\xef\xbf"
json: error: <stdin>:1:4:          ^
command failed: echo -e '\''"\xef\xbf"'\''|json
$ echo -e '\''"\xef\x80\x7f"'\''|json
json: error: <stdin>:1:4: lex error: invalid string literal
json: error: <stdin>:1:4: "\xef\x80\x7f"
json: error: <stdin>:1:4:          ^
command failed: echo -e '\''"\xef\x80\x7f"'\''|json
$ echo -e '\''"\xef\xbf\x7f"'\''|json
json: error: <stdin>:1:4: lex error: invalid string literal
json: error: <stdin>:1:4: "\xef\xbf\x7f"
json: error: <stdin>:1:4:          ^
command failed: echo -e '\''"\xef\xbf\x7f"'\''|json
$ echo -e '\''"\xef\x80\xc0"'\''|json
json: error: <stdin>:1:4: lex error: invalid utf-8 encoding
json: error: <stdin>:1:4: "\xef\x80\xc0"
json: error: <stdin>:1:4:          ^
command failed: echo -e '\''"\xef\x80\xc0"'\''|json
$ echo -e '\''"\xef\xbf\xc0"'\''|json
json: error: <stdin>:1:4: lex error: invalid utf-8 encoding
json: error: <stdin>:1:4: "\xef\xbf\xc0"
json: error: <stdin>:1:4:          ^
command failed: echo -e '\''"\xef\xbf\xc0"'\''|json
$ echo -e '\''"\xf0"'\''|json
json: error: <stdin>:1:3: lex error: invalid utf-8 encoding
json: error: <stdin>:1:3: "\xf0"
json: error: <stdin>:1:3:      ^
command failed: echo -e '\''"\xf0"'\''|json
$ echo -e '\''"\xf0\x8f"'\''|json
json: error: <stdin>:1:3: lex error: invalid utf-8 encoding
json: error: <stdin>:1:3: "\xf0\x8f"
json: error: <stdin>:1:3:      ^
command failed: echo -e '\''"\xf0\x8f"'\''|json
$ echo -e '\''"\xf0\xc0"'\''|json
json: error: <stdin>:1:3: lex error: invalid utf-8 encoding
json: error: <stdin>:1:3: "\xf0\xc0"
json: error: <stdin>:1:3:      ^
command failed: echo -e '\''"\xf0\xc0"'\''|json
$ echo -e '\''"\xf0\x90"'\''|json
json: error: <stdin>:1:4: lex error: invalid utf-8 encoding
json: error: <stdin>:1:4: "\xf0\x90"
json: error: <stdin>:1:4:          ^
command failed: echo -e '\''"\xf0\x90"'\''|json
$ echo -e '\''"\xf0\xbf"'\''|json
json: error: <stdin>:1:4: lex error: invalid utf-8 encoding
json: error: <stdin>:1:4: "\xf0\xbf"
json: error: <stdin>:1:4:          ^
command failed: echo -e '\''"\xf0\xbf"'\''|json
$ echo -e '\''"\xf0\x90\x7f"'\''|json
json: error: <stdin>:1:4: lex error: invalid string literal
json: error: <stdin>:1:4: "\xf0\x90\x7f"
json: error: <stdin>:1:4:          ^
command failed: echo -e '\''"\xf0\x90\x7f"'\''|json
$ echo -e '\''"\xf0\xbf\xc0"'\''|json
json: error: <stdin>:1:4: lex error: invalid utf-8 encoding
json: error: <stdin>:1:4: "\xf0\xbf\xc0"
json: error: <stdin>:1:4:          ^
command failed: echo -e '\''"\xf0\xbf\xc0"'\''|json
$ echo -e '\''"\xf0\x90\x80"'\''|json
json: error: <stdin>:1:5: lex error: invalid utf-8 encoding
json: error: <stdin>:1:5: "\xf0\x90\x80"
json: error: <stdin>:1:5:              ^
command failed: echo -e '\''"\xf0\x90\x80"'\''|json
$ echo -e '\''"\xf0\xbf\xbf"'\''|json
json: error: <stdin>:1:5: lex error: invalid utf-8 encoding
json: error: <stdin>:1:5: "\xf0\xbf\xbf"
json: error: <stdin>:1:5:              ^
command failed: echo -e '\''"\xf0\xbf\xbf"'\''|json
$ echo -e '\''"\xf0\x90\x80\x7f"'\''|json
json: error: <stdin>:1:5: lex error: invalid string literal
json: error: <stdin>:1:5: "\xf0\x90\x80\x7f"
json: error: <stdin>:1:5:              ^
command failed: echo -e '\''"\xf0\x90\x80\x7f"'\''|json
$ echo -e '\''"\xf0\xbf\xbf\xc0"'\''|json
json: error: <stdin>:1:5: lex error: invalid utf-8 encoding
json: error: <stdin>:1:5: "\xf0\xbf\xbf\xc0"
json: error: <stdin>:1:5:              ^
command failed: echo -e '\''"\xf0\xbf\xbf\xc0"'\''|json
$ echo -e '\''"\xf1"'\''|json
json: error: <stdin>:1:3: lex error: invalid utf-8 encoding
json: error: <stdin>:1:3: "\xf1"
json: error: <stdin>:1:3:      ^
command failed: echo -e '\''"\xf1"'\''|json
$ echo -e '\''"\xf1\x7f"'\''|json
json: error: <stdin>:1:3: lex error: invalid string literal
json: error: <stdin>:1:3: "\xf1\x7f"
json: error: <stdin>:1:3:      ^
command failed: echo -e '\''"\xf1\x7f"'\''|json
$ echo -e '\''"\xf1\xc0"'\''|json
json: error: <stdin>:1:3: lex error: invalid utf-8 encoding
json: error: <stdin>:1:3: "\xf1\xc0"
json: error: <stdin>:1:3:      ^
command failed: echo -e '\''"\xf1\xc0"'\''|json
$ echo -e '\''"\xf1\x80"'\''|json
json: error: <stdin>:1:4: lex error: invalid utf-8 encoding
json: error: <stdin>:1:4: "\xf1\x80"
json: error: <stdin>:1:4:          ^
command failed: echo -e '\''"\xf1\x80"'\''|json
$ echo -e '\''"\xf1\xbf"'\''|json
json: error: <stdin>:1:4: lex error: invalid utf-8 encoding
json: error: <stdin>:1:4: "\xf1\xbf"
json: error: <stdin>:1:4:          ^
command failed: echo -e '\''"\xf1\xbf"'\''|json
$ echo -e '\''"\xf1\x80\x7f"'\''|json
json: error: <stdin>:1:4: lex error: invalid string literal
json: error: <stdin>:1:4: "\xf1\x80\x7f"
json: error: <stdin>:1:4:          ^
command failed: echo -e '\''"\xf1\x80\x7f"'\''|json
$ echo -e '\''"\xf1\xbf\xc0"'\''|json
json: error: <stdin>:1:4: lex error: invalid utf-8 encoding
json: error: <stdin>:1:4: "\xf1\xbf\xc0"
json: error: <stdin>:1:4:          ^
command failed: echo -e '\''"\xf1\xbf\xc0"'\''|json
$ echo -e '\''"\xf1\x80\x80"'\''|json
json: error: <stdin>:1:5: lex error: invalid utf-8 encoding
json: error: <stdin>:1:5: "\xf1\x80\x80"
json: error: <stdin>:1:5:              ^
command failed: echo -e '\''"\xf1\x80\x80"'\''|json
$ echo -e '\''"\xf1\xbf\xbf"'\''|json
json: error: <stdin>:1:5: lex error: invalid utf-8 encoding
json: error: <stdin>:1:5: "\xf1\xbf\xbf"
json: error: <stdin>:1:5:              ^
command failed: echo -e '\''"\xf1\xbf\xbf"'\''|json
$ echo -e '\''"\xf1\x80\x80\x7f"'\''|json
json: error: <stdin>:1:5: lex error: invalid string literal
json: error: <stdin>:1:5: "\xf1\x80\x80\x7f"
json: error: <stdin>:1:5:              ^
command failed: echo -e '\''"\xf1\x80\x80\x7f"'\''|json
$ echo -e '\''"\xf1\xbf\xbf\xc0"'\''|json
json: error: <stdin>:1:5: lex error: invalid utf-8 encoding
json: error: <stdin>:1:5: "\xf1\xbf\xbf\xc0"
json: error: <stdin>:1:5:              ^
command failed: echo -e '\''"\xf1\xbf\xbf\xc0"'\''|json
$ echo -e '\''"\xf3"'\''|json
json: error: <stdin>:1:3: lex error: invalid utf-8 encoding
json: error: <stdin>:1:3: "\xf3"
json: error: <stdin>:1:3:      ^
command failed: echo -e '\''"\xf3"'\''|json
$ echo -e '\''"\xf3\x7f"'\''|json
json: error: <stdin>:1:3: lex error: invalid string literal
json: error: <stdin>:1:3: "\xf3\x7f"
json: error: <stdin>:1:3:      ^
command failed: echo -e '\''"\xf3\x7f"'\''|json
$ echo -e '\''"\xf3\xc0"'\''|json
json: error: <stdin>:1:3: lex error: invalid utf-8 encoding
json: error: <stdin>:1:3: "\xf3\xc0"
json: error: <stdin>:1:3:      ^
command failed: echo -e '\''"\xf3\xc0"'\''|json
$ echo -e '\''"\xf3\x80"'\''|json
json: error: <stdin>:1:4: lex error: invalid utf-8 encoding
json: error: <stdin>:1:4: "\xf3\x80"
json: error: <stdin>:1:4:          ^
command failed: echo -e '\''"\xf3\x80"'\''|json
$ echo -e '\''"\xf3\xbf"'\''|json
json: error: <stdin>:1:4: lex error: invalid utf-8 encoding
json: error: <stdin>:1:4: "\xf3\xbf"
json: error: <stdin>:1:4:          ^
command failed: echo -e '\''"\xf3\xbf"'\''|json
$ echo -e '\''"\xf3\x80\x7f"'\''|json
json: error: <stdin>:1:4: lex error: invalid string literal
json: error: <stdin>:1:4: "\xf3\x80\x7f"
json: error: <stdin>:1:4:          ^
command failed: echo -e '\''"\xf3\x80\x7f"'\''|json
$ echo -e '\''"\xf3\xbf\xc0"'\''|json
json: error: <stdin>:1:4: lex error: invalid utf-8 encoding
json: error: <stdin>:1:4: "\xf3\xbf\xc0"
json: error: <stdin>:1:4:          ^
command failed: echo -e '\''"\xf3\xbf\xc0"'\''|json
$ echo -e '\''"\xf3\x80\x80"'\''|json
json: error: <stdin>:1:5: lex error: invalid utf-8 encoding
json: error: <stdin>:1:5: "\xf3\x80\x80"
json: error: <stdin>:1:5:              ^
command failed: echo -e '\''"\xf3\x80\x80"'\''|json
$ echo -e '\''"\xf3\xbf\xbf"'\''|json
json: error: <stdin>:1:5: lex error: invalid utf-8 encoding
json: error: <stdin>:1:5: "\xf3\xbf\xbf"
json: error: <stdin>:1:5:              ^
command failed: echo -e '\''"\xf3\xbf\xbf"'\''|json
$ echo -e '\''"\xf3\x80\x80\x7f"'\''|json
json: error: <stdin>:1:5: lex error: invalid string literal
json: error: <stdin>:1:5: "\xf3\x80\x80\x7f"
json: error: <stdin>:1:5:              ^
command failed: echo -e '\''"\xf3\x80\x80\x7f"'\''|json
$ echo -e '\''"\xf3\xbf\xbf\xc0"'\''|json
json: error: <stdin>:1:5: lex error: invalid utf-8 encoding
json: error: <stdin>:1:5: "\xf3\xbf\xbf\xc0"
json: error: <stdin>:1:5:              ^
command failed: echo -e '\''"\xf3\xbf\xbf\xc0"'\''|json
$ echo -e '\''"\xf4"'\''|json
json: error: <stdin>:1:3: lex error: invalid utf-8 encoding
json: error: <stdin>:1:3: "\xf4"
json: error: <stdin>:1:3:      ^
command failed: echo -e '\''"\xf4"'\''|json
$ echo -e '\''"\xf4\x7f"'\''|json
json: error: <stdin>:1:3: lex error: invalid string literal
json: error: <stdin>:1:3: "\xf4\x7f"
json: error: <stdin>:1:3:      ^
command failed: echo -e '\''"\xf4\x7f"'\''|json
$ echo -e '\''"\xf4\x90"'\''|json
json: error: <stdin>:1:3: lex error: invalid utf-8 encoding
json: error: <stdin>:1:3: "\xf4\x90"
json: error: <stdin>:1:3:      ^
command failed: echo -e '\''"\xf4\x90"'\''|json
$ echo -e '\''"\xf4\x80"'\''|json
json: error: <stdin>:1:4: lex error: invalid utf-8 encoding
json: error: <stdin>:1:4: "\xf4\x80"
json: error: <stdin>:1:4:          ^
command failed: echo -e '\''"\xf4\x80"'\''|json
$ echo -e '\''"\xf4\x8f"'\''|json
json: error: <stdin>:1:4: lex error: invalid utf-8 encoding
json: error: <stdin>:1:4: "\xf4\x8f"
json: error: <stdin>:1:4:          ^
command failed: echo -e '\''"\xf4\x8f"'\''|json
$ echo -e '\''"\xf4\x80\x7f"'\''|json
json: error: <stdin>:1:4: lex error: invalid string literal
json: error: <stdin>:1:4: "\xf4\x80\x7f"
json: error: <stdin>:1:4:          ^
command failed: echo -e '\''"\xf4\x80\x7f"'\''|json
$ echo -e '\''"\xf4\x8f\xc0"'\''|json
json: error: <stdin>:1:4: lex error: invalid utf-8 encoding
json: error: <stdin>:1:4: "\xf4\x8f\xc0"
json: error: <stdin>:1:4:          ^
command failed: echo -e '\''"\xf4\x8f\xc0"'\''|json
$ echo -e '\''"\xf4\x80\x80"'\''|json
json: error: <stdin>:1:5: lex error: invalid utf-8 encoding
json: error: <stdin>:1:5: "\xf4\x80\x80"
json: error: <stdin>:1:5:              ^
command failed: echo -e '\''"\xf4\x80\x80"'\''|json
$ echo -e '\''"\xf4\x8f\xbf"'\''|json
json: error: <stdin>:1:5: lex error: invalid utf-8 encoding
json: error: <stdin>:1:5: "\xf4\x8f\xbf"
json: error: <stdin>:1:5:              ^
command failed: echo -e '\''"\xf4\x8f\xbf"'\''|json
$ echo -e '\''"\xf4\x80\x80\x7f"'\''|json
json: error: <stdin>:1:5: lex error: invalid string literal
json: error: <stdin>:1:5: "\xf4\x80\x80\x7f"
json: error: <stdin>:1:5:              ^
command failed: echo -e '\''"\xf4\x80\x80\x7f"'\''|json
$ echo -e '\''"\xf4\x8f\xbf\xc0"'\''|json
json: error: <stdin>:1:5: lex error: invalid utf-8 encoding
json: error: <stdin>:1:5: "\xf4\x8f\xbf\xc0"
json: error: <stdin>:1:5:              ^
command failed: echo -e '\''"\xf4\x8f\xbf\xc0"'\''|json
$ echo -e '\''"\xf5"'\''|json
json: error: <stdin>:1:2: lex error: invalid utf-8 encoding
json: error: <stdin>:1:2: "\xf5"
json: error: <stdin>:1:2:  ^
command failed: echo -e '\''"\xf5"'\''|json
$ echo -e '\''"\x41\xe2\x89\xa2\xce\x91\x2e"'\''|json
"A≢Α."'
) -L string-utf8.new <(
echo '$ json0() { LD_LIBRARY_PATH=../lib ../src/json --pretty --verbose "$@"; }'
json0() { LD_LIBRARY_PATH=../lib ../src/json --pretty --verbose "$@"; } 2>&1 ||
echo 'command failed: json0() { LD_LIBRARY_PATH=../lib ../src/json --pretty --verbose "$@"; }'

echo '$ json() { json0 --literal-value --validate-utf8 "$@"; }'
json() { json0 --literal-value --validate-utf8 "$@"; } 2>&1 ||
echo 'command failed: json() { json0 --literal-value --validate-utf8 "$@"; }'

echo '$ echo -e '\''"\xc0"'\''|json'
echo -e '"\xc0"'|json 2>&1 ||
echo 'command failed: echo -e '\''"\xc0"'\''|json'

echo '$ echo -e '\''"\xc1"'\''|json'
echo -e '"\xc1"'|json 2>&1 ||
echo 'command failed: echo -e '\''"\xc1"'\''|json'

echo '$ echo -e '\''"\xc2"'\''|json'
echo -e '"\xc2"'|json 2>&1 ||
echo 'command failed: echo -e '\''"\xc2"'\''|json'

echo '$ echo -e '\''"\xdf"'\''|json'
echo -e '"\xdf"'|json 2>&1 ||
echo 'command failed: echo -e '\''"\xdf"'\''|json'

echo '$ echo -e '\''"\xc2\x7f"'\''|json'
echo -e '"\xc2\x7f"'|json 2>&1 ||
echo 'command failed: echo -e '\''"\xc2\x7f"'\''|json'

echo '$ echo -e '\''"\xdf\x7f"'\''|json'
echo -e '"\xdf\x7f"'|json 2>&1 ||
echo 'command failed: echo -e '\''"\xdf\x7f"'\''|json'

echo '$ echo -e '\''"\xc2\xc0"'\''|json'
echo -e '"\xc2\xc0"'|json 2>&1 ||
echo 'command failed: echo -e '\''"\xc2\xc0"'\''|json'

echo '$ echo -e '\''"\xdf\xc0"'\''|json'
echo -e '"\xdf\xc0"'|json 2>&1 ||
echo 'command failed: echo -e '\''"\xdf\xc0"'\''|json'

echo '$ echo -e '\''"\xe0"'\''|json'
echo -e '"\xe0"'|json 2>&1 ||
echo 'command failed: echo -e '\''"\xe0"'\''|json'

echo '$ echo -e '\''"\xe0\x9f"'\''|json'
echo -e '"\xe0\x9f"'|json 2>&1 ||
echo 'command failed: echo -e '\''"\xe0\x9f"'\''|json'

echo '$ echo -e '\''"\xe0\xc0"'\''|json'
echo -e '"\xe0\xc0"'|json 2>&1 ||
echo 'command failed: echo -e '\''"\xe0\xc0"'\''|json'

echo '$ echo -e '\''"\xe0\xa0"'\''|json'
echo -e '"\xe0\xa0"'|json 2>&1 ||
echo 'command failed: echo -e '\''"\xe0\xa0"'\''|json'

echo '$ echo -e '\''"\xe0\xbf"'\''|json'
echo -e '"\xe0\xbf"'|json 2>&1 ||
echo 'command failed: echo -e '\''"\xe0\xbf"'\''|json'

echo '$ echo -e '\''"\xe0\xa0\x7f"'\''|json'
echo -e '"\xe0\xa0\x7f"'|json 2>&1 ||
echo 'command failed: echo -e '\''"\xe0\xa0\x7f"'\''|json'

echo '$ echo -e '\''"\xe0\xbf\x7f"'\''|json'
echo -e '"\xe0\xbf\x7f"'|json 2>&1 ||
echo 'command failed: echo -e '\''"\xe0\xbf\x7f"'\''|json'

echo '$ echo -e '\''"\xe0\xa0\xc0"'\''|json'
echo -e '"\xe0\xa0\xc0"'|json 2>&1 ||
echo 'command failed: echo -e '\''"\xe0\xa0\xc0"'\''|json'

echo '$ echo -e '\''"\xe0\xbf\xc0"'\''|json'
echo -e '"\xe0\xbf\xc0"'|json 2>&1 ||
echo 'command failed: echo -e '\''"\xe0\xbf\xc0"'\''|json'

echo '$ echo -e '\''"\xe1"'\''|json'
echo -e '"\xe1"'|json 2>&1 ||
echo 'command failed: echo -e '\''"\xe1"'\''|json'

echo '$ echo -e '\''"\xe1\x7f"'\''|json'
echo -e '"\xe1\x7f"'|json 2>&1 ||
echo 'command failed: echo -e '\''"\xe1\x7f"'\''|json'

echo '$ echo -e '\''"\xe1\xc0"'\''|json'
echo -e '"\xe1\xc0"'|json 2>&1 ||
echo 'command failed: echo -e '\''"\xe1\xc0"'\''|json'

echo '$ echo -e '\''"\xe1\x80"'\''|json'
echo -e '"\xe1\x80"'|json 2>&1 ||
echo 'command failed: echo -e '\''"\xe1\x80"'\''|json'

echo '$ echo -e '\''"\xe1\xbf"'\''|json'
echo -e '"\xe1\xbf"'|json 2>&1 ||
echo 'command failed: echo -e '\''"\xe1\xbf"'\''|json'

echo '$ echo -e '\''"\xe1\x80\x7f"'\''|json'
echo -e '"\xe1\x80\x7f"'|json 2>&1 ||
echo 'command failed: echo -e '\''"\xe1\x80\x7f"'\''|json'

echo '$ echo -e '\''"\xe1\xbf\x7f"'\''|json'
echo -e '"\xe1\xbf\x7f"'|json 2>&1 ||
echo 'command failed: echo -e '\''"\xe1\xbf\x7f"'\''|json'

echo '$ echo -e '\''"\xe1\x80\xc0"'\''|json'
echo -e '"\xe1\x80\xc0"'|json 2>&1 ||
echo 'command failed: echo -e '\''"\xe1\x80\xc0"'\''|json'

echo '$ echo -e '\''"\xe1\xbf\xc0"'\''|json'
echo -e '"\xe1\xbf\xc0"'|json 2>&1 ||
echo 'command failed: echo -e '\''"\xe1\xbf\xc0"'\''|json'

echo '$ echo -e '\''"\xec"'\''|json'
echo -e '"\xec"'|json 2>&1 ||
echo 'command failed: echo -e '\''"\xec"'\''|json'

echo '$ echo -e '\''"\xec\x7f"'\''|json'
echo -e '"\xec\x7f"'|json 2>&1 ||
echo 'command failed: echo -e '\''"\xec\x7f"'\''|json'

echo '$ echo -e '\''"\xec\xc0"'\''|json'
echo -e '"\xec\xc0"'|json 2>&1 ||
echo 'command failed: echo -e '\''"\xec\xc0"'\''|json'

echo '$ echo -e '\''"\xec\x80"'\''|json'
echo -e '"\xec\x80"'|json 2>&1 ||
echo 'command failed: echo -e '\''"\xec\x80"'\''|json'

echo '$ echo -e '\''"\xec\xbf"'\''|json'
echo -e '"\xec\xbf"'|json 2>&1 ||
echo 'command failed: echo -e '\''"\xec\xbf"'\''|json'

echo '$ echo -e '\''"\xec\x80\x7f"'\''|json'
echo -e '"\xec\x80\x7f"'|json 2>&1 ||
echo 'command failed: echo -e '\''"\xec\x80\x7f"'\''|json'

echo '$ echo -e '\''"\xec\xbf\x7f"'\''|json'
echo -e '"\xec\xbf\x7f"'|json 2>&1 ||
echo 'command failed: echo -e '\''"\xec\xbf\x7f"'\''|json'

echo '$ echo -e '\''"\xec\x80\xc0"'\''|json'
echo -e '"\xec\x80\xc0"'|json 2>&1 ||
echo 'command failed: echo -e '\''"\xec\x80\xc0"'\''|json'

echo '$ echo -e '\''"\xec\xbf\xc0"'\''|json'
echo -e '"\xec\xbf\xc0"'|json 2>&1 ||
echo 'command failed: echo -e '\''"\xec\xbf\xc0"'\''|json'

echo '$ echo -e '\''"\xed"'\''|json'
echo -e '"\xed"'|json 2>&1 ||
echo 'command failed: echo -e '\''"\xed"'\''|json'

echo '$ echo -e '\''"\xed\x7f"'\''|json'
echo -e '"\xed\x7f"'|json 2>&1 ||
echo 'command failed: echo -e '\''"\xed\x7f"'\''|json'

echo '$ echo -e '\''"\xed\xa0"'\''|json'
echo -e '"\xed\xa0"'|json 2>&1 ||
echo 'command failed: echo -e '\''"\xed\xa0"'\''|json'

echo '$ echo -e '\''"\xed\x80"'\''|json'
echo -e '"\xed\x80"'|json 2>&1 ||
echo 'command failed: echo -e '\''"\xed\x80"'\''|json'

echo '$ echo -e '\''"\xed\x9f"'\''|json'
echo -e '"\xed\x9f"'|json 2>&1 ||
echo 'command failed: echo -e '\''"\xed\x9f"'\''|json'

echo '$ echo -e '\''"\xed\x80\x7f"'\''|json'
echo -e '"\xed\x80\x7f"'|json 2>&1 ||
echo 'command failed: echo -e '\''"\xed\x80\x7f"'\''|json'

echo '$ echo -e '\''"\xed\x9f\x7f"'\''|json'
echo -e '"\xed\x9f\x7f"'|json 2>&1 ||
echo 'command failed: echo -e '\''"\xed\x9f\x7f"'\''|json'

echo '$ echo -e '\''"\xed\x80\xc0"'\''|json'
echo -e '"\xed\x80\xc0"'|json 2>&1 ||
echo 'command failed: echo -e '\''"\xed\x80\xc0"'\''|json'

echo '$ echo -e '\''"\xed\x9f\xc0"'\''|json'
echo -e '"\xed\x9f\xc0"'|json 2>&1 ||
echo 'command failed: echo -e '\''"\xed\x9f\xc0"'\''|json'

echo '$ echo -e '\''"\xee"'\''|json'
echo -e '"\xee"'|json 2>&1 ||
echo 'command failed: echo -e '\''"\xee"'\''|json'

echo '$ echo -e '\''"\xee\x7f"'\''|json'
echo -e '"\xee\x7f"'|json 2>&1 ||
echo 'command failed: echo -e '\''"\xee\x7f"'\''|json'

echo '$ echo -e '\''"\xee\xc0"'\''|json'
echo -e '"\xee\xc0"'|json 2>&1 ||
echo 'command failed: echo -e '\''"\xee\xc0"'\''|json'

echo '$ echo -e '\''"\xee\x80"'\''|json'
echo -e '"\xee\x80"'|json 2>&1 ||
echo 'command failed: echo -e '\''"\xee\x80"'\''|json'

echo '$ echo -e '\''"\xee\xbf"'\''|json'
echo -e '"\xee\xbf"'|json 2>&1 ||
echo 'command failed: echo -e '\''"\xee\xbf"'\''|json'

echo '$ echo -e '\''"\xee\x80\x7f"'\''|json'
echo -e '"\xee\x80\x7f"'|json 2>&1 ||
echo 'command failed: echo -e '\''"\xee\x80\x7f"'\''|json'

echo '$ echo -e '\''"\xee\xbf\x7f"'\''|json'
echo -e '"\xee\xbf\x7f"'|json 2>&1 ||
echo 'command failed: echo -e '\''"\xee\xbf\x7f"'\''|json'

echo '$ echo -e '\''"\xee\x80\xc0"'\''|json'
echo -e '"\xee\x80\xc0"'|json 2>&1 ||
echo 'command failed: echo -e '\''"\xee\x80\xc0"'\''|json'

echo '$ echo -e '\''"\xee\xbf\xc0"'\''|json'
echo -e '"\xee\xbf\xc0"'|json 2>&1 ||
echo 'command failed: echo -e '\''"\xee\xbf\xc0"'\''|json'

echo '$ echo -e '\''"\xef"'\''|json'
echo -e '"\xef"'|json 2>&1 ||
echo 'command failed: echo -e '\''"\xef"'\''|json'

echo '$ echo -e '\''"\xef\x7f"'\''|json'
echo -e '"\xef\x7f"'|json 2>&1 ||
echo 'command failed: echo -e '\''"\xef\x7f"'\''|json'

echo '$ echo -e '\''"\xef\xc0"'\''|json'
echo -e '"\xef\xc0"'|json 2>&1 ||
echo 'command failed: echo -e '\''"\xef\xc0"'\''|json'

echo '$ echo -e '\''"\xef\x80"'\''|json'
echo -e '"\xef\x80"'|json 2>&1 ||
echo 'command failed: echo -e '\''"\xef\x80"'\''|json'

echo '$ echo -e '\''"\xef\xbf"'\''|json'
echo -e '"\xef\xbf"'|json 2>&1 ||
echo 'command failed: echo -e '\''"\xef\xbf"'\''|json'

echo '$ echo -e '\''"\xef\x80\x7f"'\''|json'
echo -e '"\xef\x80\x7f"'|json 2>&1 ||
echo 'command failed: echo -e '\''"\xef\x80\x7f"'\''|json'

echo '$ echo -e '\''"\xef\xbf\x7f"'\''|json'
echo -e '"\xef\xbf\x7f"'|json 2>&1 ||
echo 'command failed: echo -e '\''"\xef\xbf\x7f"'\''|json'

echo '$ echo -e '\''"\xef\x80\xc0"'\''|json'
echo -e '"\xef\x80\xc0"'|json 2>&1 ||
echo 'command failed: echo -e '\''"\xef\x80\xc0"'\''|json'

echo '$ echo -e '\''"\xef\xbf\xc0"'\''|json'
echo -e '"\xef\xbf\xc0"'|json 2>&1 ||
echo 'command failed: echo -e '\''"\xef\xbf\xc0"'\''|json'

echo '$ echo -e '\''"\xf0"'\''|json'
echo -e '"\xf0"'|json 2>&1 ||
echo 'command failed: echo -e '\''"\xf0"'\''|json'

echo '$ echo -e '\''"\xf0\x8f"'\''|json'
echo -e '"\xf0\x8f"'|json 2>&1 ||
echo 'command failed: echo -e '\''"\xf0\x8f"'\''|json'

echo '$ echo -e '\''"\xf0\xc0"'\''|json'
echo -e '"\xf0\xc0"'|json 2>&1 ||
echo 'command failed: echo -e '\''"\xf0\xc0"'\''|json'

echo '$ echo -e '\''"\xf0\x90"'\''|json'
echo -e '"\xf0\x90"'|json 2>&1 ||
echo 'command failed: echo -e '\''"\xf0\x90"'\''|json'

echo '$ echo -e '\''"\xf0\xbf"'\''|json'
echo -e '"\xf0\xbf"'|json 2>&1 ||
echo 'command failed: echo -e '\''"\xf0\xbf"'\''|json'

echo '$ echo -e '\''"\xf0\x90\x7f"'\''|json'
echo -e '"\xf0\x90\x7f"'|json 2>&1 ||
echo 'command failed: echo -e '\''"\xf0\x90\x7f"'\''|json'

echo '$ echo -e '\''"\xf0\xbf\xc0"'\''|json'
echo -e '"\xf0\xbf\xc0"'|json 2>&1 ||
echo 'command failed: echo -e '\''"\xf0\xbf\xc0"'\''|json'

echo '$ echo -e '\''"\xf0\x90\x80"'\''|json'
echo -e '"\xf0\x90\x80"'|json 2>&1 ||
echo 'command failed: echo -e '\''"\xf0\x90\x80"'\''|json'

echo '$ echo -e '\''"\xf0\xbf\xbf"'\''|json'
echo -e '"\xf0\xbf\xbf"'|json 2>&1 ||
echo 'command failed: echo -e '\''"\xf0\xbf\xbf"'\''|json'

echo '$ echo -e '\''"\xf0\x90\x80\x7f"'\''|json'
echo -e '"\xf0\x90\x80\x7f"'|json 2>&1 ||
echo 'command failed: echo -e '\''"\xf0\x90\x80\x7f"'\''|json'

echo '$ echo -e '\''"\xf0\xbf\xbf\xc0"'\''|json'
echo -e '"\xf0\xbf\xbf\xc0"'|json 2>&1 ||
echo 'command failed: echo -e '\''"\xf0\xbf\xbf\xc0"'\''|json'

echo '$ echo -e '\''"\xf1"'\''|json'
echo -e '"\xf1"'|json 2>&1 ||
echo 'command failed: echo -e '\''"\xf1"'\''|json'

echo '$ echo -e '\''"\xf1\x7f"'\''|json'
echo -e '"\xf1\x7f"'|json 2>&1 ||
echo 'command failed: echo -e '\''"\xf1\x7f"'\''|json'

echo '$ echo -e '\''"\xf1\xc0"'\''|json'
echo -e '"\xf1\xc0"'|json 2>&1 ||
echo 'command failed: echo -e '\''"\xf1\xc0"'\''|json'

echo '$ echo -e '\''"\xf1\x80"'\''|json'
echo -e '"\xf1\x80"'|json 2>&1 ||
echo 'command failed: echo -e '\''"\xf1\x80"'\''|json'

echo '$ echo -e '\''"\xf1\xbf"'\''|json'
echo -e '"\xf1\xbf"'|json 2>&1 ||
echo 'command failed: echo -e '\''"\xf1\xbf"'\''|json'

echo '$ echo -e '\''"\xf1\x80\x7f"'\''|json'
echo -e '"\xf1\x80\x7f"'|json 2>&1 ||
echo 'command failed: echo -e '\''"\xf1\x80\x7f"'\''|json'

echo '$ echo -e '\''"\xf1\xbf\xc0"'\''|json'
echo -e '"\xf1\xbf\xc0"'|json 2>&1 ||
echo 'command failed: echo -e '\''"\xf1\xbf\xc0"'\''|json'

echo '$ echo -e '\''"\xf1\x80\x80"'\''|json'
echo -e '"\xf1\x80\x80"'|json 2>&1 ||
echo 'command failed: echo -e '\''"\xf1\x80\x80"'\''|json'

echo '$ echo -e '\''"\xf1\xbf\xbf"'\''|json'
echo -e '"\xf1\xbf\xbf"'|json 2>&1 ||
echo 'command failed: echo -e '\''"\xf1\xbf\xbf"'\''|json'

echo '$ echo -e '\''"\xf1\x80\x80\x7f"'\''|json'
echo -e '"\xf1\x80\x80\x7f"'|json 2>&1 ||
echo 'command failed: echo -e '\''"\xf1\x80\x80\x7f"'\''|json'

echo '$ echo -e '\''"\xf1\xbf\xbf\xc0"'\''|json'
echo -e '"\xf1\xbf\xbf\xc0"'|json 2>&1 ||
echo 'command failed: echo -e '\''"\xf1\xbf\xbf\xc0"'\''|json'

echo '$ echo -e '\''"\xf3"'\''|json'
echo -e '"\xf3"'|json 2>&1 ||
echo 'command failed: echo -e '\''"\xf3"'\''|json'

echo '$ echo -e '\''"\xf3\x7f"'\''|json'
echo -e '"\xf3\x7f"'|json 2>&1 ||
echo 'command failed: echo -e '\''"\xf3\x7f"'\''|json'

echo '$ echo -e '\''"\xf3\xc0"'\''|json'
echo -e '"\xf3\xc0"'|json 2>&1 ||
echo 'command failed: echo -e '\''"\xf3\xc0"'\''|json'

echo '$ echo -e '\''"\xf3\x80"'\''|json'
echo -e '"\xf3\x80"'|json 2>&1 ||
echo 'command failed: echo -e '\''"\xf3\x80"'\''|json'

echo '$ echo -e '\''"\xf3\xbf"'\''|json'
echo -e '"\xf3\xbf"'|json 2>&1 ||
echo 'command failed: echo -e '\''"\xf3\xbf"'\''|json'

echo '$ echo -e '\''"\xf3\x80\x7f"'\''|json'
echo -e '"\xf3\x80\x7f"'|json 2>&1 ||
echo 'command failed: echo -e '\''"\xf3\x80\x7f"'\''|json'

echo '$ echo -e '\''"\xf3\xbf\xc0"'\''|json'
echo -e '"\xf3\xbf\xc0"'|json 2>&1 ||
echo 'command failed: echo -e '\''"\xf3\xbf\xc0"'\''|json'

echo '$ echo -e '\''"\xf3\x80\x80"'\''|json'
echo -e '"\xf3\x80\x80"'|json 2>&1 ||
echo 'command failed: echo -e '\''"\xf3\x80\x80"'\''|json'

echo '$ echo -e '\''"\xf3\xbf\xbf"'\''|json'
echo -e '"\xf3\xbf\xbf"'|json 2>&1 ||
echo 'command failed: echo -e '\''"\xf3\xbf\xbf"'\''|json'

echo '$ echo -e '\''"\xf3\x80\x80\x7f"'\''|json'
echo -e '"\xf3\x80\x80\x7f"'|json 2>&1 ||
echo 'command failed: echo -e '\''"\xf3\x80\x80\x7f"'\''|json'

echo '$ echo -e '\''"\xf3\xbf\xbf\xc0"'\''|json'
echo -e '"\xf3\xbf\xbf\xc0"'|json 2>&1 ||
echo 'command failed: echo -e '\''"\xf3\xbf\xbf\xc0"'\''|json'

echo '$ echo -e '\''"\xf4"'\''|json'
echo -e '"\xf4"'|json 2>&1 ||
echo 'command failed: echo -e '\''"\xf4"'\''|json'

echo '$ echo -e '\''"\xf4\x7f"'\''|json'
echo -e '"\xf4\x7f"'|json 2>&1 ||
echo 'command failed: echo -e '\''"\xf4\x7f"'\''|json'

echo '$ echo -e '\''"\xf4\x90"'\''|json'
echo -e '"\xf4\x90"'|json 2>&1 ||
echo 'command failed: echo -e '\''"\xf4\x90"'\''|json'

echo '$ echo -e '\''"\xf4\x80"'\''|json'
echo -e '"\xf4\x80"'|json 2>&1 ||
echo 'command failed: echo -e '\''"\xf4\x80"'\''|json'

echo '$ echo -e '\''"\xf4\x8f"'\''|json'
echo -e '"\xf4\x8f"'|json 2>&1 ||
echo 'command failed: echo -e '\''"\xf4\x8f"'\''|json'

echo '$ echo -e '\''"\xf4\x80\x7f"'\''|json'
echo -e '"\xf4\x80\x7f"'|json 2>&1 ||
echo 'command failed: echo -e '\''"\xf4\x80\x7f"'\''|json'

echo '$ echo -e '\''"\xf4\x8f\xc0"'\''|json'
echo -e '"\xf4\x8f\xc0"'|json 2>&1 ||
echo 'command failed: echo -e '\''"\xf4\x8f\xc0"'\''|json'

echo '$ echo -e '\''"\xf4\x80\x80"'\''|json'
echo -e '"\xf4\x80\x80"'|json 2>&1 ||
echo 'command failed: echo -e '\''"\xf4\x80\x80"'\''|json'

echo '$ echo -e '\''"\xf4\x8f\xbf"'\''|json'
echo -e '"\xf4\x8f\xbf"'|json 2>&1 ||
echo 'command failed: echo -e '\''"\xf4\x8f\xbf"'\''|json'

echo '$ echo -e '\''"\xf4\x80\x80\x7f"'\''|json'
echo -e '"\xf4\x80\x80\x7f"'|json 2>&1 ||
echo 'command failed: echo -e '\''"\xf4\x80\x80\x7f"'\''|json'

echo '$ echo -e '\''"\xf4\x8f\xbf\xc0"'\''|json'
echo -e '"\xf4\x8f\xbf\xc0"'|json 2>&1 ||
echo 'command failed: echo -e '\''"\xf4\x8f\xbf\xc0"'\''|json'

echo '$ echo -e '\''"\xf5"'\''|json'
echo -e '"\xf5"'|json 2>&1 ||
echo 'command failed: echo -e '\''"\xf5"'\''|json'

echo '$ echo -e '\''"\x41\xe2\x89\xa2\xce\x91\x2e"'\''|json'
echo -e '"\x41\xe2\x89\xa2\xce\x91\x2e"'|json 2>&1 ||
echo 'command failed: echo -e '\''"\x41\xe2\x89\xa2\xce\x91\x2e"'\''|json'
)

