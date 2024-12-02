## Vangers Scripting Subsystem (vss)

Native implementation of scripting system based on [duktape](https://duktape.org/).

Please format code with .clang-format before pushing changes:

```bash
clang-format -i -style=file:src/vss/.clang-format src/vss/quant-names.h
clang-format -i -style=file:src/vss/.clang-format src/vss/sys.cpp
clang-format -i -style=file:src/vss/.clang-format src/vss/sys.h
clang-format -i -style=file:src/vss/.clang-format src/vss/sys-bridge.cpp
clang-format -i -style=file:src/vss/.clang-format src/vss/sys-bridge.h
clang-format -i -style=file:src/vss/.clang-format src/vss/sys-modules.cpp
clang-format -i -style=file:src/vss/.clang-format src/vss/sys-modules.h
clang-format -i -style=file:src/vss/.clang-format src/vss/sys-quant.cpp
```

Read more in [scripting/README.md](../../scripting/README.md)