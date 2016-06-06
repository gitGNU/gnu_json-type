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

default:: all

GCC := gcc
GCC_STD := gnu99
CFLAGS := -Wall -Wextra -std=${GCC_STD} \
          -g -I. -I.. -fPIC -fvisibility=hidden \
          -DPROGRAM=json.so
LDFLAGS := -Wl,-L${LD_LIBRARY_PATH} \
           -Wl,--rpath-link=${LD_LIBRARY_PATH} \
           -Wl,--export-dynamic -pie
MKFLAGS := --no-print-directory SILENT=yes
LIBS := -ldl

EXCP := json-type-module.c test-trie.c
SRCS := $(filter-out ${EXCP}, $(wildcard *.c))
OBJS := $(patsubst %.c, %.o, ${SRCS})
BIN  := json.so

ifeq (${32BIT},yes)
CFLAGS += -m32
LDFLAGS += -m32
endif

JSON_DEBUG = $(shell \
	sed -nr '/^\s*\#\s*define\s+JSON_DEBUG\s*$$/{s//yes/;p;q}' json.h)

ifeq (${JSON_DEBUG},yes)
CFLAGS += -DJSON_DEBUG
endif

# dependency rules

ifeq (.depend, $(wildcard .depend))
include .depend
endif

ifeq (.depend-test-trie, $(wildcard .depend-test-trie))
include .depend-test-trie
endif

${BIN}: ${OBJS}

default all:: test-trie

# building rules

%.o: %.c
	${GCC} ${CFLAGS} -c $< -o $@ 

${BIN}: %.so: %.o
	${GCC} ${LDFLAGS} ${LIBS} $^ -o $@

# main targets

test-trie:
	@${MAKE} ${MKFLAGS} -f test-trie.mk

depend clean allclean::
	@${MAKE} ${MKFLAGS} -f test-trie.mk $@

depend::
	${GCC} ${CFLAGS} -c ${SRCS} -MM > .depend

all:: ${BIN}

clean::
	rm -f *.o

allclean:: clean
	rm -f *~ ${BIN}


