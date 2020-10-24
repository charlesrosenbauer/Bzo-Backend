# Bzo-Backend

---

This is intended to be a backend for the bootstrapping version of the Bzo compiler.

The basic idea behind Bzo is that the compiler, and in particular the backend, should be as transparent and as hackable as possible. At the end of the day, the programmer's job is to create an executable file, not a bunch of text files, and often times low-level programmers are left unable to build the kinds of code they really want.

Writing low-level code should not be about contorting the language to trick the compiler into generating code that it normally wouldn't. That may improve performance, but it rarely gets the ideal machine code, and often comes at the expense of bug-ridden, unmaintainable, and unreadable code. Furthermore, low-level programming is most necessary in basic infrastructure. This is the last place where we want bugs and inefficiencies. Ideally, we want to get around both of these problems.

The goal of Bzo is to provide a different approach; the language is fundamentally a set of tools for building executables. Direct layout of instructions, etc. can be controlled by the programmer, or by automated systems that the programmer builds. If the programmer doesn't care about this, they can always use the default tools that the language provides, but the option to do things on their own is there for when it is needed.
