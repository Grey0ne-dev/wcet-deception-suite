#ifndef WCET_DECOYS_H
#define WCET_DECOYS_H

#include <stdint.h>
#include "wcet_common.h"

WCET_NOINLINE void decoy_linear_1024(uint32_t seed);
WCET_NOINLINE void decoy_nested_64x64(uint32_t seed);
WCET_NOINLINE void decoy_call_tree(uint32_t seed);
WCET_NOINLINE void decoy_switch_forest(uint32_t seed);

#endif
