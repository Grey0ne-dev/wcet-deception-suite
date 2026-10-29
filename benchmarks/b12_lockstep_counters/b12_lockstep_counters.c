#include "wcet_common.h"
#include "decoys.h"

int main(void)
{
    uint32_t n = bounded_input(31u) + 1u;
    uint32_t a = 0u;
    uint32_t b = 0u;
    uint32_t i;

    for (i = 0u; i < n; ++i) {             /* exact upper bound: 32 */
        a += 2u;
        b += 2u;
        wcet_sink ^= a + i;

        if (opaque_u32(a) != opaque_u32(b)) {
            /* INFEASIBLE: a and b start equal and receive same update. */
            decoy_linear_1024(a ^ b);
        }
    }

    if (a != b) {
        /* INFEASIBLE for the same lockstep invariant. */
        decoy_call_tree(a ^ b);
    }

    return (wcet_decoy_hits != 0u) ? 1 : 0;
}
