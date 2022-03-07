<?php
class ExitCode {
    const BAD_OR_MISSING_PARAM = 10;
    const ERR_INPUT_FILE = 11;
    const ERR_OUTPUT_FILE = 12;
    // 20-69 reserved
    // parse.php
    const PARSE_BAD_OR_MISSING_HEADER = 21;
    const PARSE_BAD_OR_UNKNOWN_OPCODE = 22;
    const PARSE_ERR_LEXICAL_OR_SYNTACTIC = 23;
    const PARSE_ERR_LEXICAL = self::PARSE_ERR_LEXICAL_OR_SYNTACTIC;
    const PARSE_ERR_SYNTACTIC = self::PARSE_ERR_LEXICAL_OR_SYNTACTIC;

    // test.php
    const TEST_ERR_FOLDER = 41;
    const TEST_ERR_FILE = self::TEST_ERR_FOLDER

    const ERR_INTERNAL = 99;
}
?>