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

.PHONY: default clean allclean all depend silent

default: all

ifndef SRCS
$(error SRCS is not defined)
endif

ifndef PROGRAM
$(error PROGRAM is not defined)
endif

GCC := gcc
GCC_STD := gnu99
CFLAGS := -Wall -Wextra -std=${GCC_STD} \
          -g -I. -I.. -fPIC -fvisibility=hidden \
          -DPROGRAM=${PROGRAM} \
          -DJSON_NO_LIB_MAIN \
          -DJSON_DEBUG

BIN  := ${PROGRAM}

ifeq (${32BIT},yes)
CFLAGS += -m32
LDFLAGS += -m32
endif

# dependency rules

ifeq (.depend-${BIN}, $(wildcard .depend-${BIN}))
include .depend-${BIN}
endif

${BIN}: ${SRCS}

# building rules

ifeq (${SILENT},yes)
${BIN}: silent
endif

${BIN}:
	${GCC} ${CFLAGS} ${SRCS} -o $@ 

# main targets

silent:

depend:
	${GCC} ${CFLAGS} -c ${SRCS} -MM| \
	sed -r 's/^[^ \t]+\.o:/${BIN}:/' > .depend-${BIN}

all: ${BIN}

clean:
	rm -f *.o

allclean: clean
	rm -f *~ ${BIN}


