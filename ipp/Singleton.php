<?php
# Copied in accordance with CC BY-SA 3.0 (https://creativecommons.org/licenses/by-sa/3.0/)
# https://stackoverflow.com/a/3126181/8354587

/**
 * Class providing singleton access pattern for children classes
 */
abstract class Singleton
{
    protected function __construct() {}

    final public static function getInstance()
    {
        static $instances = array();

        $calledClass = get_called_class();

        if (!isset($instances[$calledClass])) {  # if instance not already created for derived class...
            $instances[$calledClass] = new $calledClass(); # create & memoize one instance for derived class
        }

        return $instances[$calledClass];
    }

    private function __clone() {}
}