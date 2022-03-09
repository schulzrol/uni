<?php

//todo Composite pro AST
// todo Template method pro xml

use JetBrains\PhpStorm\Pure;

include_once("XML.php");
include_once("Context.php");
include_once("expression/BaseExpression.php");

abstract class BaseInstruction implements XMLPrintable, ModifiesContext {
    protected int $arity;
    protected array $paramTypes;
    private string $opcode;
    private array $paramValues;

    /**
     * @param string $opcode
     * @param array $paramTypes
     */
    public function __construct(string $opcode, array ...$paramTypes) {
        $this->arity = count($paramTypes);
        $this->paramTypes = $paramTypes;
        $this->opcode = $opcode;
        $this->paramValues = [];
    }

    /**
     * @return int
     */
    public function getArity(): int {
        return $this->arity;
    }

    /**
     * @return array
     */
    public function getParamTypes(): array {
        return $this->paramTypes;
    }

    /**
     * @return array
     */
    public function getParamValues(): array
    {
        return $this->paramValues;
    }

    /**
     * @param BaseExpression ...$suppliedParams
     * @return bool
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
     * @return string
     */
    public function getOpcode(): string
    {
        return $this->opcode;
    }

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

    private function paramsAsXML(): string {
        $xml = "";
        foreach ($this->getParamValues() as $i => $param){
            $xml .= str_replace(["{ORDER}"], [strval($i + 1)], $param->toXMLTemplate());
        }
        return $xml;
    }

    /**
     * @param array $suppliedParams
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

abstract class NullaryInstruction extends BaseInstruction {
    #[Pure]
    public function __construct(string $opcode){
        parent::__construct($opcode);
    }
}

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

abstract class BinaryInstruction extends BaseInstruction {
    #[Pure]
    public function __construct(string $opcode, array $p1, array $p2){
        parent::__construct($opcode, $p1, $p2);
    }
}

abstract class TernaryInstruction extends BaseInstruction {
    #[Pure]
    public function __construct(string $opcode, array $p1, array $p2, array $p3){
        parent::__construct($opcode, $p1, $p2, $p3);
    }
}

// todo concrete instructions
