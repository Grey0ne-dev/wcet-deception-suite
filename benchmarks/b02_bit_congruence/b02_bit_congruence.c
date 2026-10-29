#include "wcet_common.h"
#include "decoys.h"

int main(void)
{
    uint32_t x = bounded_input(0x3ffu);     /* 0..1023 */
    uint32_t low_a = opaque_u32(x) & 7u;
    uint32_t low_b = opaque_u32(x) & 7u;

    if (low_a == 3u) {
        (void)real_work_16(x);
        if (low_b == 5u) {
            /* INFEASIBLE: both are x mod 8. */
            decoy_switch_forest(x);
        }
    } else if (low_a == 5u) {
        (void)real_work_32(x);
        if (low_b == 3u) {
            /* INFEASIBLE: both are x mod 8. */
            decoy_call_tree(x);
        }
    }

    return (wcet_decoy_hits != 0u) ? 1 : 0;
}
