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
# $ json-gentest -C type-gen-def:node-closed-array-plain2
#

[[ "$1" =~ ^-u[0-9]+$ ]] &&
u="${1:2}" ||
u=""

diff -u$u -L node-closed-array-plain2.old <(echo \
'$ set -o pipefail
$ json() { LD_LIBRARY_PATH=../lib ../src/json -Td "$@"|sed -r '\''s/^#/\\#/;/^\s*\/\//d;/^\s*$/d'\''; }
$ json <<< '\''{"type":"array","args":[{"plain":null},{"plain":null}]}'\''
static const struct json_type_node_t __0 = {
    .type = json_type_plain_node_type,
    .node.plain = {
        .type = json_type_plain_null_type,
    }
};
static const struct json_type_node_t __1 = {
    .type = json_type_plain_node_type,
    .node.plain = {
        .type = json_type_plain_null_type,
    }
};
static const struct json_type_node_t* __2[] = {
    &__0,
    &__1,
    NULL
};
static const struct json_type_node_t __3 = {
    .type = json_type_array_node_type,
    .node.array = {
        .type = json_type_closed_array_node_type,
        .val.closed = {
            .args = __2,
            .size = 2
        }
    }
};
static const struct json_type_def_t MODULE_TYPE_DEF = {
    .type = json_type_def_node_type,
    .val.node = &__3
};
$ json <<< '\''{"type":"array","args":[{"plain":null},{"plain":false}]}'\''
static const struct json_type_node_t __0 = {
    .type = json_type_plain_node_type,
    .node.plain = {
        .type = json_type_plain_null_type,
    }
};
static const struct json_type_node_t __1 = {
    .type = json_type_plain_node_type,
    .node.plain = {
        .type = json_type_plain_boolean_type,
        .val.boolean = false
    }
};
static const struct json_type_node_t* __2[] = {
    &__0,
    &__1,
    NULL
};
static const struct json_type_node_t __3 = {
    .type = json_type_array_node_type,
    .node.array = {
        .type = json_type_closed_array_node_type,
        .val.closed = {
            .args = __2,
            .size = 2
        }
    }
};
static const struct json_type_def_t MODULE_TYPE_DEF = {
    .type = json_type_def_node_type,
    .val.node = &__3
};
$ json <<< '\''{"type":"array","args":[{"plain":null},{"plain":true}]}'\''
static const struct json_type_node_t __0 = {
    .type = json_type_plain_node_type,
    .node.plain = {
        .type = json_type_plain_null_type,
    }
};
static const struct json_type_node_t __1 = {
    .type = json_type_plain_node_type,
    .node.plain = {
        .type = json_type_plain_boolean_type,
        .val.boolean = true
    }
};
static const struct json_type_node_t* __2[] = {
    &__0,
    &__1,
    NULL
};
static const struct json_type_node_t __3 = {
    .type = json_type_array_node_type,
    .node.array = {
        .type = json_type_closed_array_node_type,
        .val.closed = {
            .args = __2,
            .size = 2
        }
    }
};
static const struct json_type_def_t MODULE_TYPE_DEF = {
    .type = json_type_def_node_type,
    .val.node = &__3
};
$ json <<< '\''{"type":"array","args":[{"plain":null},{"plain":123}]}'\''
static const struct json_type_node_t __0 = {
    .type = json_type_plain_node_type,
    .node.plain = {
        .type = json_type_plain_null_type,
    }
};
static const struct json_type_node_t __1 = {
    .type = json_type_plain_node_type,
    .node.plain = {
        .type = json_type_plain_number_type,
        .val.number = (const uchar_t*) "123"
    }
};
static const struct json_type_node_t* __2[] = {
    &__0,
    &__1,
    NULL
};
static const struct json_type_node_t __3 = {
    .type = json_type_array_node_type,
    .node.array = {
        .type = json_type_closed_array_node_type,
        .val.closed = {
            .args = __2,
            .size = 2
        }
    }
};
static const struct json_type_def_t MODULE_TYPE_DEF = {
    .type = json_type_def_node_type,
    .val.node = &__3
};
$ json <<< '\''{"type":"array","args":[{"plain":null},{"plain":"foo"}]}'\''
static const struct json_type_node_t __0 = {
    .type = json_type_plain_node_type,
    .node.plain = {
        .type = json_type_plain_null_type,
    }
};
static const struct json_type_node_t __1 = {
    .type = json_type_plain_node_type,
    .node.plain = {
        .type = json_type_plain_string_type,
        .val.string = (const uchar_t*) "foo"
    }
};
static const struct json_type_node_t* __2[] = {
    &__0,
    &__1,
    NULL
};
static const struct json_type_node_t __3 = {
    .type = json_type_array_node_type,
    .node.array = {
        .type = json_type_closed_array_node_type,
        .val.closed = {
            .args = __2,
            .size = 2
        }
    }
};
static const struct json_type_def_t MODULE_TYPE_DEF = {
    .type = json_type_def_node_type,
    .val.node = &__3
};
$ json <<< '\''{"type":"array","args":[{"plain":false},{"plain":null}]}'\''
static const struct json_type_node_t __0 = {
    .type = json_type_plain_node_type,
    .node.plain = {
        .type = json_type_plain_boolean_type,
        .val.boolean = false
    }
};
static const struct json_type_node_t __1 = {
    .type = json_type_plain_node_type,
    .node.plain = {
        .type = json_type_plain_null_type,
    }
};
static const struct json_type_node_t* __2[] = {
    &__0,
    &__1,
    NULL
};
static const struct json_type_node_t __3 = {
    .type = json_type_array_node_type,
    .node.array = {
        .type = json_type_closed_array_node_type,
        .val.closed = {
            .args = __2,
            .size = 2
        }
    }
};
static const struct json_type_def_t MODULE_TYPE_DEF = {
    .type = json_type_def_node_type,
    .val.node = &__3
};
$ json <<< '\''{"type":"array","args":[{"plain":false},{"plain":false}]}'\''
static const struct json_type_node_t __0 = {
    .type = json_type_plain_node_type,
    .node.plain = {
        .type = json_type_plain_boolean_type,
        .val.boolean = false
    }
};
static const struct json_type_node_t __1 = {
    .type = json_type_plain_node_type,
    .node.plain = {
        .type = json_type_plain_boolean_type,
        .val.boolean = false
    }
};
static const struct json_type_node_t* __2[] = {
    &__0,
    &__1,
    NULL
};
static const struct json_type_node_t __3 = {
    .type = json_type_array_node_type,
    .node.array = {
        .type = json_type_closed_array_node_type,
        .val.closed = {
            .args = __2,
            .size = 2
        }
    }
};
static const struct json_type_def_t MODULE_TYPE_DEF = {
    .type = json_type_def_node_type,
    .val.node = &__3
};
$ json <<< '\''{"type":"array","args":[{"plain":false},{"plain":true}]}'\''
static const struct json_type_node_t __0 = {
    .type = json_type_plain_node_type,
    .node.plain = {
        .type = json_type_plain_boolean_type,
        .val.boolean = false
    }
};
static const struct json_type_node_t __1 = {
    .type = json_type_plain_node_type,
    .node.plain = {
        .type = json_type_plain_boolean_type,
        .val.boolean = true
    }
};
static const struct json_type_node_t* __2[] = {
    &__0,
    &__1,
    NULL
};
static const struct json_type_node_t __3 = {
    .type = json_type_array_node_type,
    .node.array = {
        .type = json_type_closed_array_node_type,
        .val.closed = {
            .args = __2,
            .size = 2
        }
    }
};
static const struct json_type_def_t MODULE_TYPE_DEF = {
    .type = json_type_def_node_type,
    .val.node = &__3
};
$ json <<< '\''{"type":"array","args":[{"plain":false},{"plain":123}]}'\''
static const struct json_type_node_t __0 = {
    .type = json_type_plain_node_type,
    .node.plain = {
        .type = json_type_plain_boolean_type,
        .val.boolean = false
    }
};
static const struct json_type_node_t __1 = {
    .type = json_type_plain_node_type,
    .node.plain = {
        .type = json_type_plain_number_type,
        .val.number = (const uchar_t*) "123"
    }
};
static const struct json_type_node_t* __2[] = {
    &__0,
    &__1,
    NULL
};
static const struct json_type_node_t __3 = {
    .type = json_type_array_node_type,
    .node.array = {
        .type = json_type_closed_array_node_type,
        .val.closed = {
            .args = __2,
            .size = 2
        }
    }
};
static const struct json_type_def_t MODULE_TYPE_DEF = {
    .type = json_type_def_node_type,
    .val.node = &__3
};
$ json <<< '\''{"type":"array","args":[{"plain":false},{"plain":"foo"}]}'\''
static const struct json_type_node_t __0 = {
    .type = json_type_plain_node_type,
    .node.plain = {
        .type = json_type_plain_boolean_type,
        .val.boolean = false
    }
};
static const struct json_type_node_t __1 = {
    .type = json_type_plain_node_type,
    .node.plain = {
        .type = json_type_plain_string_type,
        .val.string = (const uchar_t*) "foo"
    }
};
static const struct json_type_node_t* __2[] = {
    &__0,
    &__1,
    NULL
};
static const struct json_type_node_t __3 = {
    .type = json_type_array_node_type,
    .node.array = {
        .type = json_type_closed_array_node_type,
        .val.closed = {
            .args = __2,
            .size = 2
        }
    }
};
static const struct json_type_def_t MODULE_TYPE_DEF = {
    .type = json_type_def_node_type,
    .val.node = &__3
};
$ json <<< '\''{"type":"array","args":[{"plain":true},{"plain":null}]}'\''
static const struct json_type_node_t __0 = {
    .type = json_type_plain_node_type,
    .node.plain = {
        .type = json_type_plain_boolean_type,
        .val.boolean = true
    }
};
static const struct json_type_node_t __1 = {
    .type = json_type_plain_node_type,
    .node.plain = {
        .type = json_type_plain_null_type,
    }
};
static const struct json_type_node_t* __2[] = {
    &__0,
    &__1,
    NULL
};
static const struct json_type_node_t __3 = {
    .type = json_type_array_node_type,
    .node.array = {
        .type = json_type_closed_array_node_type,
        .val.closed = {
            .args = __2,
            .size = 2
        }
    }
};
static const struct json_type_def_t MODULE_TYPE_DEF = {
    .type = json_type_def_node_type,
    .val.node = &__3
};
$ json <<< '\''{"type":"array","args":[{"plain":true},{"plain":false}]}'\''
static const struct json_type_node_t __0 = {
    .type = json_type_plain_node_type,
    .node.plain = {
        .type = json_type_plain_boolean_type,
        .val.boolean = true
    }
};
static const struct json_type_node_t __1 = {
    .type = json_type_plain_node_type,
    .node.plain = {
        .type = json_type_plain_boolean_type,
        .val.boolean = false
    }
};
static const struct json_type_node_t* __2[] = {
    &__0,
    &__1,
    NULL
};
static const struct json_type_node_t __3 = {
    .type = json_type_array_node_type,
    .node.array = {
        .type = json_type_closed_array_node_type,
        .val.closed = {
            .args = __2,
            .size = 2
        }
    }
};
static const struct json_type_def_t MODULE_TYPE_DEF = {
    .type = json_type_def_node_type,
    .val.node = &__3
};
$ json <<< '\''{"type":"array","args":[{"plain":true},{"plain":true}]}'\''
static const struct json_type_node_t __0 = {
    .type = json_type_plain_node_type,
    .node.plain = {
        .type = json_type_plain_boolean_type,
        .val.boolean = true
    }
};
static const struct json_type_node_t __1 = {
    .type = json_type_plain_node_type,
    .node.plain = {
        .type = json_type_plain_boolean_type,
        .val.boolean = true
    }
};
static const struct json_type_node_t* __2[] = {
    &__0,
    &__1,
    NULL
};
static const struct json_type_node_t __3 = {
    .type = json_type_array_node_type,
    .node.array = {
        .type = json_type_closed_array_node_type,
        .val.closed = {
            .args = __2,
            .size = 2
        }
    }
};
static const struct json_type_def_t MODULE_TYPE_DEF = {
    .type = json_type_def_node_type,
    .val.node = &__3
};
$ json <<< '\''{"type":"array","args":[{"plain":true},{"plain":123}]}'\''
static const struct json_type_node_t __0 = {
    .type = json_type_plain_node_type,
    .node.plain = {
        .type = json_type_plain_boolean_type,
        .val.boolean = true
    }
};
static const struct json_type_node_t __1 = {
    .type = json_type_plain_node_type,
    .node.plain = {
        .type = json_type_plain_number_type,
        .val.number = (const uchar_t*) "123"
    }
};
static const struct json_type_node_t* __2[] = {
    &__0,
    &__1,
    NULL
};
static const struct json_type_node_t __3 = {
    .type = json_type_array_node_type,
    .node.array = {
        .type = json_type_closed_array_node_type,
        .val.closed = {
            .args = __2,
            .size = 2
        }
    }
};
static const struct json_type_def_t MODULE_TYPE_DEF = {
    .type = json_type_def_node_type,
    .val.node = &__3
};
$ json <<< '\''{"type":"array","args":[{"plain":true},{"plain":"foo"}]}'\''
static const struct json_type_node_t __0 = {
    .type = json_type_plain_node_type,
    .node.plain = {
        .type = json_type_plain_boolean_type,
        .val.boolean = true
    }
};
static const struct json_type_node_t __1 = {
    .type = json_type_plain_node_type,
    .node.plain = {
        .type = json_type_plain_string_type,
        .val.string = (const uchar_t*) "foo"
    }
};
static const struct json_type_node_t* __2[] = {
    &__0,
    &__1,
    NULL
};
static const struct json_type_node_t __3 = {
    .type = json_type_array_node_type,
    .node.array = {
        .type = json_type_closed_array_node_type,
        .val.closed = {
            .args = __2,
            .size = 2
        }
    }
};
static const struct json_type_def_t MODULE_TYPE_DEF = {
    .type = json_type_def_node_type,
    .val.node = &__3
};
$ json <<< '\''{"type":"array","args":[{"plain":123},{"plain":null}]}'\''
static const struct json_type_node_t __0 = {
    .type = json_type_plain_node_type,
    .node.plain = {
        .type = json_type_plain_number_type,
        .val.number = (const uchar_t*) "123"
    }
};
static const struct json_type_node_t __1 = {
    .type = json_type_plain_node_type,
    .node.plain = {
        .type = json_type_plain_null_type,
    }
};
static const struct json_type_node_t* __2[] = {
    &__0,
    &__1,
    NULL
};
static const struct json_type_node_t __3 = {
    .type = json_type_array_node_type,
    .node.array = {
        .type = json_type_closed_array_node_type,
        .val.closed = {
            .args = __2,
            .size = 2
        }
    }
};
static const struct json_type_def_t MODULE_TYPE_DEF = {
    .type = json_type_def_node_type,
    .val.node = &__3
};
$ json <<< '\''{"type":"array","args":[{"plain":123},{"plain":false}]}'\''
static const struct json_type_node_t __0 = {
    .type = json_type_plain_node_type,
    .node.plain = {
        .type = json_type_plain_number_type,
        .val.number = (const uchar_t*) "123"
    }
};
static const struct json_type_node_t __1 = {
    .type = json_type_plain_node_type,
    .node.plain = {
        .type = json_type_plain_boolean_type,
        .val.boolean = false
    }
};
static const struct json_type_node_t* __2[] = {
    &__0,
    &__1,
    NULL
};
static const struct json_type_node_t __3 = {
    .type = json_type_array_node_type,
    .node.array = {
        .type = json_type_closed_array_node_type,
        .val.closed = {
            .args = __2,
            .size = 2
        }
    }
};
static const struct json_type_def_t MODULE_TYPE_DEF = {
    .type = json_type_def_node_type,
    .val.node = &__3
};
$ json <<< '\''{"type":"array","args":[{"plain":123},{"plain":true}]}'\''
static const struct json_type_node_t __0 = {
    .type = json_type_plain_node_type,
    .node.plain = {
        .type = json_type_plain_number_type,
        .val.number = (const uchar_t*) "123"
    }
};
static const struct json_type_node_t __1 = {
    .type = json_type_plain_node_type,
    .node.plain = {
        .type = json_type_plain_boolean_type,
        .val.boolean = true
    }
};
static const struct json_type_node_t* __2[] = {
    &__0,
    &__1,
    NULL
};
static const struct json_type_node_t __3 = {
    .type = json_type_array_node_type,
    .node.array = {
        .type = json_type_closed_array_node_type,
        .val.closed = {
            .args = __2,
            .size = 2
        }
    }
};
static const struct json_type_def_t MODULE_TYPE_DEF = {
    .type = json_type_def_node_type,
    .val.node = &__3
};
$ json <<< '\''{"type":"array","args":[{"plain":123},{"plain":123}]}'\''
static const struct json_type_node_t __0 = {
    .type = json_type_plain_node_type,
    .node.plain = {
        .type = json_type_plain_number_type,
        .val.number = (const uchar_t*) "123"
    }
};
static const struct json_type_node_t __1 = {
    .type = json_type_plain_node_type,
    .node.plain = {
        .type = json_type_plain_number_type,
        .val.number = (const uchar_t*) "123"
    }
};
static const struct json_type_node_t* __2[] = {
    &__0,
    &__1,
    NULL
};
static const struct json_type_node_t __3 = {
    .type = json_type_array_node_type,
    .node.array = {
        .type = json_type_closed_array_node_type,
        .val.closed = {
            .args = __2,
            .size = 2
        }
    }
};
static const struct json_type_def_t MODULE_TYPE_DEF = {
    .type = json_type_def_node_type,
    .val.node = &__3
};
$ json <<< '\''{"type":"array","args":[{"plain":123},{"plain":"foo"}]}'\''
static const struct json_type_node_t __0 = {
    .type = json_type_plain_node_type,
    .node.plain = {
        .type = json_type_plain_number_type,
        .val.number = (const uchar_t*) "123"
    }
};
static const struct json_type_node_t __1 = {
    .type = json_type_plain_node_type,
    .node.plain = {
        .type = json_type_plain_string_type,
        .val.string = (const uchar_t*) "foo"
    }
};
static const struct json_type_node_t* __2[] = {
    &__0,
    &__1,
    NULL
};
static const struct json_type_node_t __3 = {
    .type = json_type_array_node_type,
    .node.array = {
        .type = json_type_closed_array_node_type,
        .val.closed = {
            .args = __2,
            .size = 2
        }
    }
};
static const struct json_type_def_t MODULE_TYPE_DEF = {
    .type = json_type_def_node_type,
    .val.node = &__3
};
$ json <<< '\''{"type":"array","args":[{"plain":"foo"},{"plain":null}]}'\''
static const struct json_type_node_t __0 = {
    .type = json_type_plain_node_type,
    .node.plain = {
        .type = json_type_plain_string_type,
        .val.string = (const uchar_t*) "foo"
    }
};
static const struct json_type_node_t __1 = {
    .type = json_type_plain_node_type,
    .node.plain = {
        .type = json_type_plain_null_type,
    }
};
static const struct json_type_node_t* __2[] = {
    &__0,
    &__1,
    NULL
};
static const struct json_type_node_t __3 = {
    .type = json_type_array_node_type,
    .node.array = {
        .type = json_type_closed_array_node_type,
        .val.closed = {
            .args = __2,
            .size = 2
        }
    }
};
static const struct json_type_def_t MODULE_TYPE_DEF = {
    .type = json_type_def_node_type,
    .val.node = &__3
};
$ json <<< '\''{"type":"array","args":[{"plain":"foo"},{"plain":false}]}'\''
static const struct json_type_node_t __0 = {
    .type = json_type_plain_node_type,
    .node.plain = {
        .type = json_type_plain_string_type,
        .val.string = (const uchar_t*) "foo"
    }
};
static const struct json_type_node_t __1 = {
    .type = json_type_plain_node_type,
    .node.plain = {
        .type = json_type_plain_boolean_type,
        .val.boolean = false
    }
};
static const struct json_type_node_t* __2[] = {
    &__0,
    &__1,
    NULL
};
static const struct json_type_node_t __3 = {
    .type = json_type_array_node_type,
    .node.array = {
        .type = json_type_closed_array_node_type,
        .val.closed = {
            .args = __2,
            .size = 2
        }
    }
};
static const struct json_type_def_t MODULE_TYPE_DEF = {
    .type = json_type_def_node_type,
    .val.node = &__3
};
$ json <<< '\''{"type":"array","args":[{"plain":"foo"},{"plain":true}]}'\''
static const struct json_type_node_t __0 = {
    .type = json_type_plain_node_type,
    .node.plain = {
        .type = json_type_plain_string_type,
        .val.string = (const uchar_t*) "foo"
    }
};
static const struct json_type_node_t __1 = {
    .type = json_type_plain_node_type,
    .node.plain = {
        .type = json_type_plain_boolean_type,
        .val.boolean = true
    }
};
static const struct json_type_node_t* __2[] = {
    &__0,
    &__1,
    NULL
};
static const struct json_type_node_t __3 = {
    .type = json_type_array_node_type,
    .node.array = {
        .type = json_type_closed_array_node_type,
        .val.closed = {
            .args = __2,
            .size = 2
        }
    }
};
static const struct json_type_def_t MODULE_TYPE_DEF = {
    .type = json_type_def_node_type,
    .val.node = &__3
};
$ json <<< '\''{"type":"array","args":[{"plain":"foo"},{"plain":123}]}'\''
static const struct json_type_node_t __0 = {
    .type = json_type_plain_node_type,
    .node.plain = {
        .type = json_type_plain_string_type,
        .val.string = (const uchar_t*) "foo"
    }
};
static const struct json_type_node_t __1 = {
    .type = json_type_plain_node_type,
    .node.plain = {
        .type = json_type_plain_number_type,
        .val.number = (const uchar_t*) "123"
    }
};
static const struct json_type_node_t* __2[] = {
    &__0,
    &__1,
    NULL
};
static const struct json_type_node_t __3 = {
    .type = json_type_array_node_type,
    .node.array = {
        .type = json_type_closed_array_node_type,
        .val.closed = {
            .args = __2,
            .size = 2
        }
    }
};
static const struct json_type_def_t MODULE_TYPE_DEF = {
    .type = json_type_def_node_type,
    .val.node = &__3
};
$ json <<< '\''{"type":"array","args":[{"plain":"foo"},{"plain":"foo"}]}'\''
static const struct json_type_node_t __0 = {
    .type = json_type_plain_node_type,
    .node.plain = {
        .type = json_type_plain_string_type,
        .val.string = (const uchar_t*) "foo"
    }
};
static const struct json_type_node_t __1 = {
    .type = json_type_plain_node_type,
    .node.plain = {
        .type = json_type_plain_string_type,
        .val.string = (const uchar_t*) "foo"
    }
};
static const struct json_type_node_t* __2[] = {
    &__0,
    &__1,
    NULL
};
static const struct json_type_node_t __3 = {
    .type = json_type_array_node_type,
    .node.array = {
        .type = json_type_closed_array_node_type,
        .val.closed = {
            .args = __2,
            .size = 2
        }
    }
};
static const struct json_type_def_t MODULE_TYPE_DEF = {
    .type = json_type_def_node_type,
    .val.node = &__3
};'
) -L node-closed-array-plain2.new <(
echo '$ set -o pipefail'
set -o pipefail 2>&1 ||
echo 'command failed: set -o pipefail'

echo '$ json() { LD_LIBRARY_PATH=../lib ../src/json -Td "$@"|sed -r '\''s/^#/\\#/;/^\s*\/\//d;/^\s*$/d'\''; }'
json() { LD_LIBRARY_PATH=../lib ../src/json -Td "$@"|sed -r 's/^#/\\#/;/^\s*\/\//d;/^\s*$/d'; } 2>&1 ||
echo 'command failed: json() { LD_LIBRARY_PATH=../lib ../src/json -Td "$@"|sed -r '\''s/^#/\\#/;/^\s*\/\//d;/^\s*$/d'\''; }'

echo '$ json <<< '\''{"type":"array","args":[{"plain":null},{"plain":null}]}'\'''
json <<< '{"type":"array","args":[{"plain":null},{"plain":null}]}' 2>&1 ||
echo 'command failed: json <<< '\''{"type":"array","args":[{"plain":null},{"plain":null}]}'\'''

echo '$ json <<< '\''{"type":"array","args":[{"plain":null},{"plain":false}]}'\'''
json <<< '{"type":"array","args":[{"plain":null},{"plain":false}]}' 2>&1 ||
echo 'command failed: json <<< '\''{"type":"array","args":[{"plain":null},{"plain":false}]}'\'''

echo '$ json <<< '\''{"type":"array","args":[{"plain":null},{"plain":true}]}'\'''
json <<< '{"type":"array","args":[{"plain":null},{"plain":true}]}' 2>&1 ||
echo 'command failed: json <<< '\''{"type":"array","args":[{"plain":null},{"plain":true}]}'\'''

echo '$ json <<< '\''{"type":"array","args":[{"plain":null},{"plain":123}]}'\'''
json <<< '{"type":"array","args":[{"plain":null},{"plain":123}]}' 2>&1 ||
echo 'command failed: json <<< '\''{"type":"array","args":[{"plain":null},{"plain":123}]}'\'''

echo '$ json <<< '\''{"type":"array","args":[{"plain":null},{"plain":"foo"}]}'\'''
json <<< '{"type":"array","args":[{"plain":null},{"plain":"foo"}]}' 2>&1 ||
echo 'command failed: json <<< '\''{"type":"array","args":[{"plain":null},{"plain":"foo"}]}'\'''

echo '$ json <<< '\''{"type":"array","args":[{"plain":false},{"plain":null}]}'\'''
json <<< '{"type":"array","args":[{"plain":false},{"plain":null}]}' 2>&1 ||
echo 'command failed: json <<< '\''{"type":"array","args":[{"plain":false},{"plain":null}]}'\'''

echo '$ json <<< '\''{"type":"array","args":[{"plain":false},{"plain":false}]}'\'''
json <<< '{"type":"array","args":[{"plain":false},{"plain":false}]}' 2>&1 ||
echo 'command failed: json <<< '\''{"type":"array","args":[{"plain":false},{"plain":false}]}'\'''

echo '$ json <<< '\''{"type":"array","args":[{"plain":false},{"plain":true}]}'\'''
json <<< '{"type":"array","args":[{"plain":false},{"plain":true}]}' 2>&1 ||
echo 'command failed: json <<< '\''{"type":"array","args":[{"plain":false},{"plain":true}]}'\'''

echo '$ json <<< '\''{"type":"array","args":[{"plain":false},{"plain":123}]}'\'''
json <<< '{"type":"array","args":[{"plain":false},{"plain":123}]}' 2>&1 ||
echo 'command failed: json <<< '\''{"type":"array","args":[{"plain":false},{"plain":123}]}'\'''

echo '$ json <<< '\''{"type":"array","args":[{"plain":false},{"plain":"foo"}]}'\'''
json <<< '{"type":"array","args":[{"plain":false},{"plain":"foo"}]}' 2>&1 ||
echo 'command failed: json <<< '\''{"type":"array","args":[{"plain":false},{"plain":"foo"}]}'\'''

echo '$ json <<< '\''{"type":"array","args":[{"plain":true},{"plain":null}]}'\'''
json <<< '{"type":"array","args":[{"plain":true},{"plain":null}]}' 2>&1 ||
echo 'command failed: json <<< '\''{"type":"array","args":[{"plain":true},{"plain":null}]}'\'''

echo '$ json <<< '\''{"type":"array","args":[{"plain":true},{"plain":false}]}'\'''
json <<< '{"type":"array","args":[{"plain":true},{"plain":false}]}' 2>&1 ||
echo 'command failed: json <<< '\''{"type":"array","args":[{"plain":true},{"plain":false}]}'\'''

echo '$ json <<< '\''{"type":"array","args":[{"plain":true},{"plain":true}]}'\'''
json <<< '{"type":"array","args":[{"plain":true},{"plain":true}]}' 2>&1 ||
echo 'command failed: json <<< '\''{"type":"array","args":[{"plain":true},{"plain":true}]}'\'''

echo '$ json <<< '\''{"type":"array","args":[{"plain":true},{"plain":123}]}'\'''
json <<< '{"type":"array","args":[{"plain":true},{"plain":123}]}' 2>&1 ||
echo 'command failed: json <<< '\''{"type":"array","args":[{"plain":true},{"plain":123}]}'\'''

echo '$ json <<< '\''{"type":"array","args":[{"plain":true},{"plain":"foo"}]}'\'''
json <<< '{"type":"array","args":[{"plain":true},{"plain":"foo"}]}' 2>&1 ||
echo 'command failed: json <<< '\''{"type":"array","args":[{"plain":true},{"plain":"foo"}]}'\'''

echo '$ json <<< '\''{"type":"array","args":[{"plain":123},{"plain":null}]}'\'''
json <<< '{"type":"array","args":[{"plain":123},{"plain":null}]}' 2>&1 ||
echo 'command failed: json <<< '\''{"type":"array","args":[{"plain":123},{"plain":null}]}'\'''

echo '$ json <<< '\''{"type":"array","args":[{"plain":123},{"plain":false}]}'\'''
json <<< '{"type":"array","args":[{"plain":123},{"plain":false}]}' 2>&1 ||
echo 'command failed: json <<< '\''{"type":"array","args":[{"plain":123},{"plain":false}]}'\'''

echo '$ json <<< '\''{"type":"array","args":[{"plain":123},{"plain":true}]}'\'''
json <<< '{"type":"array","args":[{"plain":123},{"plain":true}]}' 2>&1 ||
echo 'command failed: json <<< '\''{"type":"array","args":[{"plain":123},{"plain":true}]}'\'''

echo '$ json <<< '\''{"type":"array","args":[{"plain":123},{"plain":123}]}'\'''
json <<< '{"type":"array","args":[{"plain":123},{"plain":123}]}' 2>&1 ||
echo 'command failed: json <<< '\''{"type":"array","args":[{"plain":123},{"plain":123}]}'\'''

echo '$ json <<< '\''{"type":"array","args":[{"plain":123},{"plain":"foo"}]}'\'''
json <<< '{"type":"array","args":[{"plain":123},{"plain":"foo"}]}' 2>&1 ||
echo 'command failed: json <<< '\''{"type":"array","args":[{"plain":123},{"plain":"foo"}]}'\'''

echo '$ json <<< '\''{"type":"array","args":[{"plain":"foo"},{"plain":null}]}'\'''
json <<< '{"type":"array","args":[{"plain":"foo"},{"plain":null}]}' 2>&1 ||
echo 'command failed: json <<< '\''{"type":"array","args":[{"plain":"foo"},{"plain":null}]}'\'''

echo '$ json <<< '\''{"type":"array","args":[{"plain":"foo"},{"plain":false}]}'\'''
json <<< '{"type":"array","args":[{"plain":"foo"},{"plain":false}]}' 2>&1 ||
echo 'command failed: json <<< '\''{"type":"array","args":[{"plain":"foo"},{"plain":false}]}'\'''

echo '$ json <<< '\''{"type":"array","args":[{"plain":"foo"},{"plain":true}]}'\'''
json <<< '{"type":"array","args":[{"plain":"foo"},{"plain":true}]}' 2>&1 ||
echo 'command failed: json <<< '\''{"type":"array","args":[{"plain":"foo"},{"plain":true}]}'\'''

echo '$ json <<< '\''{"type":"array","args":[{"plain":"foo"},{"plain":123}]}'\'''
json <<< '{"type":"array","args":[{"plain":"foo"},{"plain":123}]}' 2>&1 ||
echo 'command failed: json <<< '\''{"type":"array","args":[{"plain":"foo"},{"plain":123}]}'\'''

echo '$ json <<< '\''{"type":"array","args":[{"plain":"foo"},{"plain":"foo"}]}'\'''
json <<< '{"type":"array","args":[{"plain":"foo"},{"plain":"foo"}]}' 2>&1 ||
echo 'command failed: json <<< '\''{"type":"array","args":[{"plain":"foo"},{"plain":"foo"}]}'\'''
)

