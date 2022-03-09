#!/usr/bin/php8.1
<?php
include("ExitCode.php");
include_once("Context.php");
include_once("instruction/InstructionFactory.php");
include_once("expression/ExpressionFactory.php");

ini_set('display_errors', 'stderr');

function writeHelp() {
    echo("Skript typu filtr (parse.php v jazyce PHP 8.1) načte ze standardního vstupu zdrojový kód v IPP-
code22, zkontroluje lexikální a syntaktickou správnost kódu a vypíše na standardní
výstup XML reprezentaci programu.

Tento skript pracuje s těmito parametry:
  --help vypíše na standardní výstup nápovědu skriptu (nenačítá žádný vstup) a vrací návratovou hodnotu 0. Tento parametr nelze kombinovat s žádným dalším parametrem, jinak skript skončí s chybou 10.
");
}

function handleParams() {
    $options = getopt("h", ["help"]);
    if (array_key_exists("help", $options) or array_key_exists("h", $options)){
        writeHelp();
        exit(ExitCode::OK);
    }
    $options = array_diff($options, ["help" => false, "h" => false]);

    if (count($options)){
        exit(ExitCode::BAD_OR_MISSING_PARAM);
    }
}

function falseAsNull($p) {
    return $p ?: null;
}

handleParams();

$output = "";
$comment_line_regex = "/^\s*#/iu";
$empty_line_regex = "/^\s*$/";
$order = 1;
$context = Context::getInstance();

# check header
# - ignore prepending comments
while($line = fgets(STDIN)){
    if (preg_match($comment_line_regex, $line))
        continue;
    if (preg_match($empty_line_regex, $line))
        continue;

    break;
}
# - check if header
if (!preg_match("/^\.IPPCODE22.*/i", $line)){
    exit(ExitCode::PARSE_BAD_OR_MISSING_HEADER);
}

if (str_contains($line, "#"))
    $context->incrementCommentsCount();

$instructionFactory = new InstructionFactory();
$expressionFactory = new ExpressionFactory();
$instructions = [];

while($line = fgets(STDIN)){
    # skip comment lines
    if (preg_match($comment_line_regex, $line)){
        $context->incrementCommentsCount();
        continue;
    }

    if (preg_match($empty_line_regex, $line))
        continue;

    if (str_contains($line, "#"))
        $context->incrementCommentsCount();

    # Remove comments and newline
    $cleanLine = falseAsNull(strstr($line, "#", true)) ?? $line;
    $cleanLine = trim($cleanLine);

    $splitLine = preg_split('/\s+/', $cleanLine);

    # next potential instruction
    $opcode = $splitLine[0];
    $params = array_slice($splitLine, 1);
    $nextInstruction = $instructionFactory->forOPCODE($opcode);
    if (is_null($nextInstruction)) {
        exit(ExitCode::PARSE_BAD_OR_UNKNOWN_OPCODE);
    }
    $contextModification = $nextInstruction->getContextModification();

    $paramExpressions = $expressionFactory->expressionsForParams(...$params);
    if (!in_array(null, $paramExpressions, true) and $nextInstruction->checkParamsExpressionTypes(...$paramExpressions)) {
        $nextInstruction->setParamValues(...$paramExpressions);
        $instructions[] = $nextInstruction;
        $context->incrementInstructionsCount();
    }
    else {
        exit(ExitCode::PARSE_ERR_LEXICAL_OR_SYNTACTIC);
    }
}

print("<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n");
print("<program language=\"IPPcode22\">\n");
foreach ($instructions as $order => $ins){
    print(str_replace(["{ORDER}"], [$order + 1], $ins->toXMLTemplate()));
}
print("</program>\n");
exit(ExitCode::OK);
?>