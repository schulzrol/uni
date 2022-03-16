<?php

use JetBrains\PhpStorm\Pure;

include_once("XML.php");
include_once("Context.php");
include_once("expression/BaseExpression.php");

/**
 * Abstraction of single line of code, encapsulates both opcode with arguments.
 *
 * Some instructions are capable of modifying global context (jumps, statistics, etc.).
 */
abstract class BaseInstruction implements XMLPrintable, ModifiesContext {
    protected int $arity;
    protected array $paramTypes;
    private string $opcode;
    private array $paramValues;

    /**
     * @param string $opcode operation code
     * @param array $paramTypes array of arrays of BaseExpressions for parameter type compliance
     */
    public function __construct(string $opcode, array ...$paramTypes) {
        $this->arity = count($paramTypes);
        $this->paramTypes = $paramTypes;
        $this->opcode = $opcode;
        $this->paramValues = [];
    }

    /**
     * @return int arity getter
     */
    public function getArity(): int {
        return $this->arity;
    }

    /**
     * @return array getter of compliant expressions for each instruction argument position
     */
    public function getParamTypes(): array {
        return $this->paramTypes;
    }

    /**
     * @return array values getter for each instruction parameter
     */
    public function getParamValues(): array
    {
        return $this->paramValues;
    }

    /**
     * Checks whether supplied expressions comply with instructions arity and argument position expression types
     *
     * @param BaseExpression ...$suppliedParams supplied expressions to check against instruction expression patterns
     * @return bool true if complies with instruction positional expression types, false otherwise
     */
    #[Pure]
    function checkParamsExpressionTypes(BaseExpression ...$suppliedParams): bool {
        // same number of params as arity
        if (count($suppliedParams) != $this->getArity()) return false;

        for ($i = 0; $i < $this->getArity(); $i++) {
            $suppliedInSupported = false;
            foreach ($this->getParamTypes()[$i] as $supportedParamType) {
                if (is_a($suppliedParams[$i], get_class($supportedParamType)))
                    $suppliedInSupported = true;
            }
            if (!$suppliedInSupported)
                return false;
        }


        return true;
    }

    /**
     * @return string opcode getter
     */
    public function getOpcode(): string
    {
        return $this->opcode;
    }

    /**
     * @return string returns a string as XML template of instruction together with parameters,
     *                expects key {ORDER} signifying the order in which this instruction appears in source code
     */
    public function toXMLTemplate(): string{
        $template = "    <instruction order=\"{ORDER}\" opcode=\"{OPCODE}\">\n" .
                    "{ARGS}" .
                    "    </instruction>\n";

        $data = [
            "{OPCODE}" => $this->getOpcode(),
            "{ARGS}" => $this->paramsAsXML(),
            ];
        return str_replace(array_keys($data), array_values($data), $template);
    }

    /**
     * @return string returns all instruction parameters as XML with supplied ORDER key
     */
    private function paramsAsXML(): string {
        $xml = "";
        foreach ($this->getParamValues() as $i => $param){
            $xml .= str_replace(["{ORDER}"], [strval($i + 1)], $param->toXMLTemplate());
        }
        return $xml;
    }

    /**
     * @param array $suppliedParams paramValues setter, checks expression values before assigning
     */
    public function setParamValues(BaseExpression ...$suppliedParams): void {
        if ($this->checkParamsExpressionTypes(...$suppliedParams))
            $this->paramValues=$suppliedParams;
    }

    /**
     * @return callable defaults to no context modification
     */
    public function getContextModification(): callable {
        return function(){};
    }

}

/**
 * Base class for instructions expecting 0 arguments
 */
abstract class NullaryInstruction extends BaseInstruction {
    #[Pure]
    public function __construct(string $opcode){
        parent::__construct($opcode);
    }
}

/**
 * Base class for instructions expecting 1 argument
 */
abstract class UnaryInstruction extends BaseInstruction {
    /**
     * @param string $opcode
     * @param array $p1 first parameter type
     */
    #[Pure]
    public function __construct(string $opcode, array $p1){
        parent::__construct($opcode, $p1);
    }
}

/**
 * Base class for instructions expecting 2 arguments
 */
abstract class BinaryInstruction extends BaseInstruction {
    #[Pure]
    public function __construct(string $opcode, array $p1, array $p2){
        parent::__construct($opcode, $p1, $p2);
    }
}


/**
 * Base class for instructions expecting 3 arguments
 */
abstract class TernaryInstruction extends BaseInstruction {
    #[Pure]
    public function __construct(string $opcode, array $p1, array $p2, array $p3){
        parent::__construct($opcode, $p1, $p2, $p3);
    }
}
