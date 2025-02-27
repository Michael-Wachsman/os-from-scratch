CC=g++ -g -Wall -std=c++17 -D_XOPEN_SOURCE

UNAME := $(shell uname -s)
ifeq ($(UNAME),Darwin)
    CC+=-D_XOPEN_SOURCE
    LIBCPU=libcpu_macos.o
else
    LIBCPU=libcpu.o
endif

# List of source files for your thread library
THREAD_SOURCES= cpu.cpp thread.cpp mutex.cpp cv.cpp managed_uc.cpp

# Generate the names of the thread library's object files
THREAD_OBJS=${THREAD_SOURCES:.cpp=.o}

TEST_FILES = $(wildcard tests/*.cpp)
TESTS = $(TEST_FILES:tests/%.cpp=%)

all: libthread.o app

# Compile the thread library and tag this compilation
libthread.o: ${THREAD_OBJS}
	./autotag.sh push
	ld -r -o $@ ${THREAD_OBJS}

# Compile an application program
app: app.cpp libthread.o ${LIBCPU}
	${CC} -o $@ $^ -ldl -pthread

# Compile and run a test case based off its name minus .cpp
%: tests/%.cpp libthread.o ${LIBCPU}
	${CC} -o $@ $< libthread.o ${LIBCPU} -ldl -pthread # -fsanitize=address -fsanitize=undefined
	./$@ > tests/$@.out
#	Comment out the below line if there isn't a .correct this test
	diff tests/$@.out tests/$@.correct
	rm $@ tests/$@.out

# Compile and run a test case based off its name minus .cpp in internal_tests
%: internal_tests/%.cpp libthread.o ${LIBCPU}
	${CC} -o $@ $< libthread.o ${LIBCPU} -ldl -pthread # -fsanitize=address -fsanitize=undefined
	./$@
	rm $@

# Run all cases that are going to be submitted at once
doesitwork: libthread.o ${LIBCPU}
	$(foreach t, $(TESTS), $(shell make $(t)))

.PHONY: count
count:
	@echo "Found $(shell find tests -name test-* | wc -l) / 22 tests"

# Generic rules for compiling a source file to an object file
%.o: %.cpp
	${CC} -c $<
%.o: %.cc
	${CC} -c $<

clean:
	rm -f ${THREAD_OBJS} libthread.o app test-*
