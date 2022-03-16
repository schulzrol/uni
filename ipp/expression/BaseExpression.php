<?php

include_once("XML.php");
include_once("Comparable.php");

use JetBrains\PhpStorm\Pure;

/**
 * Defines interface for associating expression value with expression type
 */
interface TypeCheckable {
    public function isTypeOf(string $supplied): bool;
}

abstract class BaseExpression implements TypeCheckable, XMLPrintable, Comparable {
    protected string $pattern;
    protected string $value;
    protected string $label;
    protected string $precedence;

    /**
     * @param string $label name of this expression
     * @param string $value value associated with this expression (must comply with expression pattern)
     * @param string $pattern pattern specifying what values can be associated with this expression type
     * @param int $precedence order of precedence when competing patterns might comply with the same expression value
     */
    function __construct(string $label, string $value, string $pattern, int $precedence = 0) {
        $this->label = $label;
        $this->pattern = $pattern;
        $this->setValue($value);
        $this->precedence = $precedence;
    }

    /**
     * @return string returns expression pattern
     */
    public function getPattern(): string {
        return $this->pattern;
    }

    /**
     * @param string $supplied value
     * @return bool true if value complies with expression's pattern, else false
     */
    public function isTypeOf(string $supplied): bool {
        return preg_match($this->getPattern(), $supplied);
    }

    /**
     * @return string label getter
     */
    public function getLabel(): string {
        return $this->label;
    }

    /**
     * @return string value associated with expression
     */
    public function getValue(): string
    {
        return $this->value;
    }

    /**
     * @param string $value value setter, extracts value from pattern matched string
     */
    public function setValue(string $value): void {
        if ($this->isTypeOf($value))
            $this->value = $value;
    }

    /**
     * @return string returns an XML template string of expression, expects key {ORDER} signifying position of
     *                expression in instruction arguments
     */
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

    /**
     * @return int precedence getter
     */
    public function getPrecedence(): int {
        return $this->precedence;
    }

    /**
     * @param Comparable $other other expression
     * @return int -1, 0, 1 respectively when less than, equal or higher precedence in comparison with $other
     */
    public function compareTo(Comparable $other): int {
        if ($this->getPrecedence() == $other->getPrecedence()) {
            return 0;
        }
        return ($this->getPrecedence() < $other->getPrecedence()) ? -1 : 1;
    }

}

// Expressions

/**
 *  NilExpression used for amtching a single value nil@nil
 */
class NilExpression extends BaseExpression {
    public function __construct(string $value='') {
        parent::__construct("nil", "nil", "/^nil@nil$/i");
    }
}

/**
 *  IntExpression used for associating [hexa]decimal or octal integer numbers
 */
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

    public function setValue(string $value): void {
        if ($this->isTypeOf($value))
            $this->value = explode('@', $value, 2)[1];
    }
}

class StringExpression extends BaseExpression {
    public function __construct(string $value="") {
        parent::__construct("string", $value, "/^string@[^\s]*$/iu");
    }

    /**
     * @param string $value value setter, cleans up string from any XML problematic characters
     */
    public function setValue(string $value): void {
        if ($this->isTypeOf($value))
            $this->value = htmlspecialchars(explode('@', $value, 2)[1], ENT_QUOTES, "UTF-8");
    }
}

class VarExpression extends BaseExpression {
    public function __construct(string $value='') {
        parent::__construct("var", $value, "/^(TF|GF|LF)@[\w\-\$\&\%\*\!\?]+$/i");
    }
}

class BoolExpression extends BaseExpression {
    public function __construct(string $value='') {
        parent::__construct("bool", $value, "/^bool@(true|false)$/i");
    }

    public function setValue(string $value): void {
        if ($this->isTypeOf($value))
            $this->value = explode('@', $value, 2)[1];
    }
}

class LabelExpression extends BaseExpression {
    public function __construct(string $value='') {
        parent::__construct("label", $value, "/^\w+\$/iu", 1);
    }
}

class TypeExpression extends BaseExpression {
    public function __construct(string $value='') {
        parent::__construct("type", $value, "/^(int|string|bool)$/i");
    }

}