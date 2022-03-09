<?php

include_once("XML.php");
include_once("Comparable.php");

use JetBrains\PhpStorm\Pure;

interface TypeCheckable {
    public function isTypeOf(string $supplied): bool;
}

//todo pridat interface na XML?
//todo expression factory
abstract class BaseExpression implements TypeCheckable, XMLPrintable, Comparable {
    protected string $pattern;
    protected string $value;
    protected string $label;
    protected string $precedence;

    function __construct(string $label, string $value, string $pattern, int $precedence = 0) {
        $this->label = $label;
        $this->pattern = $pattern;
        # todo add value type checking
        $this->setValue($value);
        $this->precedence = $precedence;
    }

    /**
     * @return string
     */
    public function getPattern(): string {
        return $this->pattern;
    }

    public function isTypeOf(string $supplied): bool {
        return preg_match($this->getPattern(), $supplied);
    }

    /**
     * @return string
     */
    public function getLabel(): string {
        return $this->label;
    }

    public function getType(): string {
        return $this->getLabel();
    }

    /**
     * @return string
     */
    public function getValue(): string
    {
        return $this->value;
    }

    /**
     * @param string $value
     */
    public function setValue(string $value): void {
        if ($this->isTypeOf($value))
            $this->value = $value;
    }

    public function toXMLTemplate(): string {
        $template = "        <arg{ORDER} type=\"{TYPE_LABEL}\">{VALUE}</arg{ORDER}>\n";
        $data = [
            "{TYPE_LABEL}" => $this->getLabel(),
            "{VALUE}" => $this->getValue()
        ];

        return str_replace(array_keys($data), array_values($data), $template);
    }

    #[Pure]
    public function __toString(): string {
        return $this->getValue();
    }

    public function getPrecedence(): int {
        return $this->precedence;
    }

    public function compareTo(Comparable $other): int {
        if ($this->getPrecedence() == $other->getPrecedence()) {
            return 0;
        }
        return ($this->getPrecedence() < $other->getPrecedence()) ? -1 : 1;
    }

}

class NilExpression extends BaseExpression {
    public function __construct(string $value='') {
        parent::__construct("nil", "nil", "/^nil@nil$/i");
    }
}

class IntExpression extends BaseExpression {

    public function __construct(string $value="") {
        $formats = [
            "hexadecimal" => "0[xX][0-9a-fA-F]+(_[0-9a-fA-F]+)*",
            "decimal" => "(([1-9][0-9]*(_[0-9]+)*)|0)",
            "octal" => "0[oO]?[0-7]+(_[0-7]+)*"
        ];
        $template = "/^int@((decimal$)?(hexadecimal$)?(octal$)?)$/i";
        $pattern = str_replace(array_keys($formats), array_values($formats), $template);
        #todo: refactor pattern
        $pattern="/^int@(0[xX][0-9a-fA-F]+(_[0-9a-fA-F]+)*$)?((([1-9][0-9]*(_[0-9]+)*)|0)$)?(0[oO]?[0-7]+(_[0-7]+)*$)?/i";
        parent::__construct("int", $value, $pattern);
    }

    /**
     * @param string $value
     */
    public function setValue(string $value): void {
        if ($this->isTypeOf($value))
            $this->value = explode('@', $value, 2)[1];
    }
}

class StringExpression extends BaseExpression {
    public function __construct(string $value="") {
        // TODO: check whether pattern is correct
        parent::__construct("string", $value, "/^string@[^\s]*$/iu");
    }

    /**
     * @param string $value
     */
    public function setValue(string $value): void {
        if ($this->isTypeOf($value))
            $this->value = htmlspecialchars(explode('@', $value, 2)[1], ENT_QUOTES, "UTF-8");
    }
}

class VarExpression extends BaseExpression {
    public function __construct(string $value='') {
        // TODO: check whether pattern is correct
        parent::__construct("var", $value, "/^(TF|GF|LF)@\w+$/i");
    }
}

class BoolExpression extends BaseExpression {
    public function __construct(string $value='') {
        // TODO: check whether pattern is correct
        parent::__construct("bool", $value, "/^bool@(true|false)$/i");
    }

    public function setValue(string $value): void {
        if ($this->isTypeOf($value))
            $this->value = explode('@', $value, 2)[1];
    }
}

class LabelExpression extends BaseExpression {
    public function __construct(string $value='') {
        // TODO: check whether pattern is correct
        parent::__construct("label", $value, "/^\w+\$/iu", 1);
    }
}

class TypeExpression extends BaseExpression {
    public function __construct(string $value='') {
        // TODO: check whether pattern is correct
        parent::__construct("type", $value, "/^(int|string|bool)$/i");
    }

}

//TODO rest of expression types