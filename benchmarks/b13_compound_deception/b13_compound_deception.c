#include "wcet_common.h"
#include "decoys.h"

int main(void)
{
    uint32_t x = bounded_input(0xffu);
    uint32_t n = (x & 15u) + 1u;            /* 1..16 */
    uint32_t mirror = opaque_xor(opaque_xor(x, 0x5au), 0x5au);
    uint32_t sum = 0u;
    uint32_t i;

    for (i = 0u; i < n; ++i) {
        sum += (x & 3u) + 1u;

        if (opaque_le_u32(n, i) != 0u) {
            /* INFEASIBLE from loop guard. */
            decoy_nested_64x64(i);
        }
    }

    if (mirror != x) {
        /* INFEASIBLE XOR round trip. */
        decoy_linear_1024(mirror);
    }

    if ((x < 128u) && (opaque_u32(mirror) >= 128u)) {
        /* INFEASIBLE correlated range. */
        decoy_switch_forest(x);
    }

    if (sum != n * ((x & 3u) + 1u)) {
        /* INFEASIBLE loop invariant. */
        decoy_call_tree(sum);
    }

    (void)real_work_32(sum ^ x);
    return (wcet_decoy_hits != 0u) ? 1 : 0;
}
