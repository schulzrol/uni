<?php

include_once("Singleton.php");

interface ModifiesContext {
    function getContextModification(): callable;
}

class Context extends Singleton{
    protected int $instructionsCount;
    protected int $commentsCount;
    protected array $labels;

    protected function __construct() {
        $this->instructionsCount = 0;
        $this->commentsCount = 0;
        $this->labels = [];
        parent::__construct();
    }

    /**
     * @param int $instructionsCount
     */
    public function setInstructionsCount(int $instructionsCount): int {
        $this->instructionsCount = $instructionsCount;
        return $instructionsCount;
    }

    /**
     * @param int $commentsCount
     */
    public function setCommentsCount(int $commentsCount): int {
        $this->commentsCount = $commentsCount;
        return $commentsCount;
    }

    /**
     * @return int
     */
    public function getInstructionsCount(): int {
        return $this->instructionsCount;
    }

    /**
     * @return int
     */
    public function getCommentsCount(): int
    {
        return $this->commentsCount;
    }

    public function incrementInstructionsCount(): int {
        return $this->setInstructionsCount($this->getInstructionsCount() + 1);
    }

    public function incrementCommentsCount(): int {
        return $this->setCommentsCount($this->getCommentsCount() + 1);
    }

    /**
     * @return array
     */
    public function getLabels(): array {
        return $this->labels;
    }

    public function getLabelOccurence(string $label): int {
        if (array_key_exists($label, $this->getLabels()))
            return $this->getLabels()[$label];
        return 0;
    }

    public function incrementLabelOccurence(string $label): void{
        if (array_key_exists($label, $this->getLabels()))
            $this->getLabels()[$label]+=1;
    }

    public function addLabel(string $label): void {
        if (array_key_exists($label, $this->getLabels()))
            $this->incrementLabelOccurence($label);
        else
            $this->getLabels()[$label] = 1;
    }


}