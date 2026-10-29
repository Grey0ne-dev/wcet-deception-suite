#include "wcet_common.h"
#include "decoys.h"

int main(void)
{
    uint32_t x = bounded_input(0xffu);      /* 0..255 */
    uint32_t copy = opaque_u32(x);          /* Semantically copy == x. */

    if (x < 128u) {
        (void)real_work_16(x);
        if (copy >= 128u) {
            /* INFEASIBLE: x < 128 and copy == x. */
            decoy_linear_1024(copy);
        }
    } else {
        (void)real_work_32(x);
        if (copy < 128u) {
            /* INFEASIBLE: x >= 128 and copy == x. */
            decoy_nested_64x64(copy);
        }
    }

    return (wcet_decoy_hits != 0u) ? 1 : 0;
}
