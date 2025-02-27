#!/bin/bash
TEST_FOUR_FILES=$(find tests -name "*.4.correct" | sed -e 's/\.correct$//')
# TEST_FOUR_FILES=$(find tests -name "testFileBackedShare.4.cpp" | sed -e 's/\.cpp$//')
TOTAL_TEST_FOUR_FILES=$(find tests -name "*.4.correct" | wc -l)
TEST_FIVE_FILES=$(find tests -name "*.5.correct" | sed -e 's/\.correct$//')
# TEST_FIVE_FILES=$(find tests -name "testSharedFile.5.cpp" | sed -e 's/\.cpp$//')
TOTAL_TEST_FIVE_FILES=$(find tests -name "*.5.correct" | wc -l)
FOUR_SUCCESS=0
FIVE_SUCCESS=0
FAILED_TESTS=()
for test in $TEST_FOUR_FILES; do
	./pager > tests/${test##*/}.out & lastpid=$!; make ${test##*/}; kill $lastpid;
	sed -e 's/([0-9]*)\|([0-9]*, [0-9]*)/()/' tests/${test##*/}.out > tests/${test##*/}.out.tmp;
	sed -i '/^destroying/d' tests/${test##*/}.out.tmp 
	sed -e 's/([0-9]*)\|([0-9]*, [0-9]*)/()/' tests/${test##*/}.correct > tests/${test##*/}.correct.tmp;
	DIFF="$(diff tests/${test##*/}.out.tmp tests/${test##*/}.correct.tmp)"
	if [[ $DIFF == "" ]]; then
		((FOUR_SUCCESS++));
		# rm tests/${test##*/}.out tests/${test##*/}.out.tmp tests/${test##*/}.correct.tmp;
	else
		FAILED_TESTS+=($test)
	fi
	rm tests/${test##*/}.out tests/${test##*/}.out.tmp tests/${test##*/}.correct.tmp;
	make restore;
	sleep 1
done

for test in $TEST_FIVE_FILES; do
	./pager -m 5 > tests/${test##*/}.out & lastpid=$!; make ${test##*/}; kill $lastpid;
	sed -e 's/([0-9]*)\|([0-9]*, [0-9]*)/()/' tests/${test##*/}.out > tests/${test##*/}.out.tmp;
	sed -i '/^destroying/d' tests/${test##*/}.out.tmp 
	sed -e 's/([0-9]*)\|([0-9]*, [0-9]*)/()/' tests/${test##*/}.correct > tests/${test##*/}.correct.tmp;
	DIFF="$(diff tests/${test##*/}.out.tmp tests/${test##*/}.correct.tmp)"
	if [[ $DIFF == "" ]]; then
		((FIVE_SUCCESS++));
		# rm tests/${test##*/}.out tests/${test##*/}.out.tmp tests/${test##*/}.correct.tmp;
	else
		FAILED_TESTS+=($test)
	fi
	rm tests/${test##*/}.out tests/${test##*/}.out.tmp tests/${test##*/}.correct.tmp;
	make restore;
	sleep 1
done
echo "$FOUR_SUCCESS / $TOTAL_TEST_FOUR_FILES 4 page cases passed"
# echo "$FOUR_SUCCESS / 1 4 page cases passed"
echo "$FIVE_SUCCESS / $TOTAL_TEST_FIVE_FILES 5 page cases passed"
# echo "$FIVE_SUCCESS / 1 5 page cases passed"
echo "Test cases that failed or have unpredictable garbage mappings: ${FAILED_TESTS[@]}"