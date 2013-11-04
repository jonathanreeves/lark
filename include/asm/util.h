#ifndef __UTIL_H__
#define __UTIL_H__

static inline int log2(int arg)
{
	unsigned int register out;
	asm volatile (
		"bsr %1, %0\n\t"
		: "=r" (out)
		: "r" (arg)
		: "memory"
	);
	return out;
}

static inline int log2rnd(int arg)
{
	unsigned int register out;
	asm volatile (
		"bsr %1, %0\n\t"
		"bsl %1, %1\n\t"
		"cmpl %1, %0\n\t"
		"je 1f\n\t"
		"incl %0\n"
		"1:\n"
		: "=r" (out)
		: "r" (arg)
		: "memory"
	);
	return out;
}

static void swap(void *arg1, void *arg2)
{
	asm volatile (
		"xor %0, %1\n\t"
		"xor %1, %0\n\t"
		"xor %0, %1\n"
		: /* no output */
		: "r" (*(unsigned int *)arg1), "r" (*(unsigned int *)arg2)
		: "memory"
	);
}

#endif
