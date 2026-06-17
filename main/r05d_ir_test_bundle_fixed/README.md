# Midea R05D one-shot IR test - fixed

Fixes:
- `main.c`: add `#include "ir_tx.h"`
- `IR/ir_tx.c`: initialize the empty RMT copy encoder config with `{}` to remove the warning under ESP-IDF v5.5.4

Replace:
- `main/main.c`
- `main/IR/ir_tx.c`
- `main/IR/ir_tx.h`
