#include "wcet_common.h"
#include "decoys.h"

int main(void)
{
    uint32_t selector = bounded_input(1u);
    uint32_t *p = select_buffer(selector);

    p[0] = opaque_u32(p[0] + 1u);
    wcet_sink ^= p[0];

    if (selector == 0u) {
        if (p == wcet_buffer_b) {
            /* INFEASIBLE: select_buffer(0) returns buffer_a. */
            decoy_call_tree(p[0]);
        }
    } else {
        if (p == wcet_buffer_a) {
            /* INFEASIBLE: select_buffer(1) returns buffer_b. */
            decoy_linear_1024(p[0]);
        }
    }

    return (wcet_decoy_hits != 0u) ? 1 : 0;
}
