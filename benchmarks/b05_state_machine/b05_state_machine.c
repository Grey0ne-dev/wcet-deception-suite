#include "wcet_common.h"
#include "decoys.h"

int main(void)
{
    uint32_t bits = bounded_input(0xffffu);
    uint32_t state = 0u;
    uint32_t step;

    for (step = 0u; step < 16u; ++step) {   /* exact bound: 16 */
        uint32_t bit = (bits >> step) & 1u;
        uint32_t next;

        switch (state) {
        case 0u: next = bit ? 1u : 2u; break;
        case 1u: next = bit ? 2u : 3u; break;
        case 2u: next = bit ? 0u : 3u; break;
        case 3u: next = bit ? 0u : 1u; break;
        default: next = 7u; break;
        }

        state = opaque_u32(next) & 7u;
        wcet_sink ^= state + step;

        if (state == 7u) {
            /* INFEASIBLE: transitions from states 0..3 remain in 0..3. */
            decoy_switch_forest(state);
        }
    }

    return (wcet_decoy_hits != 0u) ? 1 : 0;
}
