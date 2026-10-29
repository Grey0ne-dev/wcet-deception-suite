#ifndef WCET_COMMON_H
#define WCET_COMMON_H

#include <stdint.h>
#include <stddef.h>

#if defined(WCET_DISABLE_ASM_BARRIER)
#define WCET_NOINLINE __attribute__((noinline))
#define WCET_USED __attribute__((used))
#define WCET_BARRIER() ((void)0)
#elif defined(__GNUC__) || defined(__clang__)
#define WCET_NOINLINE __attribute__((noinline))
#define WCET_USED __attribute__((used))
#define WCET_BARRIER() __asm__ volatile("" ::: "memory")
#else
#define WCET_NOINLINE
#define WCET_USED
#define WCET_BARRIER() ((void)0)
#endif

extern volatile uint32_t wcet_input;
extern volatile uint32_t wcet_sink;
extern volatile uint32_t wcet_decoy_hits;
extern uint32_t wcet_buffer_a[16];
extern uint32_t wcet_buffer_b[16];

WCET_NOINLINE uint32_t opaque_u32(uint32_t x);
WCET_NOINLINE uint32_t opaque_xor(uint32_t x, uint32_t mask);
WCET_NOINLINE uint32_t opaque_lt_u32(uint32_t x, uint32_t y);
WCET_NOINLINE uint32_t opaque_le_u32(uint32_t x, uint32_t y);
WCET_NOINLINE uint32_t bounded_input(uint32_t mask);
WCET_NOINLINE uint32_t *select_buffer(uint32_t selector);
WCET_NOINLINE uint32_t real_work_16(uint32_t seed);
WCET_NOINLINE uint32_t real_work_32(uint32_t seed);

#endif
