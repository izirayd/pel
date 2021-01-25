# PEL - Parser engine of language

This library was created for parsing data that has a signature property, that is, is deterministic.

PEL is a programming language, api library and built-in emulator. PEL is based on his own hypotheses about the construction of the world, its data and interactions, which allows this language to work.

PEL is similar to first-order logic in but PEL was created independently of the current theory of first-order logic.

The main purpose of PEL is to parse programming languages.

Current version 0.2 is still a prototype.

Graph debug:
![d3693-clip-85kb](https://user-images.githubusercontent.com/20372478/105662215-37654100-5ee0-11eb-8db5-9473e9cdaf50.png)

## List of changes

### Version 0.x:
- [ ] Make a test system
- [ ] Reduce memory overhead for basic data structures
- [ ] Improve error handling and error tracking
- [ ] Make support for all kinds of strings
- [ ] Make AST interface

### Version 0.3:
- [ ] Made full support for loops and recursion with an exit system

### Version 0.2:
- [x] Added support for conditional operator
- [x] Rewritten PEL parser

### Version 0.1:

- [x] Added optimization algorithms for displacement in the movement of graph vertices.
- [x] Vertex grouping is now in superposition.
- [x] Added work with multiblocks.
- [x] Created a recursion operator that works completely without recursion.
- [x] All real recursions were removed in the core, they were replaced by another algorithm that excludes recursion using the stack.
- [x] The core of PEL was created.
- [x] Solved the problem of calculating true positioning at the top of the graph, even for instructions OR.
- [x] Group system created.
- [x] A system of blocks, types, values has been created.
- [x] A specification has been created for various areas of work with PEL.
- [x] Created a system for working with files and uploading in real time for developer mode.
- [x] Created command system.
- [x] Graph system created.

## Sample code of PEL

In this example, a variable is read, where its type is determined, and any name that does not contain symbols is selected.

```cpp
#module {tree};

group space   : { " " }  = execute, ignore;
group symbol  : { ";," } = execute, split;
group word    : { !space and !symbol } = execute, glue;


type int    : { "int" };
type string : { "string" };
type float  : { "float" };

type read_type : { int or string or float };
type var_name  : { word };
type var_decl  : { read_type, var_name, ";"  } = execute;
```
