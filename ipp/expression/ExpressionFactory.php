<?php


include_once("BaseExpression.php");

/**
 * Expression factory used for creating expression objects from expression class specific value patterns.
 */
class ExpressionFactory {
    private array $children;

    public function __construct() {
        $this->children = [];
        // get all type checkable classes that are not the base class -> all children expression classes
        foreach(get_declared_classes() as $class){
            if (in_array("TypeCheckable", class_implements($class)) and $class != "BaseExpression") {
                $this->children[$class] = new $class();
            }
        }

        /* assert type order from specific pattern -> general pattern to ensure correct class when comparing values
         * against patterns
         */
        usort($this->children, "compareComparables");

    }

    /**
     * Gets the correct expression instance for given value
     *
     * @param string $value expression value for which to find correct expression type
     * @return BaseExpression|null if any expression class pattern matches value, return new instance of such
     *                             expression class with value assigned, otherwise null
     */
    public function expressionForValue(string $value): ?BaseExpression {
        foreach ($this->children as $child)
            if ($child->isTypeOf($value)) {
                $classOfChild = get_class($child);
                return new $classOfChild($value);
            }
        return null;
    }

    /**
     * Like expressionForValue() but for variable number of values
     *
     * @param string ...$values variable number of expression values for which to find the correct expression type
     * @return array returns ?BaseExpression for each of input values in an array. BaseExpression if value for known type, null otherwise
     *
     */
    public function expressionsForParams(string ...$values): array {
        $retExpressions = [];
        foreach ($values as $param)
            $retExpressions[] = $this->expressionForValue($param);

        return $retExpressions;
    }

}