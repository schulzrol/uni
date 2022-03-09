<?php


include_once("BaseExpression.php");

class ExpressionFactory {
    private array $children;

    public function __construct() {
        $this->children = [];
        foreach(get_declared_classes() as $class){
            if (in_array("TypeCheckable", class_implements($class)) and $class != "BaseExpression") {
                $this->children[$class] = new $class();
            }
        }

        usort($this->children, "compareComparables");

    }

    public function expressionForValue(string $value): ?BaseExpression {
        foreach ($this->children as $_ => $child)
            if ($child->isTypeOf($value)) {
                $classOfChild = get_class($child);
                return new $classOfChild($value);
            }
        return null;
    }

    public function expressionsForParams(string ...$params): array {
        $retExpressions = [];
        foreach ($params as $param)
            $retExpressions[] = $this->expressionForValue($param);

        return $retExpressions;
    }

    public function fromNameAsSingleton(string $name): ?BaseExpression {
        if (array_key_exists($name, $this->children))
            return $this->children[$name];
        return null;
    }

    public function fromNameNew(string $name): ?BaseExpression {
        if (array_key_exists($name, $this->children)) {
            $classOfChild = get_class($this->children[$name]);
            return new $classOfChild();
        }
        return null;
    }
}