# CubeEngine/EngineSrc/Tina

## Role

Small custom scripting language pipeline: value model, tokenizer, parser, compiler, and runtime VM.

## Important Objects

- `TinaVal`: runtime value union for int/float/string/ref/function pointer/bool/nil plus arithmetic/comparison helpers.
- `TinaTokenizer`: source string to token list.
- `TinaParser`: recursive-descent parser producing `TinaASTNode`.
- `TinaCompiler`: AST to three-address-like IL commands and `TinaProgram`.
- `TinaRunTime`: stack/register VM executing a named function from `TinaProgram`.
- `TinaProgram`, `TinaFunctionInfo`, `ILCmd`, `OperandLocation`: compiled program representation.

## Contracts

- Pipeline is tokenizer -> parser -> compiler -> runtime.
- Stack locals are early-bound by index; env/upvalue/property-style names are late-bound through env maps.

## Boundaries

- Tina is independent from Python scripting; do not mix `ScriptPy` and Tina lifecycle assumptions.

## Known Traps

- `TinaVal` owns raw string memory; copy/destructor semantics need care.
