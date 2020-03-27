# Bzo-Backend

---

This is intended to be a backend for the bootstrapping version of the Bzo compiler.

The goal (the backend is not done yet) is to read Bzo IR, apply a few simple optimizations, and then write x86_64 machine code to an ELF file.


Layout:

Parser -> IR Analysis -> IR Xforms -> IR to x86_64 -> Instruction Scheduling -> ELF + Code Generation


# Progress:

**Parser:**
DONE:
* Basic lisp parser
TODO:
* Convert lisp function AST to bytecode SSA IR
* Convert lisp typedef  AST to type model
* Generate string table

**IR Analysis:**
TODO:
* Basic validity checks
* Copy analysis
* Lifetime analysis

**IR Xforms:**
TODO:
* Higher-order-functions to loops
* Smart Allocation
* Tail-call elimination
* Other optimizations?

**IR to x86_64:**
DONE:
* SSA appending
* x86_64 opcode table parsing
TODO:
* IR to x86_64 translation

**Instruction Scheduling:**
TODO:
* Tableau scheduler

**ELF + Code Generation:**
DONE:
* ELF Header generation
* Basic x86_64 machine codegen
TODO:
* Finish machine codegen
* Full ELF generation
* Linking support