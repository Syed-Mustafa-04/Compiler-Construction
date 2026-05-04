# json2xml

A JSON-to-XML translator that parses JSON input, builds an Abstract Syntax Tree (AST), and outputs pretty-printed XML.

## Build

```bash
make
```

Requires: `flex`, `bison`, `gcc`

Generates:
- `json2xml` — executable
- `ast.txt` — AST structure (generated at runtime)

## Run

```bash
./json2xml < input.json > output.xml
```

Reads JSON from stdin, writes XML to stdout.

**Example:**
```bash
./json2xml < tests/test1.json
```

## AST Structure

The AST consists of nodes with types:
- `NODE_OBJECT` — JSON object `{}`
- `NODE_ARRAY` — JSON array `[]`
- `NODE_PAIR` — key-value pair
- `NODE_STRING`, `NODE_NUMBER`, `NODE_BOOL`, `NODE_NULL` — scalar values

Each node stores:
- `type` — node type
- `key` — pair key (if applicable)
- `str_val` — scalar value
- `children` — child nodes
- `child_count` — number of children

**AST Output:**
After parsing, `ast.txt` contains the AST in text format:
```
[OBJECT]
  [PAIR] key="name" value="Alice"
  [PAIR] key="age" value="30"
```

## XML Output

Objects become element hierarchies; arrays become repeated `<item>` elements.

**JSON:**
```json
{ "person": { "name": "Alice", "age": 30 } }
```

**XML:**
```xml
<root>
  <person>
    <name>Alice</name>
    <age>30</age>
  </person>
</root>
```

## Features

- **Pretty printing** — indented, readable XML
- **Unicode escapes** — `\u1234` and surrogate pairs in strings
- **Scientific notation** — `1e10`, `2.5E-3`
- **Error reporting** — syntax errors with line and column numbers
- **AST preservation** — `ast.txt` persists after execution

## Error Handling

Invalid JSON exits with non-zero status and prints error to stderr:
```
Syntax Error: unexpected token at line 2, col 5
```

**Test error handling:**
```bash
./tests/run_error_test.sh
```

## Assumptions & Limitations

- JSON input must be valid according to RFC 7159
- Root element must be an object or array
- No streaming; entire input loaded into AST
- No DTD or schema validation
- XML attributes not supported (JSON lacks distinction)
- Duplicate object keys keep last value
