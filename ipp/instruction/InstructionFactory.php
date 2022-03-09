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

class InstructionFactory {
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
}

class RETURNInstruction extends NullaryInstruction {
    public function __construct() {
        parent::__construct("RETURN");
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
}

class JUMP extends UnaryInstruction {
    public function __construct(){ parent::__construct("JUMP", [LabelExpression]); }
}

class JUMPIFEQ extends TernaryInstruction{
    public function __construct(){
        parent::__construct("JUMPIFEQ", [LabelExpression], SymbExpression, SymbExpression);
    }
}

class JUMPIFNEQ extends TernaryInstruction{
    public function __construct(){
        parent::__construct("JUMPIFNEQ", [LabelExpression], SymbExpression, SymbExpression);
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