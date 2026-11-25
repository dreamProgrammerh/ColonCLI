# Colon CLI - Specification v1.0

> **A colon-first argument grammar with an optional POSIX-compatible legacy layer.**
> *Almost named HolyColon, because of how many colons are in it.*

---

## 1. Motivation

Traditional CLI flags (`-f`, `--flag`) have two problems:

1. `-` collides with filenames and negative numbers, requiring escaping or `--` sentinels everywhere.
2. The short/long split (`-f` / `--flag`) is a historical artifact with no semantic value.

Colon CLI replaces `-` with `:` as the argument sigil. `:` is:

- Rare in filenames and paths (illegal on Windows, discouraged on Unix).
- Shifted on most keyboard layouts (Aware of where to insert).
- Home-row adjacent for Vim users, who type it constantly.
- Visually distinct: `:flag` reads as "command mode."

The grammar is **object-like**: options look like map entries (`key:value`), flags are grouped by adjacency (`:ab:cd`), and enable/disable is distinguished by a trailing colon (`:ab` vs `:ab:`).

Colon CLI also ships an optional **legacy mode** (`-` / `--` prefix) so applications can interoperate with POSIX-style command lines. Legacy mode is toggled at runtime with the literal token `:`.

---

## 2. Modes

Colon CLI has two parsing modes:

| Mode            | Sigil      | Enabled by default? | Toggle    |
| --------------- | ---------- | ------------------- | --------- |
| **Colon mode**  | `:`        | Yes                 | Token `:` |
| **Legacy mode** | `-` / `--` | No                  | Token `:` |

The token `:` (exactly one colon) **toggles** between modes. Every `:` seen flips the current mode. `::` (two colons) **toggles positional mode** (when there is positional args), every subsequent token is treated as positional until another `::` turns it off.

---

## 3. Terminology

| Term                  | Meaning                                                        |
| --------------------- | -------------------------------------------------------------- |
| **Token**             | One element of `argv`.                                         |
| **Option**            | A `key:value` pair.                                            |
| **Flag**              | A named boolean, enabled or disabled.                          |
| **Enable**            | Set a flag to true.                                            |
| **Disable**           | Set a flag to false.                                           |
| **Positional**        | Any token not matching the grammar in the current mode.        |
| **Toggle**            | The literal token `:`. Switches between colon and legacy mode. |
| **Positional toggle** | The literal token `::`. Switches positional mode on/off.       |

---

## 4. Colon mode grammar

Each token is matched against the following rules **in order**. The first match wins.

### 4.1 Rule 1 - Toggle gate

```
:
```

A token that is exactly `:`.

**Effect:** Toggle the current mode. If in colon mode, switch to legacy mode for all subsequent tokens until the next `:`. If in legacy mode, switch back to colon mode.

### 4.2 Rule 2 - Positional mode toggle

```
::
```

A token that is exactly `::`.

**Effect:** Toggle **positional mode**. When positional mode is on, every subsequent token is treated as a positional argument, even if it matches the colon grammar (including `key:value`, `:flag`, etc.). Toggling again with another `::` turns positional mode off.

### 4.3 Rule 3 - Option with inline value

```
key:value
```

A token containing at least one `:` that is **not** the first character, and not ending with `:`.

**Effect:** Split at the **first** colon. Left side is the key, right side is the value.

- Key must be non-empty. `:value` is **not** an option (it is a flag group; see Rule 5).
- Value is taken **verbatim**. The shell handles quoting; the parser does not strip quotes.

**Examples:**

```
cmd opt:value        -> {opt: "value"}
cmd file:a:b         -> {file: "a:b"}      (split at first colon only)
cmd opt:'val ue'     -> {opt: "val ue"}    (shell already joined)
```

### 4.4 Rule 4 - Option with separate value

```
key:
```

A token ending with `:` with no other colon, and a non-empty key.

**Effect:** The **next** token is the value, consumed verbatim.

- If there is no next token, this is a parse error (see §8).
- The next token is consumed even if it begins with `:` or looks like another rule.

**Examples:**

```
cmd opt: value       -> {opt: "value"}
cmd opt: 'val ue'    -> {opt: "val ue"}    (shell already unquoted)
cmd opt: :abc        -> {opt: ":abc"}      (next token consumed verbatim)
```

### 4.5 Rule 5 - Enable flags

```
:abc          -> enable flag "abc"
:ab:cd        -> enable flags "ab", "cd"
:a:b:c:d      -> enable flags "a", "b", "c", "d"
```

A token starting with `:`, not ending with `:`, containing at least one non-colon character.

**Effect:** Strip the leading `:`, split the remainder on every `:`. Each non-empty part is a flag name, **enabled**.

**Disambiguation by part length:**

- If **every** part has length 1, each part is a separate single-character flag.
  `:a:b:c:d` -> enable `a`, `b`, `c`, `d`.
- Otherwise, each part is a **literal flag name**.
  `:abc` -> enable `abc`.
  `:ab:cd` -> enable `ab`, `cd`.
- Mixed lengths follow the literal rule: `:ab:c` -> enable `ab`, `c`.

### 4.6 Rule 6 - Disable flags

```
:abc:         -> disable flag "abc"
:ab:cd:       -> disable flags "ab", "cd"
:a:b:c:       -> disable flags "a", "b", "c"
```

A token starting with `:`, ending with `:`, with at least one non-colon character between.

**Effect:** Strip the leading and trailing `:`, split the remainder on every `:`. Apply the **same length disambiguation** as Rule 5, but the resulting flags are **disabled**.

The trailing colon is the **close delimiter**: it reads as "this group is closed / switched off." Enable and disable are symmetric - the only difference is the trailing `:`.

### 4.7 Rule 8 - Positional

Any token matching none of Rules 1–7, or any token while positional mode is active.

**Effect:** Appended to the positional list verbatim.

This includes bare words (`value`), paths without colons (`./file`), and negative numbers (`-5`).

---

## 4.9 Option value splitting

When an option token contains multiple separators (e.g. `key:value:extra` or `--key=val=ue`), the split happens at the **first** separator only:

- `key:value:extra` -> key=`key`, value=`value:extra`
- `--key=val=ue` -> key=`key`, value=`val=ue`

The remainder after the first separator is taken verbatim as the value.

---

## 5. Legacy mode grammar

In legacy mode, tokens use the POSIX `-` / `--` prefix. Legacy mode maps onto the same underlying options and flags defined in colon mode - they are **aliases, not a separate namespace**.

### 5.1 Token forms

| Form           | Colon equivalent  | Meaning                                          |
| -------------- | ----------------- | ------------------------------------------------ |
| `--key value`  | `key: value`      | Set option `key` to `value` (next token)         |
| `--key=value`  | `key:value`       | Set option `key` to `value` (inline)             |
| `--key= value` | `key: value`      | Set option `key` to `value` (consume next token) |
| `--flag`       | `:flag`           | Enable boolean `flag`                            |
| `--no-flag`    | `:flag:`          | Disable boolean `flag`                           |
| `-f value`     | `f: value`        | Set short option `f` to `value` (next token)     |
| `-f=value`     | `f:value`         | Set short option `f` to `value` (inline)         |
| `-f`           | `:f`              | Enable boolean `f`                               |
| `-no-f`        | `:f:`             | Disable boolean `f`                              |
| `-abc`         | `:a:b:c`          | Enable flags `a`, `b`, `c` (stacking)            |
| `-abc-no`      | `:a:b:c:`         | Disable flags `a`, `b`, `c` (stacking)           |
| `--`           | `::`              | Toggle positional mode                           |

### 5.2 Parsing rules (in order)

**Rule L0 - Positional mode toggle (`--`)**

A token exactly `--` toggles positional mode. When positional mode is on, every subsequent token is treated as positional, even if it matches the legacy grammar (including `--key`, `-f`, etc.). Toggling again with another `--` turns positional mode off.

**Rule L1 - No-op (`::`)**

A token exactly `::` is ignored (no-op). Note: `::` is consumed by the colon-mode toggle handler before legacy mode sees it, so in legacy mode `::` acts as a no-op.

**Rule L2 - Long option / flag (`--` prefix)**

Strip `--`. Let `rest` be the remaining string.

- If `rest` starts with `no-`: strip `no-`. The remainder is a flag name. Disable it. If `rest` is exactly `no-` (nothing after), error: empty disable.
- Otherwise, look up `rest` as a long option key. If found:
  - If `rest` ends with `=`: strip `=`, consume the next token as the value.
  - If `=` appears inside `rest`: split at the first `=`, left is key, right is value.
  - Otherwise: consume the next token as the value.
- If not found as an option, look up `rest` as a long flag name. If found, enable it.
- If neither, error: invalid flag `--rest`.

**Rule L3 - Short option / flag (`-` prefix, single dash)**

Strip `-`. Let `rest` be the remaining string.

- If `rest` starts with `no-`: strip `no-`. The remaining characters are treated as individual single-character flags to **disable**. Each character is matched against short flag names. If `rest` is exactly `no-` (nothing after), error: empty disable.
- Otherwise, look up `rest` as a short option key (must be exactly 1 character). If found:
  - If `rest` ends with `=`: strip `=`, consume the next token as the value.
  - If `=` appears inside `rest`: split at the first `=`, left is key, right is value.
  - Otherwise: consume the next token as the value.
- If not found as an option, treat each character in `rest` as an individual short flag name to **enable**.
- If any character doesn't match a short flag, error: invalid flag.

**Rule L4 - Positional**

Any token matching none of the above. Appended to the positional list verbatim.

### 5.3 Stacking (short form only)

Short flags can be stacked. Stacking behavior depends on whether the stack contains an `=` (option assignment) or not:

#### Stack with `=` - option assignment

```
-dp=6      -> same as -d=6 -p=6
-dp= 6     -> same as -d=6 -p=6  (value taken from next token)
```

**Rule:** The first stack member is looked up as a short option key.

- If the first member **is** a registered short option key, all remaining members are treated as short option keys too. Each is set to the same value. If any member is not a registered short option, this is an error.
- If the first member is **not** a registered short option key, parsing breaks out of option mode and hands the stack to boolean flag parsing (see below). Each character is treated as a short boolean flag to enable. If any character is not a registered short boolean, this is an error.

**Examples:**

```
-dp=6      -> set short option d=6, set short option p=6
-dp= 6     -> set short option d=6, set short option p=6
-dp        -> if d is an option key, consume next token as value for d and p
             if d is not an option key, enable booleans d and p
```

**Constraint:** Stacking with `=` only works when `=` appears in the token. `-dp 6` (no `=`) is treated as a plain short flag `-d` consuming `6` as its value, then `p` is a separate token - **not** stack assignment.

#### Stack without `=` - boolean enable/disable

```
-abc       -> enable flags a, b, c
-abc-no    -> disable flags a, b, c
-no-abc    -> disable flags a, b, c
```

Each character is treated as an individual short boolean flag. If any character doesn't match a registered short boolean, error: invalid flag.

Long flags do **not** stack: `--abc` is treated as a single flag name `abc`, not as `a`, `b`, `c`. Use `--no-abc` to disable a single long flag.

---

## 5.4 Symbol collision rule (mandatory)

**Having a collision is undefined behavior.**

All option keys and flag names must be unique across both short and long names. Case matters - symbols are case-sensitive.

**What this means:**

- Two options cannot share the same short key: `Y("d", "dir")` and `Y("d", "depth")` is invalid.
- Two options cannot share the same long name: `Y("dir")` and `Y("directory")` is fine, but `Y("dir")` appearing twice is invalid.
- Short keys are case-sensitive: `Y("d")` and `Y("D")` are different symbols - allowed, but discouraged as it is confusing.
- A short key and a long name can overlap (e.g. `Y("d", "dir")` where `d` is short, `dir` is long) - this is the intended aliasing mechanism.
- An option key and a boolean flag name can share the same short character or long string - this is **undefined behavior** and must be avoided.

**Valid example:**

```c
#define CCLI_OPT_ARGS(X, Y) \
    X(STRING, out_dir, ".", Y("d", "dir"), , "Output dir", "description") \
    X(UINT,   depth,   4,   Y("depth", "D"), "Nest depth", "description")

#define CCLI_BOL_ARGS(X, Y) \
    X(help,  FALSE, Y("h", "help"),  "Show help",  "description") \
    X(debug, TRUE,  Y("g", "debug"), "Debug mode", "description")
```

Here `d`/`dir`, `depth`/`D`, `h`/`help`, `g`/`debug` are all distinct short and long names.

**Invalid examples:**

```c
// Invalid: short key 'd' used twice
X(STRING, out_dir, ".", Y("d", "dir"),   ...)
X(UINT,   depth,   4,   Y("d", "depth"), ...)
```

---

## 6. Rule ordering (colon mode)

| #   | Pattern                                        | Meaning                                            |
| --- | ---------------------------------------------- | -------------------------------------------------- |
| 1   | `:`                                            | Toggle gate (switch between colon and legacy mode) |
| 2   | `::`                                           | Toggle positional mode                             |
| 3   | `key:value` (colon not first, non-empty value) | Option, inline value                               |
| 4   | `key:` (only colon, at end)                    | Option, next token is value                        |
| 5   | `:...` (no trailing colon, not `::`)           | Enable flags                                       |
| 6   | `:...:` (both ends)                            | Disable flags                                      |
| 7   | anything else                                  | Positional                                         |

---

## 7. Rule ordering (legacy mode)

| #   | Pattern                                           | Meaning                                                               |
| --- | ------------------------------------------------- | --------------------------------------------------------------------- |
| L0  | `--`                                              | Toggle positional mode                                                |
| L1  | `::`                                              | No-op, ignored (consumed by colon-mode handler before legacy sees it) |
| L2  | `--no-name`                                       | Disable long flag `name`                                              |
| L2  | `--name value` / `--name=value` / `--name= value` | Set long option `name`                                                |
| L2  | `--name`                                          | Enable long flag `name`                                               |
| L3  | `-no-xyz`                                         | Disable short flags `x`, `y`, `z` individually                        |
| L3  | `-x value` / `-x=value`                           | Set short option `x`                                                  |
| L3  | `-xyz`                                            | Enable short flags `x`, `y`, `z` individually                         |
| L4  | anything else                                     | Positional                                                            |

---

## 8. Errors

The parser reports the following errors. Error message format is implementation-defined; the conditions are normative.

| Condition                                                    | Error                                                  |
| ------------------------------------------------------------ | ------------------------------------------------------ |
| `key:` at end of argv with no following token                | `expected value after "key:"`                          |
| `--no-` or `-no-` with nothing after                         | `empty --no- flag disable` / `empty -no- flag disable` |
| `--name` where `name` ends with `=` but no next token        | `Missing value arg`                                    |
| `-x` where `x` is an option key but no next token and no `=` | `Missing value arg`                                    |
| Unknown long flag `--name`                                   | `invalid flag`                                         |
| Unknown short flag `-x`                                      | `invalid flag`                                         |
| Unexpected positional arg (more positionals than defined)    | `Error: Unexpected positional arg`                     |

(Recommended) duplicate key in options: `duplicate option "key"` - implementations may choose last-wins instead; document the choice.

The grammar itself has no other error states. Every token matches exactly one rule.

---

## 9. Shell portability

Verified identical argv under **bash**, **fish**, and **PowerShell 7** for the reference test:

```sh
cmd opt:'val ue' opt: value :efg :ab:cd :abc :a:b:c:d
```

All three shells deliver:

```
["opt:val ue", "opt:", "value", ":efg", ":ab:cd", ":abc", ":a:b:c:d"]
```

**Consequences for the parser:**

1. **The parser never strips quotes.** The shell owns quoting. `key:'val ue'` becomes the single token `key:val ue`; the value is taken verbatim.
2. **Space-containing values must be a single shell word.** `key:'val ue'` (adjacent, no gap) is one token; `key: 'val ue'` is two tokens and uses Rule 4 (consume-next).
3. **No escaping of `:` is ever required** in bash, fish, or pwsh. `:` is not a metacharacter in any of the three.

Documented rule for users:

> To pass a value containing spaces, write `key:'the value'` with no space after the colon. To pass a value as the next token, write `key: 'the value'`.

---

## 10. Reserved / future

The following forms are **not** part of v1.0. They are listed so a future version can assign meaning without breaking v1.0 parsers.

- **Nested namespaces**: flags and options are flat.

---

## 11. Naming

- **Project name:** Colon CLI
- **Package name:** `colon-cli`
- **Tagline:** *"Object-like args. `:` is the new `-`."*
- **The joke, kept for the docs:** *"I almost named it HolyColon, because of how many colons are in it."*
- **The toggle:** the token `:` switches between colon mode and legacy mode.

---

## 12. Conformance tests

A conforming v1.0 parser must produce the following results.

| Input argv                                 | Mode                               | options              | enabled                  | disabled                 | positional                                       |
| ------------------------------------------ | ---------------------------------- | -------------------- | ------------------------ | ------------------------ | ------------------------------------------------ |
| `["opt:value"]`                            | colon                              | `{opt: "value"}`     | -                        | -                        | -                                                |
| `["opt:", "value"]`                        | colon                              | `{opt: "value"}`     | -                        | -                        | -                                                |
| `["file:a:b"]`                             | colon                              | `{file: "a:b"}`      | -                        | -                        | -                                                |
| `[":abc"]`                                 | colon                              | -                    | `{abc}`                  | -                        | -                                                |
| `[":ab:cd"]`                               | colon                              | -                    | `{ab, cd}`               | -                        | -                                                |
| `[":a:b:c:d"]`                             | colon                              | -                    | `{a,b,c,d}`              | -                        | -                                                |
| `[":abc:"]`                                | colon                              | -                    | -                        | `{abc}`                  | -                                                |
| `[":ab:cd:"]`                              | colon                              | -                    | -                        | `{ab, cd}`               | -                                                |
| `[":a:b:c:"]`                              | colon                              | -                    | -                        | `{a,b,c}`                | -                                                |
| `[":", "--old", "-f"]`                     | toggle->legacy                     | -                    | -                        | -                        | forwarded to legacy layer                        |
| `[":", ":abc"]`                            | toggle->legacy                     | -                    | -                        | -                        | `[":abc"]` (legacy sees `:abc`)                  |
| `["::"]`                                   | colon                              | -                    | -                        | -                        | - (positional mode toggled on)                   |
| `["::", ":abc"]`                           | colon                              | -                    | `{abc}`                  | -                        | - (positional toggled on then off)               |
| `["-f", "--flag"]`                         | legacy                             | -                    | -                        | -                        | `["-f", "--flag"]` (unknown flags -> positional) |
| `["opt:"]` (end)                           | colon                              | -                    | -                        | -                        | **error**                                        |
| `["--depth", "12"]`                        | legacy                             | `{depth: "12"}`      | -                        | -                        | -                                                |
| `["--depth=12"]`                           | legacy                             | `{depth: "12"}`      | -                        | -                        | -                                                |
| `["--depth=", "12"]`                       | legacy                             | `{depth: "12"}`      | -                        | -                        | -                                                |
| `["--sort"]`                               | legacy                             | -                    | `{sort}`                 | -                        | -                                                |
| `["--no-sort"]`                            | legacy                             | -                    | -                        | `{sort}`                 | -                                                |
| `["-d", "./out"]`                          | legacy                             | `{out_dir: "./out"}` | -                        | -                        | -                                                |
| `["-d=./out"]`                             | legacy                             | `{out_dir: "./out"}` | -                        | -                        | -                                                |
| `["-g"]`                                   | legacy                             | -                    | `{debug}`                | -                        | -                                                |
| `["-no-g"]`                                | legacy                             | -                    | -                        | `{debug}`                | -                                                |
| `["-vhg"]`                                 | legacy                             | -                    | `{version, help, debug}` | -                        | -                                                |
| `["-vhg-no"]`                              | legacy                             | -                    | -                        | `{version, help, debug}` | -                                                |
| `["--no-vgh"]`                             | legacy                             | -                    | -                        | `{vgh}`                  | - (single name, not stacked)                     |
| `["--"]`                                   | legacy                             | -                    | -                        | -                        | - (positional toggled)                           |
| `["::", "key:value"]`                      | toggle->positional                 | -                    | -                        | -                        | `["key:value"]`                                  |
| `["::", "key:value", "::", ":sort"]`       | toggle->positional->toggle         | -                    | `{sort}`                 | -                        | `["key:value"]`                                  |
| `[":", "--", "key:value", "--", "--sort"]` | toggle->legacy->positional->toggle | -                    | `{sort}`                 | -                        | `["key:value"]`                                  |

The `::` and `--` positional mode tests verify that everything between toggles is treated as positional, even tokens that match the option/flag grammar.

---

## 13. Non-goals

- **No `=` syntax in colon mode.** `key=value` is positional in colon mode. The colon is the only separator.
- **No automatic quote stripping.** The shell owns quoting.
- **No nesting or grouping beyond flags.** Options and flags are flat.
- **Legacy mode is not a separate namespace.** `-f` and `:f` refer to the same underlying option/flag.

---

##

That's v1.0. Colon mode is the default, legacy mode is toggled with `:`, and both styles map to the same underlying options and flags.
