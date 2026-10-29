#include "wcet_common.h"
#include "decoys.h"

int main(void)
{
    uint32_t x = bounded_input(0xffffu);
    uint32_t encoded = opaque_xor(x, 0xa55au);
    uint32_t decoded = opaque_xor(encoded, 0xa55au);

    (void)real_work_16(decoded);

    if (decoded != x) {
        /* INFEASIBLE: (x XOR M) XOR M == x. */
        decoy_linear_1024(decoded);
        decoy_nested_64x64(x);
    }

    return (wcet_decoy_hits != 0u) ? 1 : 0;
}
