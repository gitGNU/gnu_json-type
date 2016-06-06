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

PROGRAM := json
LIBRARY := json.so

LIB_HOME := ../lib

GCC := gcc
GCC_STD := gnu99
CFLAGS := -Wall -Wextra \
          -std=${GCC_STD}  -g -I.. -I. \
          -DPROGRAM=${PROGRAM} \
          -DLIBRARY=${LIBRARY}
LDFLAGS := -Wl,-L=${LIB_HOME} \
           -Wl,-rpath=. \
           -Wl,-rpath-link=${LIB_HOME} \
           -l:${LIBRARY} \
           -fPIC

SRCS := $(wildcard *.c)
OBJS := $(patsubst %.c, %.o, ${SRCS})
BIN  := ${PROGRAM}

# dependency rules

ifeq (.depend, $(wildcard .depend))
include .depend
endif

${BIN}: ${OBJS}

# building rules

%.o: %.c
	${GCC} ${CFLAGS} -c $< -o $@ 

${BIN}:
	${GCC} ${LDFLAGS} $^ -o $@

# main targets

depend:
	${GCC} ${CFLAGS} -c ${SRCS} -MM > .depend

all: ${BIN}

clean:
	rm -f *.o

allclean: clean
	rm -f *~ ${BIN}


