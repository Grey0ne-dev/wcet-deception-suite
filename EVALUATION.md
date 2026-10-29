# Evaluation guide

The suite measures precision on infeasible paths, not just whether a tool can
produce a number. An aggressive estimate is useful only if its assumptions
cover the executions being analyzed.

## Keep comparisons consistent

Record the source hashes, exact ELF hash, compiler and version, complete flags,
entry function, input profile, loop facts, indirect targets, and hardware model.
Use the same executable and assumptions across compared analysis variants.
Keep any changes to compilation or initialization in separate experiments.

`initialized` fixes the input through the original ELF data. `symbolic` permits
every value that can reach the source's input mask, while other globals retain
their original initializers. The native oracle resets those globals and checks
every masked value, including both selected-buffer alternatives in b06 and all
four worker targets in b07.

Loop metadata in `suite.json` provides conservative source-derived limits.
A function can contain multiple loops, so apply the stated limit to the relevant
source loops and map it to your tool's header/back-edge counting convention.
These limits are external assumptions for a comparison, not proofs that a path
is infeasible. The b07 target list includes all four selectable workers.

## Report outcomes explicitly

A suggested results table is:

| Case | Target and profile | Baseline bound | Candidate bound | Analysis status | Time |
|---|---|---:|---:|---|---:|
| b01_correlated_range | Your target and input profile | ... | ... | Complete, partial, refused, or timeout | ... |

Report the number of certified path constraints when the tool exposes it.
Distinguish completed symbolic exploration from a partial analysis that retains
independently checked facts. An unfinished maximizing ILP incumbent is not a
WCET upper bound. Retain failed and unchanged cases when reporting coverage.

The reduction percentage for two positive comparable bounds is:

`100 * (baseline - candidate) / baseline`

Lower estimates across different binaries, timing models, or input contracts
cannot be attributed solely to analysis precision. There is no target-independent
reference WCET. Source-level exhaustive execution checks expected behavior; it
does not validate target instruction semantics, timing, or an analyzer's proofs.

## Contributing a comparison

Include your tool/version, target and timing assumptions, profile, compiler
commands, executable/source hashes, resource budgets, and all case outcomes.
Attach reproducible commands and evidence for any claimed infeasible-path proof.
Use a fresh result directory for each run so a failed rerun cannot reuse an old
successful manifest.
