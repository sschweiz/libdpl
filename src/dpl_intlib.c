#ifndef __uint32_t_defined
typedef unsigned int uint32_t;
#endif

#ifndef __int32_t_defined
typedef int int32_t;
#endif

#ifndef __size_t_defined
typedef unsigned int size_t;
#endif

#ifndef __dpl_null_defined
#define dpl_null	((void *)0)
#define __dpl_null_defined
#endif

#ifndef __dpl_intlib_contraints
#define dpl_max_signed		((int32_t)0x7fffffff)
#define dpl_min_signed		((int32_t)0xffffffff)
#define dpl_max_unsigned	((uint32_t)0xffffffff)
#define dpl_min_unsigned	((uint32_t)0x00000000)
#define __dpl_intlib_contraints
#endif

/* computes the next largest power of 2 for given integer */
uint32_t nlp32(register uint32_t x)
{
	x |= (x >> 1);
	x |= (x >> 2);
	x |= (x >> 4);
	x |= (x >> 8);
	x |= (x >> 16);
	return (x + 1);
}

/* computes the most significant 1 bit for given integer */
uint32_t msb32(register uint32_t x)
{
	x |= (x >> 1);
	x |= (x >> 2);
	x |= (x >> 4);
	x |= (x >> 8);
	x |= (x >> 16);
	return (x & ~(x >> 1));
}

/* counts the number of 1s in an integer */
uint32_t one32(register uint32_t x)
{
	x -= ((x >> 1) & 0x55555555);
	x = (((x >> 2) & 0x33333333) + (x & 0x33333333));
	x = (((x >> 4) + x) & 0x0f0f0f0f);
	x += (x >> 8);
	x += (x >> 16);
	return (x & 0x0000003f);
}

/* inplace typecasting */
#define itou32(x) ((uint32_t)((int32_t)x))
#define utoi32(x) ((int32_t)((uint32_t)x))

/* converts from 2's complement to unsigned */
uint32_t abs32(int32_t a)
{
	register uint32_t b;
	b = *(uint32_t *)&a;	// movl 8(%ebp),(%eax)
	b = ~b;			// negl %eax
	b++;			// incl %eax

	return b;
}

/* fast sawp without temporary variable */
uint32_t
__attribute__((fastcall))
swap32(uint32_t *a, uint32_t *b)
{
	*a ^= *b;
	*b ^= *a;
	*a ^= *b;
}

/* floating point direct conversion */
uint32_t sp2n32(float f)
{
	register uint32_t i;
	i = *(uint32_t *)&f;
	return i;

	//	movl 8(%ebp),(%eax)
}

/* fast zero of integer array */
uint32_t
__attribute__((nonnull(1))) __attribute__((fastcall))
zeroint(int32_t *a, size_t n)
{
	while (n != 0)
	{
		*a++ = 0x00000000;
		n--;
	}
}

