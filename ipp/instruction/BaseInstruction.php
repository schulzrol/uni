<?php

use JetBrains\PhpStorm\Pure;

include("BaseOperation.php");

abstract class BaseOperation {
    protected int $arity;
    protected array $types;
    private string $opcode;
    private int $order;

    /**
     * @param array $types
     */
    public function __construct(string $opcode, int $order, BaseExpression ...$types) {
        $this->arity = count($types);
        $this->types = $types;
        $this->opcode = $opcode;
        $this->order = $order;
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
    public function getTypes(): array {
        return $this->types;
    }

    /**
     * @param BaseExpression ...$suppliedParams
     * @return bool
     */
    function checkParams(BaseExpression ...$suppliedParams): bool {
        if (count($suppliedParams) != $this->getArity()) return false;
        // same number of params as arity
        for($i=0; $i< $this->getArity(); $i++)
            if (!$this->getTypes()[$i]->isTypeOf($suppliedParams[$i])) return false;

        return true;
    }

    /**
     * @return int
     */
    public function getOrder(): int
    {
        return $this->order;
    }

    /**
     * @return string
     */
    public function getOpcode(): string
    {
        return $this->opcode;
    }

    public function toXML(): string{
        $xml = sprintf("<instruction order=\"%i\" opcode=\"%s\">\n", $this->getOrder(), $this->getOpcode());

    }
}

class NullaryOperation extends BaseOperation {
    #[Pure]
    public function __construct(string $opcode, int $order){
        parent::__construct($opcode, $order);
    }
}

class UnaryOperation extends BaseOperation {
    /**
     * @param BaseExpression $p1 first parameter type
     */
    #[Pure]
    public function __construct(string $opcode, int $order, BaseExpression $p1){
        parent::__construct($opcode, $order, $p1);
    }
}

class BinaryOperation extends BaseOperation {
    #[Pure]
    public function __construct(string $opcode, int $order, BaseExpression $p1, BaseExpression $p2){
        parent::__construct($opcode, $order, $p1, $p2);
    }
}

class TernaryOperation extends BaseOperation {
    #[Pure]
    public function __construct(string $opcode, int $order, BaseExpression $p1, BaseExpression $p2, BaseExpression $p3){
        parent::__construct($opcode, $order, $p1, $p2, $p3);
    }
}
