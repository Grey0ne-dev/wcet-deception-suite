#include "wcet_common.h"
#include "decoys.h"

int main(void)
{
    uint32_t x = bounded_input(0xffu);
    uint32_t seen_low = 0u;
    uint32_t seen_high = 0u;

#define DIAMOND() do { \
        if (opaque_lt_u32(x, 128u) != 0u) { \
            seen_low = 1u; \
            wcet_sink += 3u; \
        } else { \
            seen_high = 1u; \
            wcet_sink += 5u; \
        } \
    } while (0)

    DIAMOND(); DIAMOND(); DIAMOND(); DIAMOND();
    DIAMOND(); DIAMOND(); DIAMOND(); DIAMOND();
    DIAMOND(); DIAMOND(); DIAMOND(); DIAMOND();
    DIAMOND(); DIAMOND(); DIAMOND(); DIAMOND();

#undef DIAMOND

    if ((seen_low != 0u) && (seen_high != 0u)) {
        /* INFEASIBLE: every pure predicate call receives the same x. */
        decoy_call_tree(x);
        decoy_linear_1024(x);
    }

    return (wcet_decoy_hits != 0u) ? 1 : 0;
}
