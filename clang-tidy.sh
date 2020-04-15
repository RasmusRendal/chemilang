#!/usr/bin/env bash

FILES=$(find src -name '*.h')
EXIT_STATUS=0

for index in ${!FILES[*]}
do
	file=${FILES[index]}
	clang-tidy -warnings-as-errors='*' $file || EXIT_STATUS=$?
done

SOURCEFILES=$(find src -name '*.cpp')
for index in ${!SOURCEFILES[*]}
do
	file=${SOURCEFILES[index]}
	clang-tidy -warnings-as-errors='*' $file || EXIT_STATUS=$?
done


exit $EXIT_STATUS
