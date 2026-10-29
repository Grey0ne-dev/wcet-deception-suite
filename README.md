# WCET Deception Suite

An open collection of **13 C benchmarks** for evaluating infeasible-path
analysis and the precision of worst-case execution time (WCET) estimates.

Each benchmark combines feasible work with expensive decoy code behind
conditions that should be unreachable under the stated input contract. The
suite tests whether an analyzer can discover those relationships rather than
counting every structural path as feasible.

The benchmarks are independent of any analyzer. Use your own compiler,
abstract interpreter, symbolic executor, model checker, or WCET tool.

## Quick start

Native validation needs Python 3.8+ and a GCC-compatible C compiler:

```sh
git clone https://github.com/Grey0ne-dev/wcet-deception-suite.git
cd wcet-deception-suite
python3 tools/bench.py verify
python3 tools/bench.py native --output build/native
```

The native oracle checks all **264,050 legal masked inputs** across the suite,
resetting the original globals before every execution. A decoy hit or nonzero
benchmark return makes the check fail. This checks source-level behavior on
the host; it is not a machine-level timing proof.

## Benchmarks

| Case | Challenge |
|---|---|
| b01_correlated_range | Correlated range conditions across an opaque identity |
| b02_bit_congruence | Incompatible modular arithmetic conditions |
| b03_xor_roundtrip | Double-XOR identity |
| b04_loop_invariant | Arithmetic relationship maintained through a loop |
| b05_state_machine | Unreachable state in a closed state machine |
| b06_alias_correlation | Relationship between pointer selection and a selector |
| b07_indirect_call | Relationship between an indirect target and its return tag |
| b08_repeated_diamonds | Repeated deterministic conditions and path growth |
| b09_fake_loop_exit | Implications of the loop guard |
| b10_checksum_invariant | Equality of forward and reverse XOR reductions |
| b11_mutually_exclusive_guards | Mutually exclusive conditions for a fixed key |
| b12_lockstep_counters | Equality of equally initialized and updated counters |
| b13_compound_deception | Combined range, loop, and arithmetic relationships |

Each directory in `benchmarks/` is self-contained. Compile its `bundle.c` once;
that file includes the benchmark and shared workers. Compiling the included C
files separately as well would duplicate their definitions.

## Build ARM binaries

With `arm-none-eabi-gcc` and `arm-none-eabi-nm` installed:

```sh
python3 tools/bench.py build --cpu cortex-m4 --profile symbolic \
  --output build/m4-symbolic
```

The helper supports Cortex-M0, Cortex-M3, Cortex-M4, and ARM7TDMI GCC builds.
It defaults to `-O0`, software floating point, no LTO, and no jump tables.
Select another optimization level with `--opt O2`; record the actual compiler
and flags when comparing tools. Each output directory contains ELF files,
command logs, hashes, and a manifest. Existing output directories are rejected.

You can also compile the bundles directly using your target's compiler and
supply your own startup code and linker layout. The included `baremetal.ld`
files are analysis layouts that enter `main`; they are not bootable board BSPs.

## Input profiles

| Profile | Contract |
|---|---|
| `symbolic` (default) | An unknown readable 32-bit input at `0x70000000`, read once and masked by each benchmark; other globals retain original initializers |
| `initialized` | Original ELF globals, including `wcet_input = 0x5a3` |

```sh
python3 tools/bench.py build --cpu cortex-m0 --profile initialized \
  --output build/m0-initialized
```

Invoke `main` with a valid stack and initialized ordinary memory. Assume a
single invocation with no concurrent modification of the benchmark globals.
For the symbolic profile, configure the external input as unknown in your
analyzer; it is intentionally outside ELF load segments. Replacing unmapped
memory with zero changes the experiment.

The two profiles are different experiments. An initialized-input result does
not establish the same precision for every masked input. `suite.json` records
input masks, case categories, conservative loop limits, and b07's complete
four-function target set by name.

## Using a WCET analyzer

1. Choose a target, compiler, input profile, and timing model.
2. Compile identical binaries for every analysis variant you compare.
3. Map the source loop limits in `suite.json` to your tool's loop convention.
4. Provide the complete indirect-call targets where needed, then let the
   analyzer prove infeasible paths itself.
5. Report timing bounds, completion status, runtime, and resource limits.

No analyzer-specific path-exclusion files or assumed infeasible-edge facts
are supplied. Source comments and `manifest.csv` describe the intended
challenge; they are not proof inputs. There is no universal golden cycle count:
WCET depends on the executable, timing model, and input contract.

See [EVALUATION.md](EVALUATION.md) for comparison and reporting guidance.

## Validation and provenance

The 104 recovered C/header/linker/manifest files retain their original bytes.
`benchmarks/SHA256SUMS` pins their hashes. The standalone tools have passed
native exhaustive validation and ARM cross-build checks; see
[VALIDATION.md](VALIDATION.md).

This repository contains only the benchmark package and its generic helpers.
It has its own repository history and does not include an analyzer implementation.

## License

[Apache License 2.0](LICENSE). See [NOTICE](NOTICE) for source provenance.
