#include "wcet_common.h"
#include "decoys.h"

typedef uint32_t (*worker_fn)(uint32_t);

static WCET_NOINLINE uint32_t worker0(uint32_t x) { return (opaque_u32(x ^ 0x11111110u) & ~3u) | 0u; }
static WCET_NOINLINE uint32_t worker1(uint32_t x) { return (opaque_u32(x ^ 0x22222220u) & ~3u) | 1u; }
static WCET_NOINLINE uint32_t worker2(uint32_t x) { return (opaque_u32(x ^ 0x44444440u) & ~3u) | 2u; }
static WCET_NOINLINE uint32_t worker3(uint32_t x) { return (opaque_u32(x ^ 0x88888880u) & ~3u) | 3u; }

static WCET_NOINLINE worker_fn select_worker(uint32_t key)
{
    switch (key & 3u) {
    case 0u: return worker0;
    case 1u: return worker1;
    case 2u: return worker2;
    default: return worker3;
    }
}

int main(void)
{
    uint32_t x = bounded_input(0xffffu);
    uint32_t key = x & 3u;
    worker_fn fn = select_worker(key);
    uint32_t result = fn(x);

    if ((result & 3u) != key) {
        /* INFEASIBLE: selected worker stamps key into result low bits. */
        decoy_switch_forest(result);
    }

    return (wcet_decoy_hits != 0u) ? 1 : 0;
}
