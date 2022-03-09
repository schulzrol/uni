<?php
# Copied in accordance with CC BY-SA 3.0 (https://creativecommons.org/licenses/by-sa/3.0/)
# https://stackoverflow.com/a/3126181/8354587

abstract class Singleton
{
    protected function __construct() {}

    final public static function getInstance()
    {
        static $instances = array();

        $calledClass = get_called_class();

        if (!isset($instances[$calledClass])) {
            $instances[$calledClass] = new $calledClass();
        }

        return $instances[$calledClass];
    }

    private function __clone() {}
}