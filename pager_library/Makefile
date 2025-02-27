CC=g++ -g -Wall -fno-builtin -std=c++20 -D_XOPEN_SOURCE

UNAME := $(shell uname -s)
ifeq ($(UNAME),Darwin)
    CC+=-D_XOPEN_SOURCE
    LIBVMAPP=libvm_app_macos.o
    LIBVMPAGER=libvm_pager_macos.o
else
    LIBVMAPP=libvm_app.o
    LIBVMPAGER=libvm_pager.o
endif

# List of source files for your pager
PAGER_SOURCES= vm_pager.cpp Page.cpp

# Generate the names of the pager's object files
PAGER_OBJS=${PAGER_SOURCES:.cpp=.o}

all: pager

# Compile the pager and tag this compilation
pager: ${PAGER_OBJS} ${LIBVMPAGER}
	./autotag.sh push
	${CC} -o $@ $^

# Compile an application program
# test1: test1.cpp ${LIBVMAPP}
# 	${CC} -o $@ $^ -ldl

# Compile and run test files
%: tests/%.cpp ${LIBVMAPP}
	${CC} -o $@ $^ # -fsanitize=address -fsanitize=undefined
	./$@
#	Comment out the below line if there isn't a .correct this test
#	diff tests/$@.out tests/$@.correct
	rm $@

# Generic rules for compiling a source file to an object file
%.o: %.cpp
	${CC} -c $<
%.o: %.cc
	${CC} -c $<

clean:
	rm -f ${PAGER_OBJS} pager

restore:
	git checkout d2b10d6 -- lampson83.txt data1.bin data2.bin data3.bin data4.bin

.PHONY: count
count:
	@echo "Found $(shell find tests -name test*.cpp | wc -l) / 30 tests"
