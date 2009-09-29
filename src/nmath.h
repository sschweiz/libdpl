#ifndef __NMATH__
#define __NMATH__
/*
 * nmath returns an error values of -NINF (which is the largest value of a
 * signed 32 bit integer.  It also sets an error message.  A request such
 * as ln(-1) will not break the code, rather return -NINF and change errno
 * to the appropriote error.
 */

#include <errno.h>

#define NPI	3.1415926535
#define NE	2.7182818284
#define NSQRT2	1.4142135623
#define NSQRT3	1.7320508075
#define NLOG2	0.6931471805
#define NLOG10	2.3025850929
#define NGOLD	1.6180339887

#define NINF	(1 << 30)

#define NCORDIC	0.6072529350
#define N32BITS	4294967296.0

/* error conditions */
#define ERR_NRANGE	ERANGE	/* result out of range (+- infinity) */
#define ERR_NBOUNDS	EDOM    /* input out of bounds (ie. wanted   */
				/* pos integers got negative) */
#define ERR_NIMAG	ENOTSUP /* result is imaginary */

/* typedefs */
typedef long double		nfloat_t;
typedef unsigned long long int	nuint_t;
typedef signed long long int	nsint_t;
typedef signed int		nint_t;
typedef double			ndouble_t;

#define DTOUINT64(x) ((nuint_t)(x * N32BITS))
#define UINT64TOD(x) ((ndouble_t)(x / N32BITS))

static const ndouble_t NMATH_CORDIC_ANGLES[] = {
	0.78539816339745, 0.46364760900081, 0.24497866312686, 0.12435499454676,
	0.06241880999596, 0.03123983343027, 0.01562372862048, 0.00781234106010,
	0.00390623013197, 0.00195312251648, 0.00097656218956, 0.00048828121119,
	0.00024414062015, 0.00012207031189, 0.00006103515617, 0.00003051757812,
	0.00001525878906, 0.00000762939453, 0.00000381469727, 0.00000190734863,
	0.00000095367432, 0.00000047683716, 0.00000023841858, 0.00000011920929,
	0.00000005960464, 0.00000002980232, 0.00000001490116, 0.00000000745058
};

static const ndouble_t NMATH_CORDIC_LOGS[] = {
	0.69314718055994, 0.09531017980432, 0.00995033085316, 0.00099950033308,
	0.00009999500033, 0.00000999995000, 0.00000099999500, 0.00000009999995,
	0.00000000999999, 0.00000000099999, 0.00000000009999, 0.00000000000999
};

/*
 * ( a b )
 * ( c d )
 */
typedef struct smat2
{
	nuint_t a, b;
	nuint_t c, d;
} smat2_t;

/* sqaure 2x2 matrix helper functions */
smat2_t multiply_smat2(smat2_t m, smat2_t n);
smat2_t create_smat2(nuint_t a, nuint_t b, nuint_t c, nuint_t d);
smat2_t rpow_smat2(smat2_t x, nint_t n);

/* basic mathematical functions */
ndouble_t rpow(ndouble_t x, nint_t n);	/* factorial nint_teger power */
nuint_t rfib(nint_t n);
nint_t nfib(nint_t n);
nint_t nnint_t(ndouble_t n);
nuint_t rfact(nint_t n); /* recursive factorial O(lg n) */
nuint_t nfact(nint_t n);
nuint_t ncomb(nint_t n, nint_t r);
nuint_t nperm(nint_t n, nint_t r);
ndouble_t npow(ndouble_t x, ndouble_t n); /* log based x^y */
ndouble_t ndec(ndouble_t n);
ndouble_t nabs(ndouble_t n);
ndouble_t nmod(ndouble_t a, ndouble_t n);
ndouble_t nmin(ndouble_t a, ndouble_t b);
ndouble_t nmax(ndouble_t a, ndouble_t b);
nint_t ngcd(nint_t a, nint_t b);
nint_t nlcm(nint_t a, nint_t b);

nint_t nsign(ndouble_t n);
nint_t nfloor(ndouble_t n);
nint_t nceil(ndouble_t n);

/* cordic sin function and related trigs */
ndouble_t nsin(ndouble_t n);
ndouble_t ncos(ndouble_t n);
ndouble_t ntan(ndouble_t n);

/* briggs log functions and related */
ndouble_t nln(ndouble_t n);
ndouble_t nlog(ndouble_t n);
ndouble_t nlg(ndouble_t n);

ndouble_t nexp(ndouble_t n);

ndouble_t nsqrt(ndouble_t n);

/* generating functions */
ndouble_t nbern(nint_t n);
nint_t npoly(nint_t n, nint_t r);
nint_t ncat(nint_t n);

#endif
