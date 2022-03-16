<?php

include_once("Singleton.php");

/**
 * Classes implementing this interface may modify context if one decides to modify it by calling modification function
 */
interface ModifiesContext {
    function getContextModification(): callable;
}

/**
 * Context encapsulates global data in a Singleton manner. Primarily useful for STATP bonus extension
 */
class Context extends Singleton{
    protected int $instructionsCount;
    protected int $commentsCount;
    protected int $fwJumps;
    protected int $bwJumps;
    protected array $labels;
    protected int $returnCount;

    protected function __construct() {
        $this->instructionsCount = 0;
        $this->commentsCount = 0;
        $this->labels = [];
        $this->fwJumps = 0;
        $this->bwJumps = 0;
        $this->returnCount = 0;
        parent::__construct();
    }

    /**
     * @param int $instructionsCount getter
     */
    public function setInstructionsCount(int $instructionsCount): int {
        $this->instructionsCount = $instructionsCount;
        return $instructionsCount;
    }

    /**
     * @param int $commentsCount getter
     */
    public function setCommentsCount(int $commentsCount): int {
        $this->commentsCount = $commentsCount;
        return $commentsCount;
    }

    /**
     * @return int getter
     */
    public function getInstructionsCount(): int {
        return $this->instructionsCount;
    }

    /**
     * @return int getter
     */
    public function getCommentsCount(): int
    {
        return $this->commentsCount;
    }

    /**
     * @return int increments instructions count and returns the new value
     */
    public function incrementInstructionsCount(): int {
        return $this->setInstructionsCount($this->getInstructionsCount() + 1);
    }

    /**
     * @return int increments comments count and returns the new value
     */
    public function incrementCommentsCount(): int {
        return $this->setCommentsCount($this->getCommentsCount() + 1);
    }

    /**
     * @return array gets defined labels
     */
    public function getLabels(): array {
        return $this->labels;
    }

    /**
     * @param string $label label
     * @return int returns label occurences: 0->called but undefined, ==1->defined, >1->defined and called
     */
    public function getLabelOccurence(string $label): int {
        if (array_key_exists($label, $this->getLabels()))
            return $this->getLabels()[$label];
        return 0;
    }

    /**
     * @return int increments label occurrence
     */
    public function incrementLabelOccurrence(string $label): void{
        if (array_key_exists($label, $this->getLabels()))
            $this->labels[$label] += 1;
    }

    /**
     * Adds new label to recognized labels
     *
     * @param string $label new label
     * @param int $occurrence occurences of new label
     */
    public function addLabel(string $label, int $occurrence = 1): void {
        if (array_key_exists($label, $this->getLabels()))
            $this->incrementLabelOccurrence($label);
        else
            $this->labels[$label] = $occurrence;
    }

    /**
     * @param string $label labels to check
     * @return bool true if label previously defined, else false
     */
    public function labelDefined(string $label): bool {
        return $this->getLabelOccurence($label) > 0;
    }

    /**
     * @return int fwjumps getter
     */
    public function getFwJumps(): int
    {
        return $this->fwJumps;
    }

    /**
     * increments fwjumps counter
     */
    public function incrementFwJumps(): void {
        $this->fwJumps+=1;
    }

    /**
     * @return int bwjumps getter
     */
    public function getBwJumps(): int
    {
        return $this->bwJumps;
    }

    /**
     * increments bwjumps counter
     */
    public function incrementBwJumps(): void {
        $this->bwJumps+=1;
    }

    /**
     * @return int count of RETURN instructions occured
     */
    public function getReturnCount(): int
    {
        return $this->returnCount;
    }

    /**
     * returnCount setter
     *
     * @param int $returnCount new return count
     */
    public function setReturnCount(int $returnCount): void
    {
        $this->returnCount = $returnCount;
    }

    /**
     * @return void increments return count by 1
     */
    public function incrementReturnCount(): void {
        $this->setReturnCount($this->getReturnCount() + 1);
    }


}