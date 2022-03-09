<?php

interface Comparable {
    public function compareTo(Comparable $other): int;
    public function getPrecedence(): int;
}

function compareComparables(Comparable $a, Comparable $b): int {
    return $a->compareTo($b);
}