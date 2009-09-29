#ifndef __uint32_t_defined
typedef unsigned int uint32_t;
#endif

/* a*x + b using leal */
uint32_t nlin32(register uint32_t a,
	register uint32_t x,
	register uint32_t b)
{
	return a*x+b;
}
