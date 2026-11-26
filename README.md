# Colon CLI

A single-header C argument parser with two modes:

- **Colon mode**: COLON-style `:flags` and `key:value` options.
- **Legacy mode**: POSIX-style `-f` and `--flag` flags/options.

> toggled at runtime with `:`.

Both modes map to the same underlying options and flags aka they are aliases, not separate namespaces.

## Showcase

```c
#define CCLI colc

#define CCLI_POS_ARGS(X, Y) \
    X(STRING,    input,  "input",  "Input file",  "Path to the source file to process") \
    X(STRING,    output, "output", "Output file", "Path to the generated output file") \
    Y(8, STRING, files,  "file",   "Extra files", "Additional input files to bundle")

#define CCLI_OPT_ARGS(X) \
    X(STRING, out_dir, ".",    ("o", "out", "dir"), "Output dir", "Directory for generated files") \
    X(STRING, format,  "json", ("f", "format"),     "Format",     "Output format: json, yaml, toml") \
    X(UINT,   depth,   4,      ("d", "depth"),      "Max depth",  "Maximum recursion depth") \
    X(UINT,   limit,   100,    ("l", "limit"),      "Limit",      "Maximum number of items to process")

#define CCLI_BOL_ARGS(X) \
    X(help,      FALSE, ("h", "help"),      "Help",      "Show this help message") \
    X(version,   FALSE, ("v", "version"),   "Version",   "Show version information") \
    X(force,     FALSE, ("F", "force"),     "Force",     "Overwrite existing output files") \
    X(recursive, TRUE,  ("r", "recursive"), "Recursive", "Process directories recursively") \
    X(verbose,   FALSE, ("V", "verbose"),   "Verbose",   "Print detailed processing info")

#include "coloncli.h"
```

**Help message output**:

```txt
colc - Colon CLI

Usage: colc [OPTIONS] :: [ARGS...]

[Options]
  o:out:dir:  STRING  Output dir  ["."]     Directory for generated files
  f:format:   STRING  Format      ["json"]  Output format: json, yaml, toml
  d:depth:    UINT    Max depth   [4]       Maximum recursion depth
  l:limit:    UINT    Limit       [100]     Maximum number of items to process

[Flags]
  :h:help       Help       [ ]  Show this help message
  :v:version    Version    [ ]  Show version information
  :F:force      Force      [ ]  Overwrite existing output files
  :r:recursive  Recursive  [*]  Process directories recursively
  :V:verbose    Verbose    [ ]  Print detailed processing info

[Positions]
  input   STRING     Input file   Path to the source file to process
  output  STRING     Output file  Path to the generated output file
  file    STRING[8]  Extra files  Additional input files to bundle
```

## Features

- **Single header**: just `#include "coloncli.h"`.
- **Zero dependencies**: pure C99, no standard library only stdarg.h, limits.h headers.
- **Colon mode**:
  - `key:value`, `key: value`
  - `k:value`, `k: value`
  - `:flag` (enable),
  - `:flag:` (disable),
  - `:a:b:c`, `:ab:cde:f` (stack enable),
  - `:a:b:c:`, `:ab:cde:f:` (stack disable).
- **Legacy mode**:
  - `--key value`, `--key=value`, `--key= value`
  - `-k value`, `-k=value`, `-k= value`
  - `-abc value`, (stack assign) if 'a' is registered option
  - `-abc=value`, `-abc= value` (stack assign) same as `-a=value -b=value -c=value`
  - `-f`, `--flag`, (enable)
  - `-no-f`, `--no-flag`, (disable)
  - `-xyz` (stack enable)
  - `-no-xyz`, (stack disable)
- **Dual-mode toggle**: the token `:` switches between modes at runtime.
- **Positional arguments**: declared with `CCLI_POS_ARGS`, parsed in order.
- **Array positional arguments**: `Y(cap, type, prop, ...)` for variable-length lists.
- **Optional arguments**: declared with `CCLI_OPT_ARGS`, with defaults.
- **Boolean flags**: declared with `CCLI_BOL_ARGS`, default true/false.
- **Built-in help and struct printing**: `colc_help_str()` and `colc_struct_str()`.
- **Types supported**: `STRING`, `UINT`, `INT`, `CHAR`.

## Build

Colon CLI is header-only. Compile your `main.c` as usual.

```sh
gcc -std=c99 -Iinclude src/main.c -o myapp
```

## Quick start

### 1. Define your arguments

In your `main.c` file, before `#include "coloncli.h"`:

```c
#define CCLI colc

#define CCLI_POS_ARGS(X, Y) \
    X(   STRING, input,  "input",  "Input file",  "Path to the source file to process") \
    X(   STRING, output, "output", "Output file", "Path to the generated output file") \
    Y(8, STRING, files,  "file",   "Extra files", "Additional input files to bundle")

#define CCLI_OPT_ARGS(X) \
    X(STRING, out_dir, ".",    ("o", "out", "dir"), "Output dir", "Directory for generated files") \
    X(STRING, format,  "json", ("f", "format"),     "Format",     "Output format: json, yaml, toml") \
    X(UINT,   depth,   4,      ("d", "depth"),      "Max depth",  "Maximum recursion depth") \
    X(UINT,   limit,   100,    ("l", "limit"),      "Limit",      "Maximum number of items to process")

#define CCLI_BOL_ARGS(X) \
    X(help,      FALSE, ("h", "help"),      "Help",      "Show this help message") \
    X(version,   FALSE, ("v", "version"),   "Version",   "Show version information") \
    X(force,     FALSE, ("F", "force"),     "Force",     "Overwrite existing output files") \
    X(recursive, TRUE,  ("r", "recursive"), "Recursive", "Process directories recursively") \
    X(verbose,   FALSE, ("V", "verbose"),   "Verbose",   "Print detailed processing info")

#include "coloncli.h"
```

### 2. Parse and use

```c
int main(int argc, char** argv) {
    colc_args_t cli = colc_default();

    char err[4096];
    char buf[4096];
    unsigned int n;

    n = colc_parse(&cli, err, sizeof(err), argc, argv);
    if (n) {
        fprintf(stderr, "%.*s\n", n, err);
        return 2;
    }

    if (cli.help) {
        n = colc_help_str(buf, sizeof(buf));
        fprintf(stdout, "%.*s\n", n, buf);
        return 0;
    }

    if (cli.version) {
        fprintf(stdout, "colc 1.0.0 - Colon CLI\n");
        return 0;
    }

    if (!cli.ok) {
        return 1;
    }

    if (cli.input.len == 0) {
        fprintf(stderr, "Error: missing required positional argument 'input'\n");
        n = colc_help_str(buf, sizeof(buf));
        fprintf(stderr, "%.*s\n", n, buf);
        return 2;
    }

    colc_struct_str(&cli, buf, sizeof(buf));
    fprintf(stdout, "%.*s\n", n, buf);
    return 0;
}
```

### 3. Run it

Colon mode (default):

```sh
./myapp input.txt out:./tmp level:3 :verbose
```

Legacy mode (toggle with `:`):

```sh
./myapp : input.txt --out ./tmp --level 3 --verbose
```

Both (Cursed):

```sh
./myapp out: ./tmp : --level 3 : :verbose input.txt
```

## Configuration macros

You can define these before `#include "coloncli.h"` to customize behavior:

| Macro               | Default                                 | Valid values | Purpose                                            |
| ------------------- | --------------------------------------- | ------------ | -------------------------------------------------- |
| `CCLI_LEGACY`       | `0`                                     | `0`, `1`     | Start in **legacy mode** (`1`) or colon mode (`0`) |
| `CCLI_POSITIONAL`   | `0`                                     | `0`, `1`     | Start in **positional mode** (`1`) or not (`0`)    |
| `CCLI_NAME`         | `CCLI` stringified                      | Any string   | Program **name** in help header/usage              |
| `CCLI_HEADER`       | `"$name - Colon CLI"`                   | Any string   | Help **title** line                                |
| `CCLI_USAGE`        | `"Usage: $name [OPTIONS] :: [ARGS...]"` | Any string   | Help **usage** line                                |
| `CCLI_DESCRIPTION`  | `""`                                    | Any string   | Help **description**                               |
| `CCLI_EXAMPLE`      | `""`                                    | Any string   | Help **examples** section                          |
| `CCLI_HELP_TYPE`    | `1`                                     | `0`, `1`     | Show **type** column in help                       |
| `CCLI_HELP_DEFAULT` | `1`                                     | `0`, `1`     | Show **default** column in help                    |
| `CCLI_HELP_LABEL`   | `1`                                     | `0`, `1`     | Show **label** column in help                      |
| `CCLI_HELP_DESC`    | `1`                                     | `0`, `1`     | Show **description** column in help                |
| `CCLI_HELP_ALIGN`   | `1`                                     | `0`, `1`     | Enable column **alignment** padding                |

## Macro reference

| Macro                 | Arguments                                                                    | Purpose                               |
| --------------------- | ---------------------------------------------------------------------------- | ------------------------------------- |
| `CCLI_POS_ARGS(X, Y)` | `X(type, prop, name, label, desc)` / `Y(cap, type, prop, name, label, desc)` | Declare positional arguments          |
| `CCLI_OPT_ARGS(X)`    | `X(type, prop, default, (keys...), label, desc)`                             | Declare optional arguments            |
| `CCLI_BOL_ARGS(X)`    | `X(prop, default, (keys...), label, desc)`                                   | Declare boolean flags                 |
| `(keys...)`           | `("short", "long")`, `("short", "short")`, `("long", "long")`                | Define short/long names (1-5 aliases) |

## Macro Signatures

- **Type:**
  - `STRING`
  - `CHAR`
  - `UINT`
  - `INT`
  - empty -> `STRING`
- **Bool:**
  - `TRUE`
  - `FALSE`
  - `YES`
  - `NO`
  - empty -> `FALSE`
- **Uint:** any non-negative number
- **String:** any string `""`
- **Aliases:** String\[1..5]
- **Identifier:** any identifier
- **Literal:** any literal int uint string char

### CCLI_POS_ARGS

- **X:** (type: Type, prop: Identifier, name: String, label: String, desc: String)
- **Y:** (cap: Uint, type: Type, prop: Identifier, name: String, label: String, desc: String)

### CCLI_OPT_ARGS

- **X:** (type: Type, prop: Identifier, default: Literal, keys: (Aliases), label: String, desc: String)

### CCLI_BOL_ARGS

- **X:** (prop: Identifier, default: Bool, keys: (Aliases), label: String, desc: String)

## Syntax reference

### Colon mode

| Form          | Meaning                                  |
| ------------- | ---------------------------------------- |
| `key:value`   | Set option `key` to `value` (inline)     |
| `key: value`  | Set option `key` to `value` (next token) |
| `:flag`       | Enable boolean `flag`                    |
| `:flag:`      | Disable boolean `flag`                   |
| `:a:b:c`      | Enable flags `a`, `b`, `c`               |
| `:abc`        | Enable flag `abc` (literal name)         |
| `:`           | Toggle to legacy mode (on/off)           |
| `::`          | Toggle positional mode (on/off)          |
| anything else | Positional argument                      |

### Legacy mode

| Form           | Colon equivalent          | Meaning                             |
| -------------- | ------------------------- | ----------------------------------- |
| `--key value`  | `key: value`              | Set option (next token)             |
| `--key=value`  | `key:value`               | Set option (inline)                 |
| `--key= value` | `key: value`              | Set option (consume next)           |
| `--flag`       | `:flag`                   | Enable boolean                      |
| `--no-flag`    | `:flag:`                  | Disable boolean                     |
| `-f value`     | `f: value`                | Set short option (next token)       |
| `-f=value`     | `f:value`                 | Set short option (inline)           |
| `-f`           | `:f`                      | Enable boolean                      |
| `-no-f`        | `:f:`                     | Disable boolean                     |
| `-abc`         | `:a:b:c`                  | Enable stacked flags `a`, `b`, `c`  |
| `-no-abc`      | `:a:b:c:`                 | Disable stacked flags `a`, `b`, `c` |
| `--`           | `::`                      | Toggle positional mode (on/off)     |
| `:`            | Toggle back to colon mode |
| anything else  | Positional argument       |

**Stack assignment (short form only):**

```
-dp=6   -> same as -d=6 -p=6
-dp= 6  -> same as -d=6 -p=6  (value taken from next token)
-dp 6   -> same as -d=6 -p=6  (only if 'd' is valid option)
```

**Rule:** The first stack member is looked up as a short option key.

- If the first member **is** a registered short option key, all remaining members are treated as short option keys too. Each is set to the same value. If any member is not a registered short option, this is an error.
- If the first member is **not** a registered short option key, parsing breaks out of option mode and hands the stack to boolean flag parsing. Each character is treated as a short boolean flag to enable. If any character is not a registered short boolean, this is an error.
- If the stack ends with or contain '=', it will **always** treat all stack members as short option keys. Each is set the same value. If any member is not a registered short option, this is an error.

**Examples:**

```
-dp=6   -> always set short option 'd' and 'p' to '6'.
-dp= 6  -> always set short option 'd' and 'p' to '6'.
-dp     -> if d is an option key, consume next token as value for d and p
            else if d is not an option key, try to enable booleans d and p
```

### Positional mode

When active (toggled with `::` or `--`),
every token is treated as a positional argument, even if it looks like an option or flag.

**Example:**

```sh
./myapp log:log.txt :: input:file.txt :output.txt :: :verbose
```

_Positional args:_ `input:file.txt` `:output.txt`

```sh
./myapp : --log=log.txt -- ==input==.txt -out--put.txt -- --verbose
```

_Positional args:_ `==input==.txt` `-out--put.txt`

### Value splitting

When an option token contains multiple separators,
the split happens at the **first** separator only:

- `key:val:ue` -> key=`key`, value=`val:ue`
- `key:value:` -> key=`key`, value=`value:`
- `key::value` -> key=`key`, value=`:value`
- `--key=val=ue` -> key=`key`, value=`val=ue`
- `--key=value=` -> key=`key`, value=`value=`
- `--key==value` -> key=`key`, value=`=value`

## Collision rule (mandatory)

**Having a collision is undefined behavior.**

All option keys and flag names must be unique across both short and long names. Symbols are case-sensitive.

- Two options cannot share the same short key.
- Two options cannot share the same long name.

**Allowed:**

- Keys are case-sensitive: `("d")` and `("D")` are different symbols.
- A short key and its long alias (e.g. `("d", "dir")`) is the intended mechanism.

**Avoid it:**

- An option key and a boolean flag name sharing the same short/long name is undefined behavior.
- An option/boolean using the same short/long name twice (e.g. `("d", "d")`) is undefined behavior.

## Undefined behavior

The following are undefined behavior:

- Using a type other than `STRING`, `CHAR`, `UINT`, `INT` (empty falls back to `STRING`)
- Empty `prop` name in any macro
- Fewer than 1 or more than 5 aliases inside `(keys...)`
- Empty string `""` as an alias inside `(keys...)`
- Fewer than 5 arguments to `X` in `CCLI_POS_ARGS`
- Fewer than 6 arguments to `Y` in `CCLI_POS_ARGS`
- Fewer than 5 arguments to `X` in `CCLI_BOL_ARGS`
- Fewer than 6 arguments to `X` in `CCLI_OPT_ARGS`
- Empty `default` value to `X` in `CCLI_OPT_ARGS`
- Boolean `default` not `FALSE`/`TRUE`/`NO`/`YES` to `X` in `CCLI_BOL_ARGS` (empty falls back to `FALSE`)
- Symbol collision (duplicate key/flag name across options and booleans)

## License

MIT.

## Inspired

The idea of macro-defined arguments processed by a single header was inspired by [esay-args](https://github.com/gouwsxander/easy-args#1-define-your-arguments).
