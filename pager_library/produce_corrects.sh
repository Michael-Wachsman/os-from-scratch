#!/bin/bash
TEST_FOUR_FILES=$(find tests -name "*.4.cpp" | sed -e 's/\.cpp$//')
TEST_FIVE_FILES=$(find tests -name "*.5.cpp" | sed -e 's/\.cpp$//')
for test in $TEST_FOUR_FILES; do
	./pager > tests/${test##*/}.correct & lastpid=$!; make ${test##*/}; kill $lastpid
	sleep 1
done

for test in $TEST_FIVE_FILES; do
	./pager -m 5 > tests/${test##*/}.correct & lastpid=$!; make ${test##*/}; kill $lastpid
	sleep 1
done