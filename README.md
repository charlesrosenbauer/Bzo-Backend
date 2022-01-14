**Note: this project is being deprecated**


# Bzo-Backend

---

~~This is intended to be a backend for the bootstrapping version of the Bzo compiler.~~

After careful consideration regarding changes to the language syntax (the original syntax had the appearance of something simple while being horribly complex in subtle ways), this is going to be the full bootstrapping compiler.

The basic idea behind Bzo is that the compiler, and in particular the backend, should be as transparent and as hackable as possible. At the end of the day, the programmer's job is to create an executable file, not a bunch of text files, and often times low-level programmers are left unable to build the kinds of code they really want.

Writing low-level code should not be about contorting the language to trick the compiler into generating code that it normally wouldn't. That may improve performance, but it rarely gets the ideal machine code, and often comes at the expense of bug-ridden, unmaintainable, and unreadable code. Furthermore, low-level programming is most necessary in basic infrastructure. This is the last place where we want bugs and inefficiencies. Ideally, we want to get around both of these problems.

The goal of Bzo is to provide a different approach; the language is fundamentally a set of tools for building executables. Direct layout of instructions, etc. can be controlled by the programmer, or by automated systems that the programmer builds. If the programmer doesn't care about this, they can always use the default tools that the language provides, but the option to do things on their own is there for when it is needed.

---

**Syntax**

Note: This syntax is not final.


Hello World:
```
[#import: io]

main :: () -> () {
	[printf: "Hello World!"]
}
```

Average Function:
```
avg :: [xs : []I64] -> [I64] {
	sum := [fold: xs, 0, [x,y]{x+y}]
	sum / xs.len
}
```

However, note that handing it an empty array will produce a divide-by-zero error! We can resolve this using a constraint:
```
avg :: [xs: []I64] -> [I64] {
	|: [#greq: xs.len > 0]
	sum := [fold: xs, 0, [x,y]{x+y}]
	sum / xs.len
}
```

The ```|:``` denotes a constraint, and the ```#greq``` is the constraint function. ```#greq``` is short for global requirement; any function that calls this function will be required to attest that ```xs.len``` is greater than zero. Unlike similar approaches taken in other languages, dependent typing for example, proving this does not usually require significant programmer effort, and instead mostly relies on compiler analysis (much of which is already performed by compilers for optimization anyway). Proofs, as well as many of the data structures required to construct and operate on such proofs, can be cached in order to amortize these costs.

```
List :: [
	vals : []Obj
	locs : []I32
	|: [#req: vals.len = locs.len]
]
```

This is a type called ```List```. It is a struct containing two arrays, ```vals``` and ```locs```. The constraint requires that both arrays be of equal size. If a ```List``` is ever constructed or modified in a way such that these two arrays end up with an unequal size, the code will not compile.




