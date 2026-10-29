#include "wcet_common.h"
#include "decoys.h"

int main(void)
{
    uint32_t n = bounded_input(31u) + 1u;   /* 1..32 */
    uint32_t i;
    uint32_t acc = 0u;

    for (i = 0u; i < n; ++i) {             /* exact upper bound: 32 */
        acc ^= real_work_16(i + acc);

        if (opaque_le_u32(n, i) != 0u) {
            /* INFEASIBLE: loop guard establishes i < n. */
            decoy_nested_64x64(acc);
        }
    }

    wcet_sink ^= acc;
    return (wcet_decoy_hits != 0u) ? 1 : 0;
}
