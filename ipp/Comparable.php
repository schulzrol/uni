<?php

/**
 * Interface providing functionality to compare two class objects by their level of precedence
 */
interface Comparable {
    // -1, 0, 1 respectively when less than, equal or higher precedence of $this than $other
    public function compareTo(Comparable $other): int;
    public function getPrecedence(): int;
}

/**
 * Helper function for comparing two comparables, useful for usort
 *
 * @param Comparable $a Comparable object
 * @param Comparable $b Comparable object
 * @return int -1, 0, 1 respectively when less than, equal or higher precedence of $a in comparison with $b
 */
function compareComparables(Comparable $a, Comparable $b): int {
    return $a->compareTo($b);
}