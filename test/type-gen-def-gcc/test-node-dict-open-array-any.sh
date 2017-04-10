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
# $ json-gentest -C type-gen-def-gcc:node-dict-open-array-any
#

[[ "$1" =~ ^-u[0-9]+$ ]] &&
u="${1:2}" ||
u=""

diff -u$u -L node-dict-open-array-any.old <(echo \
'$ set -o pipefail
$ json() { LD_LIBRARY_PATH=../lib ../src/json -Td "$@"|sed -r $'\''1i\\\\\\#include "json-type.h"\n'\'''\'';s/^#/\\#/;/^\s*\/\//d;/^\s*$/d'\''|tee /dev/fd/2|sed -r '\''s/^\\//'\''|gcc -Wall -Wextra -std=gnu99 -I ../lib -xc -c - -o /dev/null; }
$ json <<< '\''{"type":"dict","args":[{"name":"foo","type":{"type":"array","args":"type"}}]}'\''
\#include "json-type.h"
\#define EQ  cell.eq
\#define VAL cell.val
static const struct json_type_node_t __0 = {
    .type = json_type_any_node_type,
    .node.any = {
        .type = json_type_any_type_type
    }
};
static const struct json_type_node_t __1 = {
    .type = json_type_array_node_type,
    .node.array = {
        .type = json_type_open_array_node_type,
        .val.open.arg = &__0
    }
};
static const struct json_type_dict_trie_node_t __2 = {
    .sym = '\''\0'\'',
    .VAL = 0
};
static const struct json_type_dict_trie_node_t __3 = {
    .sym = '\''o'\'',
    .EQ = &__2
};
static const struct json_type_dict_trie_node_t __4 = {
    .sym = '\''o'\'',
    .EQ = &__3
};
static const struct json_type_dict_trie_node_t __5 = {
    .sym = '\''f'\'',
    .EQ = &__4
};
static const struct json_type_dict_trie_t __6 = {
    .root = &__5
};
static const struct json_type_dict_node_arg_t __7[] = {
    {
        .name = (const uchar_t*) "foo",
        .type = &__1
    }
};
static const struct json_type_node_t __8 = {
    .type = json_type_dict_node_type,
    .attr.dict = &__6,
    .node.dict = {
        .args = __7,
        .size = 1
    }
};
static const struct json_type_def_t MODULE_TYPE_DEF = {
    .type = json_type_def_node_type,
    .val.node = &__8
};
$ json <<< '\''{"type":"dict","args":[{"name":"foo","type":{"type":"array","args":"null"}}]}'\''
\#include "json-type.h"
\#define EQ  cell.eq
\#define VAL cell.val
static const struct json_type_node_t __0 = {
    .type = json_type_any_node_type,
    .node.any = {
        .type = json_type_any_null_type
    }
};
static const struct json_type_node_t __1 = {
    .type = json_type_array_node_type,
    .node.array = {
        .type = json_type_open_array_node_type,
        .val.open.arg = &__0
    }
};
static const struct json_type_dict_trie_node_t __2 = {
    .sym = '\''\0'\'',
    .VAL = 0
};
static const struct json_type_dict_trie_node_t __3 = {
    .sym = '\''o'\'',
    .EQ = &__2
};
static const struct json_type_dict_trie_node_t __4 = {
    .sym = '\''o'\'',
    .EQ = &__3
};
static const struct json_type_dict_trie_node_t __5 = {
    .sym = '\''f'\'',
    .EQ = &__4
};
static const struct json_type_dict_trie_t __6 = {
    .root = &__5
};
static const struct json_type_dict_node_arg_t __7[] = {
    {
        .name = (const uchar_t*) "foo",
        .type = &__1
    }
};
static const struct json_type_node_t __8 = {
    .type = json_type_dict_node_type,
    .attr.dict = &__6,
    .node.dict = {
        .args = __7,
        .size = 1
    }
};
static const struct json_type_def_t MODULE_TYPE_DEF = {
    .type = json_type_def_node_type,
    .val.node = &__8
};
$ json <<< '\''{"type":"dict","args":[{"name":"foo","type":{"type":"array","args":"boolean"}}]}'\''
\#include "json-type.h"
\#define EQ  cell.eq
\#define VAL cell.val
static const struct json_type_node_t __0 = {
    .type = json_type_any_node_type,
    .node.any = {
        .type = json_type_any_boolean_type
    }
};
static const struct json_type_node_t __1 = {
    .type = json_type_array_node_type,
    .node.array = {
        .type = json_type_open_array_node_type,
        .val.open.arg = &__0
    }
};
static const struct json_type_dict_trie_node_t __2 = {
    .sym = '\''\0'\'',
    .VAL = 0
};
static const struct json_type_dict_trie_node_t __3 = {
    .sym = '\''o'\'',
    .EQ = &__2
};
static const struct json_type_dict_trie_node_t __4 = {
    .sym = '\''o'\'',
    .EQ = &__3
};
static const struct json_type_dict_trie_node_t __5 = {
    .sym = '\''f'\'',
    .EQ = &__4
};
static const struct json_type_dict_trie_t __6 = {
    .root = &__5
};
static const struct json_type_dict_node_arg_t __7[] = {
    {
        .name = (const uchar_t*) "foo",
        .type = &__1
    }
};
static const struct json_type_node_t __8 = {
    .type = json_type_dict_node_type,
    .attr.dict = &__6,
    .node.dict = {
        .args = __7,
        .size = 1
    }
};
static const struct json_type_def_t MODULE_TYPE_DEF = {
    .type = json_type_def_node_type,
    .val.node = &__8
};
$ json <<< '\''{"type":"dict","args":[{"name":"foo","type":{"type":"array","args":"number"}}]}'\''
\#include "json-type.h"
\#define EQ  cell.eq
\#define VAL cell.val
static const struct json_type_node_t __0 = {
    .type = json_type_any_node_type,
    .node.any = {
        .type = json_type_any_number_type
    }
};
static const struct json_type_node_t __1 = {
    .type = json_type_array_node_type,
    .node.array = {
        .type = json_type_open_array_node_type,
        .val.open.arg = &__0
    }
};
static const struct json_type_dict_trie_node_t __2 = {
    .sym = '\''\0'\'',
    .VAL = 0
};
static const struct json_type_dict_trie_node_t __3 = {
    .sym = '\''o'\'',
    .EQ = &__2
};
static const struct json_type_dict_trie_node_t __4 = {
    .sym = '\''o'\'',
    .EQ = &__3
};
static const struct json_type_dict_trie_node_t __5 = {
    .sym = '\''f'\'',
    .EQ = &__4
};
static const struct json_type_dict_trie_t __6 = {
    .root = &__5
};
static const struct json_type_dict_node_arg_t __7[] = {
    {
        .name = (const uchar_t*) "foo",
        .type = &__1
    }
};
static const struct json_type_node_t __8 = {
    .type = json_type_dict_node_type,
    .attr.dict = &__6,
    .node.dict = {
        .args = __7,
        .size = 1
    }
};
static const struct json_type_def_t MODULE_TYPE_DEF = {
    .type = json_type_def_node_type,
    .val.node = &__8
};
$ json <<< '\''{"type":"dict","args":[{"name":"foo","type":{"type":"array","args":"string"}}]}'\''
\#include "json-type.h"
\#define EQ  cell.eq
\#define VAL cell.val
static const struct json_type_node_t __0 = {
    .type = json_type_any_node_type,
    .node.any = {
        .type = json_type_any_string_type
    }
};
static const struct json_type_node_t __1 = {
    .type = json_type_array_node_type,
    .node.array = {
        .type = json_type_open_array_node_type,
        .val.open.arg = &__0
    }
};
static const struct json_type_dict_trie_node_t __2 = {
    .sym = '\''\0'\'',
    .VAL = 0
};
static const struct json_type_dict_trie_node_t __3 = {
    .sym = '\''o'\'',
    .EQ = &__2
};
static const struct json_type_dict_trie_node_t __4 = {
    .sym = '\''o'\'',
    .EQ = &__3
};
static const struct json_type_dict_trie_node_t __5 = {
    .sym = '\''f'\'',
    .EQ = &__4
};
static const struct json_type_dict_trie_t __6 = {
    .root = &__5
};
static const struct json_type_dict_node_arg_t __7[] = {
    {
        .name = (const uchar_t*) "foo",
        .type = &__1
    }
};
static const struct json_type_node_t __8 = {
    .type = json_type_dict_node_type,
    .attr.dict = &__6,
    .node.dict = {
        .args = __7,
        .size = 1
    }
};
static const struct json_type_def_t MODULE_TYPE_DEF = {
    .type = json_type_def_node_type,
    .val.node = &__8
};
$ json <<< '\''{"type":"dict","args":[{"name":"foo","type":{"type":"array","args":"object"}}]}'\''
\#include "json-type.h"
\#define EQ  cell.eq
\#define VAL cell.val
static const struct json_type_node_t __0 = {
    .type = json_type_any_node_type,
    .node.any = {
        .type = json_type_any_object_type
    }
};
static const struct json_type_node_t __1 = {
    .type = json_type_array_node_type,
    .node.array = {
        .type = json_type_open_array_node_type,
        .val.open.arg = &__0
    }
};
static const struct json_type_dict_trie_node_t __2 = {
    .sym = '\''\0'\'',
    .VAL = 0
};
static const struct json_type_dict_trie_node_t __3 = {
    .sym = '\''o'\'',
    .EQ = &__2
};
static const struct json_type_dict_trie_node_t __4 = {
    .sym = '\''o'\'',
    .EQ = &__3
};
static const struct json_type_dict_trie_node_t __5 = {
    .sym = '\''f'\'',
    .EQ = &__4
};
static const struct json_type_dict_trie_t __6 = {
    .root = &__5
};
static const struct json_type_dict_node_arg_t __7[] = {
    {
        .name = (const uchar_t*) "foo",
        .type = &__1
    }
};
static const struct json_type_node_t __8 = {
    .type = json_type_dict_node_type,
    .attr.dict = &__6,
    .node.dict = {
        .args = __7,
        .size = 1
    }
};
static const struct json_type_def_t MODULE_TYPE_DEF = {
    .type = json_type_def_node_type,
    .val.node = &__8
};
$ json <<< '\''{"type":"dict","args":[{"name":"foo","type":{"type":"array","args":"array"}}]}'\''
\#include "json-type.h"
\#define EQ  cell.eq
\#define VAL cell.val
static const struct json_type_node_t __0 = {
    .type = json_type_any_node_type,
    .node.any = {
        .type = json_type_any_array_type
    }
};
static const struct json_type_node_t __1 = {
    .type = json_type_array_node_type,
    .node.array = {
        .type = json_type_open_array_node_type,
        .val.open.arg = &__0
    }
};
static const struct json_type_dict_trie_node_t __2 = {
    .sym = '\''\0'\'',
    .VAL = 0
};
static const struct json_type_dict_trie_node_t __3 = {
    .sym = '\''o'\'',
    .EQ = &__2
};
static const struct json_type_dict_trie_node_t __4 = {
    .sym = '\''o'\'',
    .EQ = &__3
};
static const struct json_type_dict_trie_node_t __5 = {
    .sym = '\''f'\'',
    .EQ = &__4
};
static const struct json_type_dict_trie_t __6 = {
    .root = &__5
};
static const struct json_type_dict_node_arg_t __7[] = {
    {
        .name = (const uchar_t*) "foo",
        .type = &__1
    }
};
static const struct json_type_node_t __8 = {
    .type = json_type_dict_node_type,
    .attr.dict = &__6,
    .node.dict = {
        .args = __7,
        .size = 1
    }
};
static const struct json_type_def_t MODULE_TYPE_DEF = {
    .type = json_type_def_node_type,
    .val.node = &__8
};'
) -L node-dict-open-array-any.new <(
echo '$ set -o pipefail'
set -o pipefail 2>&1 ||
echo 'command failed: set -o pipefail'

echo '$ json() { LD_LIBRARY_PATH=../lib ../src/json -Td "$@"|sed -r $'\''1i\\\\\\#include "json-type.h"\n'\'''\'';s/^#/\\#/;/^\s*\/\//d;/^\s*$/d'\''|tee /dev/fd/2|sed -r '\''s/^\\//'\''|gcc -Wall -Wextra -std=gnu99 -I ../lib -xc -c - -o /dev/null; }'
json() { LD_LIBRARY_PATH=../lib ../src/json -Td "$@"|sed -r $'1i\\\\\\#include "json-type.h"\n'';s/^#/\\#/;/^\s*\/\//d;/^\s*$/d'|tee /dev/fd/2|sed -r 's/^\\//'|gcc -Wall -Wextra -std=gnu99 -I ../lib -xc -c - -o /dev/null; } 2>&1 ||
echo 'command failed: json() { LD_LIBRARY_PATH=../lib ../src/json -Td "$@"|sed -r $'\''1i\\\\\\#include "json-type.h"\n'\'''\'';s/^#/\\#/;/^\s*\/\//d;/^\s*$/d'\''|tee /dev/fd/2|sed -r '\''s/^\\//'\''|gcc -Wall -Wextra -std=gnu99 -I ../lib -xc -c - -o /dev/null; }'

echo '$ json <<< '\''{"type":"dict","args":[{"name":"foo","type":{"type":"array","args":"type"}}]}'\'''
json <<< '{"type":"dict","args":[{"name":"foo","type":{"type":"array","args":"type"}}]}' 2>&1 ||
echo 'command failed: json <<< '\''{"type":"dict","args":[{"name":"foo","type":{"type":"array","args":"type"}}]}'\'''

echo '$ json <<< '\''{"type":"dict","args":[{"name":"foo","type":{"type":"array","args":"null"}}]}'\'''
json <<< '{"type":"dict","args":[{"name":"foo","type":{"type":"array","args":"null"}}]}' 2>&1 ||
echo 'command failed: json <<< '\''{"type":"dict","args":[{"name":"foo","type":{"type":"array","args":"null"}}]}'\'''

echo '$ json <<< '\''{"type":"dict","args":[{"name":"foo","type":{"type":"array","args":"boolean"}}]}'\'''
json <<< '{"type":"dict","args":[{"name":"foo","type":{"type":"array","args":"boolean"}}]}' 2>&1 ||
echo 'command failed: json <<< '\''{"type":"dict","args":[{"name":"foo","type":{"type":"array","args":"boolean"}}]}'\'''

echo '$ json <<< '\''{"type":"dict","args":[{"name":"foo","type":{"type":"array","args":"number"}}]}'\'''
json <<< '{"type":"dict","args":[{"name":"foo","type":{"type":"array","args":"number"}}]}' 2>&1 ||
echo 'command failed: json <<< '\''{"type":"dict","args":[{"name":"foo","type":{"type":"array","args":"number"}}]}'\'''

echo '$ json <<< '\''{"type":"dict","args":[{"name":"foo","type":{"type":"array","args":"string"}}]}'\'''
json <<< '{"type":"dict","args":[{"name":"foo","type":{"type":"array","args":"string"}}]}' 2>&1 ||
echo 'command failed: json <<< '\''{"type":"dict","args":[{"name":"foo","type":{"type":"array","args":"string"}}]}'\'''

echo '$ json <<< '\''{"type":"dict","args":[{"name":"foo","type":{"type":"array","args":"object"}}]}'\'''
json <<< '{"type":"dict","args":[{"name":"foo","type":{"type":"array","args":"object"}}]}' 2>&1 ||
echo 'command failed: json <<< '\''{"type":"dict","args":[{"name":"foo","type":{"type":"array","args":"object"}}]}'\'''

echo '$ json <<< '\''{"type":"dict","args":[{"name":"foo","type":{"type":"array","args":"array"}}]}'\'''
json <<< '{"type":"dict","args":[{"name":"foo","type":{"type":"array","args":"array"}}]}' 2>&1 ||
echo 'command failed: json <<< '\''{"type":"dict","args":[{"name":"foo","type":{"type":"array","args":"array"}}]}'\'''
)

