# Validation

The standalone tools were checked on 2026-10-29.

| Check | Result |
|---|---|
| Original source integrity | All 104 files matched SHA-256 records |
| Modified source rejection | The integrity checker rejected a changed file |
| Native exhaustive oracle | All 13 cases passed, 264,050 legal inputs total |
| Cortex-M4, symbolic input | All 13 ARM binaries built and linked |
| Cortex-M0, initialized input | All 13 ARM binaries built and linked |
| Cortex-M3, initialized input | All 13 ARM binaries built and linked |
| ARM7TDMI, initialized input | All 13 ARM binaries built and linked |

Every cross-built binary had no undefined symbols. The symbolic-profile check
also verified that `wcet_input` resolves to `0x70000000` outside the ELF load
segments. Actual compiler versions and per-case binary hashes are recorded in
[validation.json](validation.json).

Reproduce the checks with fresh output directories:

```sh
python3 tools/bench.py verify
python3 tools/bench.py native --output build/native
python3 tools/bench.py build --cpu cortex-m4 --profile symbolic --output build/m4
python3 tools/bench.py build --cpu cortex-m0 --profile initialized --output build/m0
python3 tools/bench.py build --cpu cortex-m3 --profile initialized --output build/m3
python3 tools/bench.py build --cpu arm7tdmi --profile initialized --output build/arm7
```

These checks validate the source package and generic build/oracle workflow.
They do not run a WCET analyzer or establish a hardware timing bound.
