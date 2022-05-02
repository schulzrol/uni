#!/usr/bin/php8.1
<?php
include_once("ExitCode.php");

ini_set('display_errors', 'stderr');

function fillTemplate($template, $data) {
    $result = $template;
    foreach ($data as $key => $value) {
        $result = str_replace("{{" . $key . "}}", $value, $result);
    }
    return $result;
}

function addTestToDom($dom, $data, $test_row_template) {
    $test_row = fillTemplate($test_row_template, $data);
    $dom = fillTemplate($dom, array("TEST_ROW" => $test_row . "\n" . "{{TEST_ROW}}"));
    return $dom;
}

function run_parse_test($htmlData, $src, $parseOutput, $parseScript, $rc, $expected_output) {
    $parseRc = 0;
    $outputs_are_same = false;

    $runScript = "php8.1 " . $parseScript . " < " . $src;
    exec($runScript, $parseOut, $parseRc);
    $parseOut = shell_exec($runScript);

    $outputFile = fopen($parseOutput, "w");
    fwrite($outputFile, $parseOut);
    fclose($outputFile);

    $htmlData['expected_code'] = $rc;
    $htmlData['parse_real_code'] = $parseRc;

    # run diff on output files
    exec("diff " . $parseOutput . " " . $expected_output, $diffOut, $diffRc);
    $outputs_are_same = ($diffRc == 0);

    $htmlData['parse_rc'] = $parse_rc;
    $htmlData['parse_result'] = ($parseRc == $rc) && ($outputs_are_same || $rc != 0) ? "OK" : "FAIL";

    return $htmlData;
}

function run_interpret_test($htmlData, $src, $intOutput, $interpretScript, $rc, $expected_output) {
    $intRc = 0;
    $outputs_are_same = false;
    $runScript = "python3.8 " . $interpretScript . " --source=" . $src;
    exec($runScript, $intOut, $intRc);
    $intOut = shell_exec($runScript);

    $outputFile = fopen($intOutput, "w");
    fwrite($outputFile, $intOut);
    fclose($outputFile);

    # run diff on output files
    exec("diff " . $intOutput . " " . $expected_output, $diffOut, $diffRc);
    $outputs_are_same = ($diffRc == 0);

    $htmlData['interpret_rc'] = $intRc;
    $htmlData['interpret_result'] = ($intRc == $rc) && ($outputs_are_same || $rc != 0) ? "OK" : "FAIL";
    return $htmlData;
}

$longOpts = array("help", "directory:", "recursive", "int-only", "parse-only","parse-script:", "int-script:", "jexampath:", "noclean");
$getOpts = getopt("", $longOpts);

# TODO: edit help
if (array_key_exists("help", $getOpts)) {    # Prints help
    if ($argc == 2 || $argv[1] == "--help") {
        echo "Usage:\n";
        echo "--parse-script PATH  Path to parser script\n";
        echo "--parse-only         Only run parse tests\n";
        echo "--int-only           Only run interpret tests\n";
        echo "--directory PATH     Dir with tests to run.\n";
        echo "--int-script PATH    Path to interpreter script\n";
        echo "--recursive          Recursive test dir traversal.\n";
        echo "--help               Prints help.\n";
        exit;
    }
    else exit(ExitCode::BAD_OR_MISSING_PARAM);
}

$dir = "./";
$recursiveTests = false;
$parseScript = "./parse.php";
$interpretScript = "./interpret.py";
$jexampath= "/pub/courses/ipp/jexamxml/";
$doClean=true;
$doBoth = true;
$parse_only = false;
$interpret_only = false;

if (array_key_exists("directory", $getOpts)) $dir = $getOpts["directory"];

if (array_key_exists("recursive", $getOpts)) $recursiveTests = true;

if (array_key_exists("noclean", $getOpts)) $doClean = false;

if (array_key_exists("parse-script", $getOpts)) $parseScript = $getOpts["parse-script"];

if (array_key_exists("int-script", $getOpts)) $interpretScript = $getOpts["int-script"];

if (array_key_exists("int-only", $getOpts)){
    $interpret_only = true;
    $doBoth = false;
}

if (array_key_exists("parse-only", $getOpts)){
    $parse_only = true;
    $doBoth = false;
}

if (array_key_exists("jexampath", $getOpts)) $jexampath = rtrim($getOpts["jexampath"], "/") . "/";

if (!file_exists($dir) ||
   (!file_exists($parseScript) && array_key_exists("parse-script", $getOpts)) ||
   (!file_exists($interpretScript) && array_key_exists("int-script", $getOpts)) ||
   (!file_exists($jexampath) && array_key_exists("jexampath", $getOpts)))
    exit(ExitCode::ERR_INPUT_FILE);

# get all tests based on recursive flag
if ($recursiveTests)
    exec("find " . $dir . " -regex '.*\.src$'", $testPaths);
else
    exec("find " . $dir . " -maxdepth 1 -regex '.*\.src$'", $testPaths);

# doesnt allow parse-only and int-only and int-script at the same time
if (array_key_exists("parse-only", $getOpts) && array_key_exists("int-only", $getOpts) && array_key_exists("int-script", $getOpts))
    exit(ExitCode::BAD_OR_MISSING_PARAM);

if (array_key_exists("parse-only", $getOpts) && array_key_exists("int-only", $getOpts) && array_key_exists("parse-script", $getOpts) && array_key_exists("jexampath", $getOpts))
    exit(ExitCode::BAD_OR_MISSING_PARAM);

#print("Directory: $dir\n");
#print("Recursive: $recursiveTests\n");
#print("Parse script: $parseScript\n");
#print("Interpret script: $interpretScript\n");
#print("Jexamxml path: $jexampath\n");
#print("Clean: $doClean\n");
#print("\n");

$intOutput= tempnam("/tmp", "");
$parseOutput = tempnam("/tmp", "");

$main_template = file_get_contents("resources/main_view_template.html");
$test_row_template = file_get_contents("resources/test_row_template.html");
$dom = $main_template;

$tests_passed = 0;
$tests_failed = 0;

foreach ($testPaths as $src) {
    $pathParts = explode('/', $src);
    $testName = explode('.', end($pathParts))[0];
    $testPath = "";

    foreach (array_slice($pathParts, 0, -1) as $dir) {
        $testPath = $testPath . $dir . '/';
    }

    $in = $testPath . $testName . ".in";
    $out = $testPath . $testName . ".out";
    $rcFile = $testPath . $testName . ".rc";

    # create missing files
    if (!file_exists($out)) {
        $file = fopen($out, "w");
        fclose($file);
    }

    if (!file_exists($rcFile)) {
        $rc = 0;
        $file = fopen($rcFile, "w");
        fwrite($file, "0");
        fclose($file);
    }
    else {
        $file = fopen($rcFile, "r");
        $rc = intval(file_get_contents($rcFile));
        fclose($file);
    }

    if (!file_exists($in)) {
        $file = fopen($in, "w");
        fclose($file);
    }


    $htmlData = array();
    $htmlData['name'] = $testName;
    $htmlData['location'] = $testPath;
    $htmlData['expected_rc'] = $rc;
    $htmlData['interpret_rc'] = "NOT RUN";
    $htmlData['parse_rc'] = "NOT RUN";

    $doParseTests = ($parse_only || $doBoth);
    $doInterpretTests = ($interpret_only || $doBoth);

    # run parse test
    if ($doParseTests) {
        $htmlData = run_parse_test($htmlData, $src, $parseOutput, $parseScript, $rc, $out);
    }

    # run interpret test
    if ($doInterpretTests){
        $htmlData = run_interpret_test($htmlData, $src, $intOutput, $interpretScript, $rc, $out);
    }

    if (($doParseTests && $htmlData['parse_result'] == "OK" || $interpret_only) &&
        ($doInterpretTests && $htmlData['interpret_result'] == "OK") || $parse_only) {
        $htmlData['result'] = "OK";
        $tests_passed++;
    }
    else {
        $tests_failed++;
        $htmlData['result'] = "FAIL";
    }

    $dom = addTestToDom($dom, $htmlData, $test_row_template);
}


print(fillTemplate($dom, array("TESTS_OK" => $tests_passed,
                                "TESTS_FAIL" => $tests_failed,
                                "TESTS_ALL" => $tests_passed + $tests_failed,
                                "TEST_ROW" => "",
                            )));
if ($doClean){
    unlink($parseOutput);
    unlink($intOutput);
}

exit(0);