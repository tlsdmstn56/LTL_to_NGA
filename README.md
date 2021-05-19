# LTL to Büchi conversion

************************

## IO description

### Input format for LTL-formula

Read LTL-formula in Polish notation from standard input.

__Propositional operators__
- fi ::= 't' # constant true
- fi ::= 'p'[0-9]+ # proposition
- fi ::= '!' fi	# negation
- fi ::= '^' fi fi # conjunction
- fi ::= [ \t\n\r\v\f] fi # white space is ignored
- fi ::= fi [ \t\n\r\v\f] # white space is ignored

__Temporal operators__
- fi ::= 'X' fi # next
- fi ::= 'U' fi fi # until

Pass formula into the executable file (_ltl_converter_) via standard input.
`ltl_converter test.txt` - as an example.


### Output automaton

Output will be saved into the (hardcoded) __dot.gv__ file in a dot-language format.
See more [here](https://graphviz.org/doc/info/lang.html) to understand main principles.

Also, will be printed detailed explanation of each a_i state into the standard output.

### Graphical representation
You may need __graphviz__ package to visualise dot language.
The default example of usage is a __run.sh__ file (Run it in a bash terminal: `bash run.sh`).

_Note:_ run main program before to generate appropriate dot file.

************************

## Theory

### Bibliography

1. _Javier Esparza_: __Automata theory. An algorithmic approach.__ September 24, 2019:
   Constructing the NGA for an LTL formula: p. 309
2. _Кривий С.Л._ __Скінченні автомати: теорія, алгоритми, складність.__ "Букрек" 2020:
   Верифікація систем: p. 277-294

The second source is used as the main one.
Esparza proposes another algorithm for constructing atomic pluralities (Automaton states generating).

************************

### Example of usage

LTL-formula for the input: `^ p0 ! U p0 p1` is equal to the `f = p0 ^ ¬(p0 U p1)` common mathematical representation.

Standard output:
```bash
# ltl_converter
a0 = {p0; p1; U p0 p1; ! ^ p0 ! U p0 p1}
a1 = {p0; ! p1; U p0 p1; ! ^ p0 ! U p0 p1}
a2 = {p0; ! p1; ! U p0 p1; ^ p0 ! U p0 p1}
a3 = {! p0; p1; U p0 p1; ! ^ p0 ! U p0 p1}
a4 = {! p0; ! p1; ! U p0 p1; ! ^ p0 ! U p0 p1}
```

Graphical representation:
```bash
# bash run.sh
```

![alt-Image](example.png "An example of automaton.png output")


### Requirements

- C++ 20 standard support
- CMake >= 3.19 version
- [_graphviz_](https://graphviz.org/) open source graph visualization software

************************

[**GitHub Repository**](https://github.com/maxs-im/LTL_to_NGA)
