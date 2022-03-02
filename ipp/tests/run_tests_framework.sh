# Test Framework

mkdir -p "$runDir"
nSuccess=0
nAll=0

for testCase in "$testFolder"/*.src; do
  ((nAll=nAll+1))
  diffCorrect=false
  returnCorrect=false

  testCaseName="${testCase##*/}"
  testSource="$testCase"
  testOutput="${testCase%.*}.out"
  testReturnCode=$(<"${testCase%.*}.rc")
  runOutput="$runDir/${testCaseName%.*}_run.out"

  testScriptRun
  runReturnCode="$?"

  echo "$nAll: ${testCase%.*} ->"
  echo "=============================================="
  echo "comparing outputs: $testOutput $runOutput"
  diff -y "$testOutput" "$runOutput"
  diffReturnCode="$?"

  if [[ "$diffReturnCode" == "0" ]]; then
    echo "CORRECT"
    diffCorrect=true
  else
    echo "WRONG DIFF!"
  fi

  echo "=============================================="
  echo "comparing return codes:"
  if [[ "$testReturnCode" == "$runReturnCode" ]]; then
    echo "CORRECT test:$testReturnCode == run:$runReturnCode"
    returnCorrect=true
  else
    echo "WRONG RETURN CODE! test:$testReturnCode != run:$runReturnCode"
  fi

  if [ "$diffCorrect" = true ] && [ "$returnCorrect" = true ]; then
    ((nAll=nAll+1))
  fi

  echo
done

echo "$nSuccess/$nAll"
[ "$nSuccess" = "$nAll" ]
