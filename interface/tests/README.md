# interface/tests

Unit tests for the ZBook interface module, using Zephyr's `ztest` framework
and discovered by `twister`.

## Layout

```
tests/unit/
├── unit.cmake              # shared CMake boilerplate, see below
└── <category>/<peripheral>/
    ├── CMakeLists.txt
    ├── prj.conf
    ├── testcase.yaml
    └── src/
        └── main.c
```

`<category>` mirrors `interface/Kconfig.<category>` (`protocols`, `actuators`,
`io`, `sensors`, `storage`). `<peripheral>` mirrors one `CONFIG_ZBOOK_*`
symbol (e.g. `spi`, `uart`).

### Adding a new test

`CMakeLists.txt` is 2 lines via the shared `unit.cmake` helper (find_package/
project/target_sources boilerplate lives there once, not copy-pasted per
test):

```cmake
include(${CMAKE_CURRENT_SOURCE_DIR}/../../unit.cmake)
zbook_unit_test(<peripheral>)
```

This expects `src/main.c` in the same directory. `prj.conf` and
`testcase.yaml` still need to be written per test (Kconfig symbols and
test id/tags differ per peripheral).

## Conventions

- **Platform:** `platform_allow: native_sim` by default -- unit tests exercise
  pure logic / API contracts and must not need real hardware. If a peripheral
  can't be meaningfully tested without real silicon, tag it `hardware` and
  keep it under `tests/unit/<category>/<peripheral>/` anyway with
  `platform_allow: zbook@p2/rp2350b/m33`; don't invent a separate top-level
  tree for that until there's more than one such case.
- **Test id:** `interface.<category>.<peripheral>` in `testcase.yaml`, tags
  `[interface, <category>, <peripheral>]`.

## Running

```sh
west twister -T interface/tests --platform native_sim -v
```
