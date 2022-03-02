#!/bin/bash

runDir="./runs"
testFolder="parse-only"
testScriptRun() {
  ../parse.php < "$testSource" > "$runOutput"
}

. run_tests_framework.sh