#!/usr/bin/php8.1
<?php
include("ExitCode.php");
include_once("Context.php");
include_once("instruction/InstructionFactory.php");
include_once("expression/ExpressionFactory.php");

ini_set('display_errors', 'stderr');
const DEBUG = false;

function writeHelp() {
    echo("Skript typu filtr (parse.php v jazyce PHP 8.1) načte ze standardního vstupu zdrojový kód v IPP-
code22, zkontroluje lexikální a syntaktickou správnost kódu a vypíše na standardní
výstup XML reprezentaci programu.

Tento skript pracuje s těmito parametry:
  --help vypíše na standardní výstup nápovědu skriptu (nenačítá žádný vstup) a vrací návratovou hodnotu 0. Tento parametr nelze kombinovat s žádným dalším parametrem, jinak skript skončí s chybou 10.
");
}

/**
 * Handles compatible commandline arguments and errors on incompatibilities
 */
function handleParams() {
    $options = getopt("h",["help", "stats:", "loc", "comments", "labels", "jumps", "fwjumps", "backjumps", "badjumps"]);
    if (array_key_exists("help", $options) or array_key_exists("h", $options)){
        writeHelp();
        if (count($options) > 1)
            exit(ExitCode::BAD_OR_MISSING_PARAM);
        else
            exit(ExitCode::OK);
    }
}

/**
 * Helper function for easily leveraging ?? operator even with non-null objects
 *
 * @param $p mixed object to evaluate as true/false
 * @return mixed|null returns $p if $p evaluates to boolean true, otherwise returns null
 */
function falseAsNull($p) {
    return $p ?: null;
}

/**
 * Returns the correct input stream for production or debug environment based on global DEBUG variable
 * @return false|null|resource for global DEBUG === true returns static test file, else STDIN
 */
function getInputStream() {
    static $testfile = null;
    if ($testfile === null and DEBUG) {
        $testfile = fopen("./tests/both/spec_example.src", "r");
    }
    return DEBUG ? $testfile : STDIN;
}

handleParams();

$output = "";
$comment_line_regex = "/^\s*#/iu";
$empty_line_regex = "/^\s*$/";
$order = 1;
$context = Context::getInstance();

# skip prepending comments and empty lines
while($line = fgets(getInputStream())){
    if (preg_match($comment_line_regex, $line)) {
        $context->incrementCommentsCount();
        continue;
    }
    if (preg_match($empty_line_regex, $line))
        continue;

    break;
}
# check if after skipped lines a header present
if (!preg_match("/^\.IPPCODE22.*/i", $line)){
    exit(ExitCode::PARSE_BAD_OR_MISSING_HEADER);
}

// skip lines starting with # - comments
if (str_contains($line, "#"))
    $context->incrementCommentsCount();

$instructionFactory = new InstructionFactory();
$expressionFactory = new ExpressionFactory();
$instructions = [];

// parse loop
while($line = fgets(getInputStream())){
    # skip comment lines
    if (preg_match($comment_line_regex, $line)){
        $context->incrementCommentsCount();
        continue;
    }

    if (preg_match($empty_line_regex, $line))
        continue;

    if (str_contains($line, "#"))
        $context->incrementCommentsCount();

    # remove comments and newline
    $cleanLine = falseAsNull(strstr($line, "#", true)) ?? $line;
    # remove excess whitespace before/after opcode and parameters in line
    $cleanLine = trim($cleanLine);

    # split line on whitespace
    $splitLine = preg_split('/\s+/', $cleanLine);

    # next potential instruction
    $opcode = $splitLine[0];
    # parameters to potential instruction
    $params = array_slice($splitLine, 1);

    $nextInstruction = $instructionFactory->forOPCODE($opcode);
    if (is_null($nextInstruction)) {
        exit(ExitCode::PARSE_BAD_OR_UNKNOWN_OPCODE);
    }

    $paramExpressions = $expressionFactory->expressionsForParams(...$params);
    if (!in_array(null, $paramExpressions, true) and $nextInstruction->checkParamsExpressionTypes(...$paramExpressions)) {
        $nextInstruction->setParamValues(...$paramExpressions);
        $contextModification = $nextInstruction->getContextModification()();
        $instructions[] = $nextInstruction;
        $context->incrementInstructionsCount();
    }
    else {
        exit(ExitCode::PARSE_ERR_LEXICAL_OR_SYNTACTIC);
    }
}

// create the resulting XML
print("<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n");
print("<program language=\"IPPcode22\">\n");
foreach ($instructions as $order => $ins){
    print(str_replace(["{ORDER}"], [$order + 1], $ins->toXMLTemplate()));
}
print("</program>\n");
exit(ExitCode::OK);
?>