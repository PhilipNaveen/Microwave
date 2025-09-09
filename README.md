# Microwave Programming Language

Microwave is an esoteric programming language I implemented in C++. Its syntax and semantics are intentionally unconventional, and have evolved beyond the original "microwave oven command" design. This document details the current language design and features.

---

## Introduction

Microwave is a result of experimental language design inspired by boredom and college coursework. The purpose is to demonstrate that a computer can be programmed using a minimalist, quirky language. The language is not intended for serious development.

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
