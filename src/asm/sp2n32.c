#ifndef __uint32_t_defined
typedef unsigned int uint32_t;
#endif

// movl 8(%ebp),%eax
uint32_t sp2n32(float f)
{
	uint32_t i;
	i = *(uint32_t *)&f;
	return i;
}
