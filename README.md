# AJLang

AJLang is a small **teaching-style** scripting language: a C interpreter with a tokenizer, recursive-descent parser, and tree-walking evaluator. Scripts use the `.aj` extension.

## Why use it?

- **Learn how interpreters work** — the codebase is small enough to read end-to-end (`src/tokenizer.c`, `src/parser.c`, `src/evaluator.c`).
- **Glue and experiments** — quick linear scripts with `print`, integer input, control flow, and **`exec`** to reuse anything the shell can run.
- **Not a replacement for Python, Bash, or Lua** — tiny stdlib, no PyPI-style ecosystem. Use those when you need large programs or heavy libraries.

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
| **Import** | `import "path.aj";` — loads and runs another file in the **same** variable environment (see below) |

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
| `examples/import_demo.aj` | `import` of `lib/demo.aj` |
| `lib/demo.aj` | Tiny module for import demos |

## `import`

Syntax:

```text
import "relative/or/absolute/path.aj";
```

Semantics:

- The imported file is **tokenized, parsed, and evaluated** when the `import` statement runs.
- It shares the **same global environment** as the main script (`let` / assignments in the module are visible after the import).
- **Resolution order** for a non-absolute path:
  1. Relative to the directory of the file that contains the `import` (usually the current `.aj` file; for nested imports, the **importing** file’s directory).
  2. If not found: **`$AJLANG_HOME/lib/`** *logical path* if `AJLANG_HOME` is set, else **`$HOME/.ajlang/lib/`**.
- **Absolute paths** (`"/tmp/x.aj"`) are used as-is.
- **Circular imports** (A → B → A) are detected and rejected.

Other reuse options: copy/paste snippets, **`exec`**, and (when implemented) **`ajp install`** dropping files under `$(./ajp home)`.

## `ajp` (package manager stub)

```bash
./ajp help          # usage
./ajp home          # print/create library directory (default ~/.ajlang/lib)
```

## Limitations (honest list)

- No functions, classes, or arrays; no separate module scope (import runs in the global environment)
- No `==`, `&&`, `||` in the grammar (workarounds: range checks, nested `if`, or shell via `exec`)
- String operations are minimal (no string `+` in expressions)
- `read` reads integers only, not arbitrary text lines
- `exec` runs a real shell — use only trusted commands; captured output is capped (see `evaluator.c`)

## Repository layout

```
include/     headers
src/         tokenizer, parser, evaluator, main, io (read_file)
lib/         optional `.aj` modules for `import`
examples/    sample .aj programs
Makefile
ajp          package manager CLI (stub)
```

Pull requests that add **`==`**, **`ajp install`**, or **function** syntax are natural next steps for growing the project.
