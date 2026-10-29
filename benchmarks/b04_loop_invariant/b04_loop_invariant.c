#include "wcet_common.h"
#include "decoys.h"

int main(void)
{
    uint32_t n = bounded_input(31u) + 1u;   /* 1..32 */
    uint32_t i;
    uint32_t sum = 0u;

    for (i = 0u; i < n; ++i) {             /* exact upper bound: 32 */
        sum += 3u;
        wcet_sink ^= (sum + i);
    }

    sum = opaque_u32(sum);
    n = opaque_u32(n);

    if (sum != (3u * n)) {
        /* INFEASIBLE: loop invariant gives sum == 3*n. */
        decoy_nested_64x64(sum);
    }

    return (wcet_decoy_hits != 0u) ? 1 : 0;
}
