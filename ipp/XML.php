<?php

/**
 * Interface defining a method for converting an object into an XML string.
 *
 * Some parts of the XML string may be left out for caller to fill out, the left out parts must be documented
 * in per class method docstring
 */
interface XMLPrintable {
    public function toXMLTemplate(): string;
}