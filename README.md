# Zeta

A small, dynamically typed scripting language with a hand-written **lexer**, **Pratt parser** and **tree-walking interpreter** in modern C++ (C++20). No parser generators, no third-party libraries: about 1,400 lines of standard C++.

```zeta
fn make_counter() {
  let count = 0;
  fn counter() {
    count = count + 1;
    return count;
  }
  return counter;
}

let a = make_counter();
print a();   // 1
print a();   // 2
```

## Features

- **Values:** numbers (64-bit floating point), strings, booleans and `nil`
- **Variables** with `let`, assignment, and block scope with shadowing
- **Control flow:** `if` / `else`, `while`, and `for`
- **Functions:** first-class, recursive, and **closures** that capture the scope they were defined in
- **Operators:** `+ - * /`, comparisons `< <= > >= == !=`, unary `-` and `!`, and string concatenation with `+`
- **Runtime errors** with line numbers (type errors, undefined variables, division by zero, wrong argument count)
- **Two ways to run:** pass a `.zt` file, or start an interactive **REPL**

## Quick start

### Visual Studio (Windows)
1. Open `Zeta.slnx` in Visual Studio 2022 or newer.
2. Build with **Ctrl+Shift+B** and run with **F5**. The debugger runs `examples/square.zt`. Change it under *Project Properties → Debugging → Command Arguments*. Leave it empty to start the REPL.

The project builds with AddressSanitizer on. When running `Zeta.exe` outside Visual Studio, use the *Developer Command Prompt* so the ASan runtime DLL is found.

### g++ / clang (Linux, macOS)
```sh
g++ -std=c++20 -O2 -o zeta main.cpp lexer.cpp token.cpp parser.cpp ast.cpp stmt.cpp
```

### Usage
```sh
zeta examples/fib.zt    # run a file
zeta                    # start the REPL
```

```
> let x = 2;
> print x * 21;
42
> fn sq(n) { return n * n; }
> print sq(x + 1);
9
```

## Language tour

```zeta
// Comments start with //

let name = "Zeta";              // numbers, strings, true/false, nil
print "hello " + name;          // hello Zeta

let total = 0;
for (let i = 1; i <= 5; i = i + 1) {
  total = total + i;
}
print total;                    // 15

fn fib(n) {
  if (n < 2) return n;
  return fib(n - 1) + fib(n - 2);
}
print fib(10);                  // 55

let x = "outer";
{
  let x = "inner";              // shadows the outer x inside this block
  print x;                      // inner
}
print x;                        // outer
```

Conditions must be real booleans: `if (5)` is a runtime error rather than "truthy".

More programs are in [`examples/`](examples): Fibonacci, closures, FizzBuzz, and an intentional runtime error.

## How it works

```
source text ──► Lexer ──► tokens ──► Parser ──► AST ──► Interpreter ──► output
```

| Stage | Files | What it does |
|---|---|---|
| Tokens | `token.h/.cpp` | Token type, lexeme and line number |
| Lexer | `lexer.h/.cpp` | Turns source text into tokens; skips whitespace and comments; recognises keywords |
| Parser | `parser.h/.cpp` | Pratt parser for expressions plus recursive descent for statements; builds the AST |
| Expressions | `ast.h/.cpp` | Expression nodes (`BinaryExpr`, `CallExpr`, …), each with `evaluate()` |
| Statements | `stmt.h/.cpp` | Statement nodes (`IfStmt`, `WhileStmt`, `FnStmt`, …), each with `execute()` |
| Runtime | `value.h`, `environment.h` | Runtime values and the chain of variable scopes |
| Entry point | `main.cpp` | File mode and REPL |

### Design notes

- **Zero-copy lexing.** Each token's lexeme is a `std::string_view` into the source buffer, so no text is copied. The trade-off is that the source must outlive its tokens, which is why the REPL keeps every line's lexer alive.
- **Pratt parsing.** Each binary operator has a *binding power* (`==` 1, comparisons 2, `+ -` 3, `* /` 4). `parse_expression(min_power)` keeps consuming operators only while they bind tighter than `min_power`. This handles precedence and left-associativity without one grammar function per precedence level. Calls are handled in the same loop as a postfix `(`.
- **The AST evaluates itself.** `Expr` and `Stmt` are abstract base classes with virtual `evaluate()` / `execute()`. Children are owned with `std::unique_ptr`.
- **Values are a `std::variant`** of `nil`, number, bool, string and function, inspected with `std::holds_alternative` / `std::get`.
- **Scopes are a chain of environments.** Each block or call creates an `Environment` whose `parent` points outward. Lookup and assignment walk the chain.
- **Closures.** A function value stores its declaration plus a `std::shared_ptr` to the environment it was defined in. Calls run in a new environment whose parent is that captured scope, so a returned function still sees its variables. Environments are shared with `std::enable_shared_from_this`.
- **`for` is syntactic sugar.** The parser rewrites `for (init; cond; step) body` into `{ init; while (cond) { body; step; } }`, so the interpreter has no `for` code at all.
- **`return` unwinds with an exception.** `return` throws a small `ReturnValue` object that the enclosing call catches, so it can exit from any depth of nested blocks and loops.
- **Two kinds of errors.** Parse errors set a flag so all of them are reported and nothing runs. Runtime errors throw `std::runtime_error` with the line number.

### Known limitations
- A function stored in the scope it captures forms a `shared_ptr` reference cycle, so that memory is only reclaimed at exit. A tracing garbage collector would fix this.
- No arrays, `%`, `and` / `or` or `break` yet (see roadmap).
- A lexer error (e.g. an unterminated string) is reported but parsing continues, which can add follow-on errors.

## Roadmap
- [ ] `%` operator
- [ ] `and` / `or` with short-circuit evaluation
- [ ] `break`
- [ ] Native functions such as `clock()`
- [ ] Arrays with indexing
- [ ] v2: compile to bytecode and run on a stack-based VM with a garbage collector

## About

Built by Zaedyn Thomas-Kennedy as a learning project. The overall architecture follows the classic tree-walking interpreter design popularised by Robert Nystrom's *Crafting Interpreters*.
