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
