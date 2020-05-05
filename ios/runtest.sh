#!/bin/sh

current=$PWD
test_case_dirs=$(find $current/hodnoceni/ -maxdepth 1 -type d | grep "_" | sort)
#echo $test_case_dirs

for test_case_dir in $test_case_dirs
do
    cd "$test_case_dir"
    rm -f ./stdout
    rm 
    cp $current/dirgraph .
    echo $test_case_dir : $(cat hodnoceni-auto | head -n 1)
    cat spusteni
    sh spusteni
    diff -u stdout.ref stdout 
    echo 
done

