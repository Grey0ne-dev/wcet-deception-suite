#include "wcet_common.h"
#include "decoys.h"

int main(void)
{
    uint32_t key = bounded_input(15u);      /* exactly one guard can match */
    uint32_t hits = 0u;

#define GUARDED_CASE(K) do { \
        if (opaque_u32(key) == (K)) { \
            ++hits; \
            (void)real_work_16(key + (K)); \
        } \
    } while (0)

    GUARDED_CASE(0u);  GUARDED_CASE(1u);  GUARDED_CASE(2u);  GUARDED_CASE(3u);
    GUARDED_CASE(4u);  GUARDED_CASE(5u);  GUARDED_CASE(6u);  GUARDED_CASE(7u);
    GUARDED_CASE(8u);  GUARDED_CASE(9u);  GUARDED_CASE(10u); GUARDED_CASE(11u);
    GUARDED_CASE(12u); GUARDED_CASE(13u); GUARDED_CASE(14u); GUARDED_CASE(15u);

#undef GUARDED_CASE

    if (hits != 1u) {
        /* INFEASIBLE: opaque_u32 is identity and key is one value. */
        decoy_nested_64x64(hits);
    }

    return (wcet_decoy_hits != 0u) ? 1 : 0;
}
