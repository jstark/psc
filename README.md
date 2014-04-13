psc - A pascal interpreter and compiler 
========================================
The code is ported from the book "Writing Interpreters and Compilers: A software engineering approach" and is written
in C++ (11). 

Current version: 0.8

## Status
### Frontend
All pascal tokens are lexed correctly. The following constructs are parsed:

```BEGIN ... END``` compound statements

Assignment statements

Expressions

```IF ... THEN``` statements

```FOR ... DO``` statements

```WHILE ... DO``` statements

```REPEAT ... UNTIL``` statements

```CASE ... OF``` statements

No type checking is performed. Actually, variables are not declared at all. Variable declaration, type
checking, function definition and whole program parsing and interpretation is under construction. 

Example usage:
```
psc hello.pas
```
Passing the option *-x* performs a cross-reference check on all *identifiers* in source.
### Intermediate
Passing the option *-i* generates and prints an **abstract syntax tree** of the input source in xml format.
### Backend
All constructs that are parsed, are also interpreted. 
