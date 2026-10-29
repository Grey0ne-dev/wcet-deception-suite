#include "wcet_common.h"

volatile uint32_t wcet_input = 0x5a3u;
volatile uint32_t wcet_sink = 0u;
volatile uint32_t wcet_decoy_hits = 0u;
uint32_t wcet_buffer_a[16] = {1u, 2u, 3u, 4u};
uint32_t wcet_buffer_b[16] = {5u, 6u, 7u, 8u};

WCET_NOINLINE uint32_t opaque_u32(uint32_t x)
{
    volatile uint32_t y = x;
    WCET_BARRIER();
    return y;
}

WCET_NOINLINE uint32_t opaque_xor(uint32_t x, uint32_t mask)
{
    volatile uint32_t y = x ^ mask;
    WCET_BARRIER();
    return y;
}

WCET_NOINLINE uint32_t opaque_lt_u32(uint32_t x, uint32_t y)
{
    volatile uint32_t r = (x < y) ? 1u : 0u;
    WCET_BARRIER();
    return r;
}

WCET_NOINLINE uint32_t opaque_le_u32(uint32_t x, uint32_t y)
{
    volatile uint32_t r = (x <= y) ? 1u : 0u;
    WCET_BARRIER();
    return r;
}

WCET_NOINLINE uint32_t bounded_input(uint32_t mask)
{
    uint32_t x = wcet_input;
    WCET_BARRIER();
    return x & mask;
}

WCET_NOINLINE uint32_t *select_buffer(uint32_t selector)
{
    WCET_BARRIER();
    return (selector == 0u) ? wcet_buffer_a : wcet_buffer_b;
}

static uint32_t work_step(uint32_t x, uint32_t i)
{
    x ^= (i * 0x9e3779b9u) + 0x7f4a7c15u;
    x = (x << 5) | (x >> 27);
    return x + (i ^ 0xa5a5a5a5u);
}

WCET_NOINLINE uint32_t real_work_16(uint32_t seed)
{
    uint32_t x = seed;
    uint32_t i;
    for (i = 0u; i < 16u; ++i) {
        x = work_step(x, i);
    }
    wcet_sink ^= x;
    return x;
}

WCET_NOINLINE uint32_t real_work_32(uint32_t seed)
{
    uint32_t x = seed;
    uint32_t i;
    for (i = 0u; i < 32u; ++i) {
        x = work_step(x, i + 17u);
    }
    wcet_sink ^= x;
    return x;
}
