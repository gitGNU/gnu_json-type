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

.PHONY: default clean allclean all depend

default: all

ifndef JSON_TYPE_HOME
$(error JSON_TYPE_HOME not defined)
endif

MODULE := $(basename $(word $(words ${MAKEFILE_LIST}),${MAKEFILE_LIST}))

GCC := gcc
GCC_STD := gnu99
CFLAGS := -Wall \
          -Wextra \
          -std=${GCC_STD} \
          -I${JSON_TYPE_HOME}/lib \
          -fPIC -fvisibility=hidden -g \
          -DMODULE=${subst -,_,${MODULE}} \
          -DMODULE_DEF=\"${MODULE}.def\"

DEF  := ${MODULE}.def
SRCS := ${MODULE}.c
BIN  := ${MODULE}.so
DEP  := .depend-${MODULE}

# dependency rules

ifeq (${DEP}, $(wildcard ${DEP}))
include ${DEP}
endif

${MODULE}.c: ${MODULE}.def
${BIN}: ${SRCS}

# building rules

${BIN}:
	${GCC} ${CFLAGS} -shared $^ -o $@

# main targets

depend:
	${GCC} ${CFLAGS} -c ${SRCS} -MM > ${DEP}

all: ${BIN}

clean:
	rm -f *.o

allclean: clean
	rm -f *~ ${BIN}


