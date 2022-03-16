<?php

include_once("BaseInstruction.php");
include_once("expression/BaseExpression.php");

# Predefine to save memory
const NilExpression = new NilExpression();
const IntExpression = new IntExpression();
const StringExpression = new StringExpression();
const VarExpression = new VarExpression();
const BoolExpression = new BoolExpression();
const LabelExpression = new LabelExpression();
const TypeExpression = new TypeExpression();

const SymbExpression = [
    IntExpression,
    VarExpression,
    NilExpression,
    StringExpression,
    BoolExpression
];

/**
 * Instruction factory used to create new instructions from associated opcode.
 *
 * Instruction parameters are not the domain of this factory and should be handled by factory caller
 */
class InstructionFactory {
    /**
     *  Factory function associating OPCODE with instruction class for OPCODE handling class
     *
     * @param string $opcode opcode of instruction (case insensitive)
     * @return BaseInstruction|null if opcode associated instruction class exists, returns new instance of such class,
     *                              else null
     */
    public function forOPCODE(string $opcode): ?BaseInstruction {
        $capitalOpcode = strtoupper($opcode);
        $InstructionClassName = $capitalOpcode;
        $InstructionClassNameAlt = $InstructionClassName . "Instruction";
        if (class_exists($InstructionClassName)){
            return new $InstructionClassName();
        }

        if (class_exists($InstructionClassNameAlt)){
            return new $InstructionClassNameAlt();
        }

        return null;
    }

}

// specific instructions

class MOVE extends BinaryInstruction {
    public function __construct() {
        parent::__construct("MOVE", [VarExpression], SymbExpression);
    }
}

class CREATEFRAME extends NullaryInstruction {
    public function __construct() {
        parent::__construct("CREATEFRAME");
    }
}

class PUSHFRAME extends NullaryInstruction {
    public function __construct() {
        parent::__construct("PUSHFRAME");
    }
}

class POPFRAME extends NullaryInstruction {
    public function __construct() {
        parent::__construct("POPFRAME");
    }
}

class DEFVAR extends UnaryInstruction {
    public function __construct() {
        parent::__construct("DEFVAR", [VarExpression]);
    }
}

class CALL extends UnaryInstruction {
    public function __construct() {
        parent::__construct("CALL", [LabelExpression]);
    }

    /**
     * @return callable returns context modification anonymous function useful for STATP bonus extension
     */
    public function getContextModification(): callable {
        return function (){
            $ctx = Context::getInstance();
            $labelValue = $this->getParamValues()[0]->getValue();
            # check if label is defined -> therefore it is not a fwjump
            if ($ctx->labelDefined($labelValue)) {
                $ctx->incrementLabelOccurrence($labelValue);
                $ctx->incrementBwJumps();
            }
            else {
                $ctx->addLabel($labelValue, 0);
                $ctx->incrementFwJumps();
            }
        };
    }
}

class RETURNInstruction extends NullaryInstruction {
    public function __construct() {
        parent::__construct("RETURN");
    }

    public function getContextModification(): callable
    {
        return function () {
            $ctx = Context::getInstance();
            $ctx->incrementReturnCount();
        };
    }

}

class PUSHS extends UnaryInstruction {
    public function __construct() {
        parent::__construct("PUSHS", SymbExpression);
    }
}

class POPS extends UnaryInstruction {
    public function __construct() {
        parent::__construct("POPS", [VarExpression]);
    }
}

class ADD extends TernaryInstruction {
    public function __construct(){
        parent::__construct("ADD", [VarExpression], SymbExpression, SymbExpression);
    }
}

class SUB extends TernaryInstruction {
    public function __construct(){
        parent::__construct("SUB", [VarExpression], SymbExpression, SymbExpression);
    }
}

class MUL extends TernaryInstruction {
    public function __construct(){
        parent::__construct("MUL", [VarExpression], SymbExpression, SymbExpression);
    }
}

class IDIV extends TernaryInstruction {
    public function __construct(){
        parent::__construct("IDIV", [VarExpression], SymbExpression, SymbExpression);
    }
}

class LT extends TernaryInstruction {
    public function __construct(){
        parent::__construct("LT", [VarExpression], SymbExpression, SymbExpression);
    }
}

class GT extends TernaryInstruction {
    public function __construct(){
        parent::__construct("GT", [VarExpression], SymbExpression, SymbExpression);
    }
}

class EQ extends TernaryInstruction {
    public function __construct(){
        parent::__construct("EQ", [VarExpression], SymbExpression, SymbExpression);
    }
}


class ANDInstruction extends TernaryInstruction {
    public function __construct(){
        parent::__construct("AND", [VarExpression], SymbExpression, SymbExpression);
    }
}

class ORInstruction extends TernaryInstruction {
    public function __construct(){
        parent::__construct("OR", [VarExpression], SymbExpression, SymbExpression);
    }
}


class NOTInstruction extends TernaryInstruction {
    public function __construct(){
        parent::__construct("NOT", [VarExpression], SymbExpression, SymbExpression);
    }
}

class INT2CHAR extends BinaryInstruction {
    public function __construct(){
        parent::__construct("INT2CHAR", [VarExpression], SymbExpression);
    }
}

class STR2INT extends TernaryInstruction {
    public function __construct(){
        parent::__construct("STR2INT", [VarExpression], SymbExpression, SymbExpression);
    }
}

class READ extends BinaryInstruction {
    public function __construct(){
        parent::__construct("READ", [VarExpression], [TypeExpression]);
    }
}

class WRITE extends UnaryInstruction {
    public function __construct(){
        parent::__construct("WRITE", SymbExpression);
    }
}

# String manipulation instructions

class CONCAT extends TernaryInstruction {
    public function __construct(){
        parent::__construct("CONCAT", [VarExpression], SymbExpression, SymbExpression);
    }
}

class STRLEN extends BinaryInstruction {
    public function __construct(){
        parent::__construct("STRLEN", [VarExpression], SymbExpression);
    }
}

class GETCHAR extends TernaryInstruction {
    public function __construct(){
        parent::__construct("GETCHAR", [VarExpression], SymbExpression, SymbExpression);
    }
}

class SETCHAR extends TernaryInstruction {
    public function __construct(){
        parent::__construct("SETCHAR", [VarExpression], SymbExpression, SymbExpression);
    }
}

# Type manipulation instructions
class TYPE extends BinaryInstruction {
    public function __construct(){
        parent::__construct("TYPE", [VarExpression], SymbExpression);
    }
}

# Control manipulation instructions
class LABEL extends UnaryInstruction {
    public function __construct(){ parent::__construct("LABEL", [LabelExpression]); }

    /**
     * @return callable returns context modification anonymous function useful for STATP bonus extension
     */
    public function getContextModification(): callable {
        return function (){
            $ctx = Context::getInstance();
            $labelValue = $this->getParamValues()[0]->getValue();
            $ctx->addLabel($labelValue);
        };
    }
}

class JUMP extends UnaryInstruction {
    public function __construct(){ parent::__construct("JUMP", [LabelExpression]); }

    /**
     * @return callable returns context modification anonymous function useful for STATP bonus extension
     */
    public function getContextModification(): callable {
        return function (){
            $ctx = Context::getInstance();
            $labelValue = $this->getParamValues()[0]->getValue();
            # check if label is defined -> therefore it is not a fwjump
            if ($ctx->labelDefined($labelValue)) {
                $ctx->incrementLabelOccurrence($labelValue);
                $ctx->incrementBwJumps();
            }
            else {
                $ctx->addLabel($labelValue, 0);
                $ctx->incrementFwJumps();
            }
        };
    }
}

class JUMPIFEQ extends TernaryInstruction{
    public function __construct(){
        parent::__construct("JUMPIFEQ", [LabelExpression], SymbExpression, SymbExpression);
    }

    /**
     * @return callable returns context modification anonymous function useful for STATP bonus extension
     */
    public function getContextModification(): callable {
        return function (){
            $ctx = Context::getInstance();
            $labelValue = $this->getParamValues()[0]->getValue();
            # check if label is defined -> therefore it is not a fwjump
            if ($ctx->labelDefined($labelValue)) {
                $ctx->incrementLabelOccurrence($labelValue);
                $ctx->incrementBwJumps();
            }
            else {
                $ctx->addLabel($labelValue, 0);
                $ctx->incrementFwJumps();
            }
        };
    }
}

class JUMPIFNEQ extends TernaryInstruction{
    public function __construct(){
        parent::__construct("JUMPIFNEQ", [LabelExpression], SymbExpression, SymbExpression);
    }

    /**
     * @return callable returns context modification anonymous function useful for STATP bonus extension
     */
    public function getContextModification(): callable {
        return function (){
            $ctx = Context::getInstance();
            $labelValue = $this->getParamValues()[0]->getValue();
            # check if label is defined -> therefore it is not a fwjump
            if ($ctx->labelDefined($labelValue)) {
                $ctx->incrementLabelOccurrence($labelValue);
                $ctx->incrementBwJumps();
            }
            else {
                $ctx->addLabel($labelValue, 0);
                $ctx->incrementFwJumps();
            }
        };
    }
}

class EXITInstruction extends UnaryInstruction {
    public function __construct(){ parent::__construct("EXIT", SymbExpression); }
}

# Servicing instructions

class DPRINT extends UnaryInstruction {
    public function __construct(){ parent::__construct("DPRINT", SymbExpression); }
}

class BREAKInstruction extends NullaryInstruction{
    public function __construct(){ parent::__construct("BREAK"); }
}