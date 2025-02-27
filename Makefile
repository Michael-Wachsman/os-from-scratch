CC=g++ -g -Wall -std=c++20 -D_XOPEN_SOURCE -Wno-deprecated-declarations

UNAME := $(shell uname -s)
ifeq ($(UNAME),Darwin)
    CC+=-D_XOPEN_SOURCE
    LIBFSCLIENT=libfs_client_macos.o
    LIBFSSERVER=libfs_server_macos.o
    BOOST_THREAD=boost_thread-mt
else
    LIBFSCLIENT=libfs_client.o
    LIBFSSERVER=libfs_server.o
    BOOST_THREAD=boost_thread
endif

# List of source files for your file server
FS_SOURCES=fs_server.cpp networking.cpp parsing.cpp filesystem.cpp
FS_INC = networking.h parsing.h filesystem.h

# Generate the names of the file server's object files
FS_OBJS=${FS_SOURCES:.cpp=.o}

all: fs

# Compile the file server and tag this compilation
#
# Remember to set the CPLUS_INCLUDE_PATH, LIBRARY_PATH, and LD_LIBRARY_PATH
# environment variables to your Boost installation.
#
# Depending on how you installed Boost, you may need to change -lboost_thread
# to -lboost_thread-mt
fs: ${FS_OBJS} ${LIBFSSERVER}
	./autotag.sh push
	${CC} -o $@ $^ -l${BOOST_THREAD} -lboost_system -pthread -ldl

# Compile a client program
# test1: test1.cpp ${LIBFSCLIENT}
# 	${CC} -o $@ $^
# 	COULD MAKE MORE FINE GRAINED TO PARSING
parse_tests: 
	${CC} -o $@ parse_tests.cpp parsing.cpp  
	./$@ 
	rm $@

%: tests/%.cpp ${LIBFSCLIENT}
	${CC} -o $@ $^
	./$@ localhost 8888
	rm $@

%: internal_tests/%.cpp ${LIBFSCLIENT}
	${CC} -o $@ $^
	./$@ localhost 8888
	rm $@

# Generic rules for compiling a source file to an object file
%.o: %.cpp
	${CC} -c $<
%.o: %.cc
	${CC} -c $<

clean:
	rm -f ${FS_OBJS} fs

count:
	@echo "Found $(shell find tests -name test*.cpp | wc -l) / 20 tests"