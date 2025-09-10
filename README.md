# Microwave Programming Language

Microwave is an estoeric programming language I created in 2024. The original microwave language was based on COW. While technically Turing-complete, it was utterly unusable. This repo serves as a total overhaul of the language, keeping the original Microwave theme, but improving the language syntax to operate similarly to Python, or C. The purpose of this language is to create a fun, minimalist, aand unserious language. While this language can be interpreted into C, and then compiled to an executable, it's not intended for use on genuine development projects.

---

## Introduction

This is the second variant of the Microwave programming language. Microwave 1-1.4 was inspired by ToyG (by Graham in CS2130 @ UVA). The relic of a language can be seen here: https://esolangs.org/wiki/Microwave

## Getting Started

- Microwave source files use the `.mw` extension.
- The compiler parses `.mw` files and generates C code.
- Supported platforms: Any with a C++14 compiler.
- To build: See the provided Makefile.

## Language Design

### Program Structure

- Every program consists of **functions** declared with the `mode` keyword.
- Function syntax:  
  ```
  mode <return_type> <function_name>(<params>) { ... }
  ```
  - Example return types: `int`, `float`, `string`, `bool`, `void`

### Types

- Supported types: `int`, `float`, `string`, `bool`, `void`
- Arrays are supported: `int[]`, `float[]`, etc.

### Statements

- **Variable Declaration:**  
  `int x = 5;`  
  `string msg = "hello";`
- **Assignment:**  
  `x = x + 1;`
- **Loops:**  
  - While loop:  
    `while (x < 10) { ... }`
  - For loop:  
    `for (int i = 0; i < n; i = i + 1) { ... }`
  - Timer loop:  
    `timer (5) { ... }` (executes the block 5 times)
- **Conditionals:**  
  - If statement:  
    `if (x == 0) { ... } else { ... }`
- **Return, break, continue:**  
  - `return <expr>;`
  - `break;`
  - `continue;`
- **Defrost statement:**  
  - `defrost <var>;` (sets a variable to zero)

### Expressions

- Numeric, string, boolean, variable references.
- Binary operators: `+`, `-`, `*`, `/`, `%`, `&&`, `||`, etc.
- Unary operators: `-`, `!`, prefix/postfix `++`, `--`
- Function calls and lambdas.

### Lambdas

- Anonymous functions can be declared using the `lambda` keyword:
  ```
  lambda (a, b) {
      return a + b;
  }
  ```

### Memory and Control

- Memory manipulation is handled via variable assignments and array operations, not via oven-themed commands.
- The previous "microwave sound" commands are deprecated in favor of standard C-like syntax.

### Input/Output

- Output is generally performed via the `beep` keyword mapped to `printf` in generated C code:
  - Example:  
    `beep "Hello World";` produces a print statement.
- Input and custom I/O may be provided via user-defined functions.

## Example (current style)

```c
mode int main() {
    int x = 0;
    while (x < 10) {
        beep x;
        x = x + 1;
    }
    return 0;
}
```

## Building & Usage

To build the compiler:
```
make
```
To compile a Microwave program:
```
./microwave source.mw output.c
```

## Summary of Changes

- The language now uses standard imperative syntax and supports functions, types, expressions, and control flow similar to C/C++.
- The quirky microwave-sound commands have been replaced with more conventional programming statements.
- The language supports lambdas, arrays, and timer-based loops.

---

For more details, see the source files in `src/` (`parser.h`, `parser.cpp`, `codegen.cpp`).
