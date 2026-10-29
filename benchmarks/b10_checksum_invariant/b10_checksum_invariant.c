#include "wcet_common.h"
#include "decoys.h"

int main(void)
{
    uint32_t x = bounded_input(0xffffu);
    uint32_t values[16];
    uint32_t i;
    uint32_t forward = 0u;
    uint32_t reverse = 0u;

    for (i = 0u; i < 16u; ++i) {
        values[i] = opaque_u32(x ^ (i * 0x1111u));
    }

    for (i = 0u; i < 16u; ++i) {
        forward ^= opaque_u32(values[i]);
    }

    for (i = 16u; i > 0u; --i) {
        reverse ^= opaque_u32(values[i - 1u]);
    }

    if (forward != reverse) {
        /* INFEASIBLE: XOR is associative/commutative over same values. */
        decoy_switch_forest(forward ^ reverse);
    }

    wcet_sink ^= forward;
    return (wcet_decoy_hits != 0u) ? 1 : 0;
}
