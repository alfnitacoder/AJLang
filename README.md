# AJLang

AJLang is a small **teaching-style** scripting language: a C interpreter with a tokenizer, recursive-descent parser, and tree-walking evaluator. Scripts use the `.aj` extension.

## Why use it?

- **Learn how interpreters work** — the codebase is small enough to read end-to-end (`src/tokenizer.c`, `src/parser.c`, `src/evaluator.c`).
- **Glue and experiments** — quick linear scripts with `print`, integer input, control flow, and **`exec`** to reuse anything the shell can run.
- **Not a replacement for Python, Bash, or Lua** — it has no package ecosystem, no standard library to speak of, and no `import` yet. Use those when you need real libraries or large programs.

## Build and run

```bash
make
./ajlang path/to/script.aj
./ajlang path/to/script.aj arg1 arg2   # see “Command-line arguments”
```

The binary is `ajlang` in the project root (ignored by git after build). Requires a C compiler (`gcc` or `clang`).

## Language basics

| Feature | Notes |
|--------|--------|
| **Variables** | `let x = 1;` or `let msg = "hello";` — later `x = x + 1;` |
| **Types** | Integers and strings in variables; arithmetic is integer-only (`+ - * /`, comparisons `> <`) |
| **Output** | `print expr;` (adds a newline) |
| **Input** | `read n;` — one integer per line |
| **Control flow** | `if condition { ... }`, `while condition { ... }` — conditions are expressions (non-zero int is true) |
| **Comments** | `//` to end of line |
| **Shell** | `exec "command", outvar;` — runs `command` via the shell, captures **stdout** into string variable `outvar` (stderr unless redirected with e.g. `2>&1`) |

### Command-line arguments

Arguments after the script path are exposed as:

- **`argc`** — integer count of those arguments  
- **`arg0`, `arg1`, …** — each token as a string  

Example:

```bash
./ajlang examples/check.aj -i eth0 -l list.txt
```

See `examples/check.aj` for usage.

### Example scripts

| File | Idea |
|------|------|
| `examples/example.aj` | Variables, `if`, `while` |
| `examples/string.aj` | Strings, `read`, arithmetic |
| `examples/calculator.aj` | Menu loop |
| `examples/shell.aj` | `exec` and OS output |
| `examples/check.aj` | `argc` / `arg0`… |

## Libraries and “import”

**There is no `import` statement yet** — the language cannot load another `.aj` file from inside a script.

Ways to reuse logic today:

1. **Copy and paste** shared `.aj` snippets, or **concatenate** files before running (e.g. with the shell) if you control the build step.
2. **`exec "…", result;`** — call installed CLI tools, scripts in other languages, or shell functions; read their stdout into a string and branch on it with `print` / `if` as needed.
3. **`ajp`** — the **AJLang package manager** name is reserved for a future workflow (`./ajp help`). Planned layout: libraries under `$HOME/.ajlang/lib` (or `$AJLANG_HOME/lib`). **`install` / `list` are not implemented yet**; once they are, the interpreter would still need an **`import`** (or `load`) feature to actually pull code into a program.

If you add `import` to the interpreter, a typical design is: resolve a module path under `$(ajp home)`, read the file, parse it into an AST, and run it in the current environment (or a nested scope). Until then, treat **the shell + `exec`** as your “foreign library” interface.

## `ajp` (package manager stub)

```bash
./ajp help          # usage
./ajp home          # print/create library directory (default ~/.ajlang/lib)
```

## Limitations (honest list)

- No functions, classes, arrays, or `import`
- No `==`, `&&`, `||` in the grammar (workarounds: range checks, nested `if`, or shell via `exec`)
- String operations are minimal (no string `+` in expressions)
- `read` reads integers only, not arbitrary text lines
- `exec` runs a real shell — use only trusted commands; captured output is capped (see `evaluator.c`)

## Repository layout

```
include/     headers
src/         tokenizer, parser, evaluator, main
examples/    sample .aj programs
Makefile
ajp          package manager CLI (stub)
```

Pull requests that add **`import`**, **`==`**, or **`ajp install`** are natural next steps for growing the project.
