#include <errno.h>
#include "nmath.h"

/*
 * gcc -Wall -fPIC -c nmath.c
 * gcc -shared -Wl,-soname,libnmath.so.1 -o libnmath.so.1.0 namth.o
 * ln -sf libnmath.so.1.0 libnmath.so
 * ln -sf libnmath.so.1.0 libnmath.so.1
 *
 * CHECKLIST:
 *  [x] fundamental constants
 * log based
 *  [x] rpow - recursive integer power (non negative)
 *  [x] npow - log based x^y power x,y are real numbers
 *  [x] nln - log base e
 *  [x] nlog - log base 10
 *  [x] nlg - log base 2
 *  [x] nexp - exponential function (e^x)
 *  [x] nsqrt - square root
 *  [ ] nrexp - significand and exponent
 * generating functions
 *  [x] nfib - recursive slow fibonacci number
 *  [x] rfib - recursive fast fibonacci number
 *  [x] nbern - bernoulli number
 *  [x] npoly - the n-th regular r-topic figurate number
 *  [x] ncat - catalan number
 * number theory
 *  [x] rfact - recursive factorial
 *  [x] nfact - alias of rfact
 *  [x] ncomb - n choose r combination
 *  [x] nperm - n perm r permutation
 *  [x] ngcd - greatest common divisor
 *  [x] nlcm - lowest common multiple
 * arithmatic functions
 *  [x] nint - nearest integer function
 *  [x] ndec - returns decimal digits
 *  [x] nabs - absolute value function
 *  [x] nsign - sign function
 *  [x] nceil - ceiling function
 *  [x] nfloor - floor function
 *  [x] nmin - minimum
 *  [x] nmax - maximum
 *  [x] nmod - remainder function (modulo arithmatic)
 * trigonometric functions
 *  [x] nsin - sine
 *  [x] ncos - cosine
 *  [x] ntan - tangent
 *  [ ] natan - arc tangent
 *  [ ] nasin - arc sine
 *  [ ] nacos - arc cosine
 *  [ ] nsinh - hyperbolic sine
 *  [ ] ncosh - hyperbolic cosine
 *  [ ] ntanh - hyperbolic tangent
 *
 * - make sure all functions return proper values and
 *   take proper inputs over their range
 * - clean up code
 */

/*
 * ( F_n+1	F_n   ) = ( 1 1 )
 * ( F_n	F_n-1 )   ( 1 0 )
 */
smat2_t multiply_smat2(smat2_t m, smat2_t n)
{
	smat2_t r;
	r.a = m.a * n.a + m.b * n.c;
	r.b = m.a * n.b + m.b * n.d;
	r.c = m.c * n.a + m.d * n.c;
	r.d = m.c * n.b + m.d * n.d;

	return r;
}

smat2_t create_smat2(nuint_t a, nuint_t b, nuint_t c, nuint_t d)
{
	smat2_t r;
	r.a = a;
	r.b = b;
	r.c = c;
	r.d = d;

	return r;
}

smat2_t rpow_smat2(smat2_t x, nint_t n)
{
	smat2_t ret = create_smat2(1, 0, 0, 1); // identity

	if (n == 1) { return x; }

	if ((n % 2) == 1)
	{
		ret = multiply_smat2(ret, rpow_smat2(x, (n - 1) / 2));
		return multiply_smat2(multiply_smat2(ret, ret), x);
	} else {
		ret = multiply_smat2(ret, rpow_smat2(x, n / 2));
		return multiply_smat2(ret, ret);
	}
}

ndouble_t rpow(ndouble_t x, nint_t n)
{
	ndouble_t ret = 1.0;

	if (n == 0) { return 1.0; }
	if (n == 1) { return x; }

	if ((n % 2) == 1)
	{
		// odd
		ret *= rpow(x, (n - 1) / 2);
		return ret * ret * x;
	} else {
		// even
		ret *= rpow(x, n / 2);
		return ret * ret;
	}
}

// O(lg n)
nuint_t rfib(nint_t n)
{
	// we want the nth power of the fib matrix;
	if (n < 0) { return -NINF; }
	if (n == 0) { return 0; }
	if (n == 1) { return 1; }

	smat2_t fib;
	fib = create_smat2(1, 1, 1, 0);

	fib = rpow_smat2(fib, n);

	return fib.b;
}

// O(Phi^n) (Phi = (1 + sqrt(5)) / 2)
nint_t nfib(nint_t n)
{
	if (n < 0) { errno = ERR_NBOUNDS; return -NINF; }
	if (n == 0) { return 0; }
	if (n == 1) { return 1; }

	return nfib(n - 2) + nfib(n - 1);
}

nuint_t rfact(nint_t n)
{
	if (n < 0) { errno = ERR_NBOUNDS; return -NINF; }
	if (n <= 1) { return 1; }
	
	return (nuint_t)n * nfact(n - 1);
}

nuint_t nfact(nint_t n)
{
	return rfact(n);
}

nuint_t ncomb(nint_t n, nint_t r)
{
	nuint_t ret = 0;
	if (n < 0) { errno = ERR_NBOUNDS; return -NINF; }
	if (r < 0) { errno = ERR_NBOUNDS; return -NINF; }

	ret = nfact(n) / (nfact(r) * nfact(n - r));

	return ret;
}

nuint_t nperm(nint_t n, nint_t r)
{
	nuint_t ret = 0;
	if (n < 0) { errno = ERR_NBOUNDS; return -NINF; }
	if (r < 0) { errno = ERR_NBOUNDS; return -NINF; }

	ret = nfact(n) / nfact(n - r);

	return ret;
}

ndouble_t nabs(ndouble_t n)
{
	if (n < 0)
	{
		return n * -1.0;
	}
	
	return n;
}

ndouble_t ndec(ndouble_t n)
{
	return (n - (nint_t)n);
}
// round to nearest integer
// halfs round to nearest even integer
nint_t nint(ndouble_t n)
{
	nint_t tenths = (nint_t)((n - (nint_t)n) * 10.0);
	ndouble_t ishalf = ((n - (nint_t)n) / 0.5);
	nint_t floor = (nint_t)n;

	if (ishalf == 1.0)
	{
		if ((floor % 2) == 1)
		{
			return floor + 1;
		} else {
			return floor;
		}
	} else {
		if (tenths > 5)
		{
			return floor + 1;
		} else {
			return floor;
		}
	}
}

ndouble_t nmod(ndouble_t a, ndouble_t n)
{
	return (a - (n * (nint_t)(a / n)));
}

ndouble_t nsin(ndouble_t n)
{
	nuint_t x, y;
	ndouble_t cur = 0, r = 1.0;
	nint_t i;

	ndouble_t ret = nmod(n, 2*NPI);
	if ((ret == NPI) || (ret == 0)) { return 0.0; }
	if (ret == NPI/2.0) { return 1.0; }
	if (ret == 3.0*NPI/2.0) { return -1.0; }

	if ((ret <= NPI) && (ret > (NPI / 2.0)))
	{
		ret = NPI - ret;
	} else if ((ret > NPI) && (ret <= (3.0 * NPI / 2.0))) {
		r = -1.0;
		ret -= NPI;
	} else if ((ret > (3.0 * NPI / 2.0))) {
		r = -1.0;
		ret = 2*NPI - ret;
	}
	
	x = DTOUINT64(NCORDIC);
	y = 0;
	
	for (i = 0; i < 28; i++)
	{
		nuint_t new;

		if (ret > cur)
		{
			new = x - (y >> i);
			y = (x >> i) + y;
			x = new;
			cur += NMATH_CORDIC_ANGLES[i];
		} else {
			new = x + (y >> i);
			y = y - (x >> i);
			x = new;
			cur -= NMATH_CORDIC_ANGLES[i];
		}
	}

	return (ndouble_t)(y / N32BITS) * r;
}

ndouble_t ncos(ndouble_t n)
{
	return nsin(n + (NPI / 2.0));
}

ndouble_t ntan(ndouble_t n)
{
	ndouble_t c = ncos(n);
	if (c != 0.0)
	{
		return (nsin(n) / c);
	}

	return 0.0;
}

ndouble_t nln(ndouble_t n)
{
	ndouble_t m, ret;
	nint_t k = (nint_t)(n / 10.0);

	nint_t i;
	nint_t consts[6] = {0,0,0,0,0,0};
	ndouble_t p;

	if (n <= 0.0) { errno = ERR_NRANGE; return -NINF; }

	if (n >= 10.0) { m = n / 10.0; } else { m = n; }

	p = m / 10.0;

	if ((p * 2) < 1)
	{
		consts[0] = 1;
		p *= 2;
	}

	for (i = 1; i < 6; i++)
	{
		while (((p / rpow(10, i)) + p) < 1)
		{
			p += (p / rpow(10, i));
			consts[i] += 1;
		}
	}

	ret = (1.0 - p);
	for (i = 0; i < 6; i++)
	{
		ret += consts[i] * NMATH_CORDIC_LOGS[i];
	}

	return (NLOG10 - ret) + k * NLOG10;
}

ndouble_t nlog(ndouble_t n)
{
	if (n <= 0) { errno = ERR_NRANGE; return -NINF; }
	return nln(n) / NLOG10;
}

ndouble_t nlg(ndouble_t n)
{
	if (n <= 0) { errno = ERR_NRANGE; return -NINF; }
	return nln(n) / NLOG2;
}

ndouble_t nexp(ndouble_t n)
{
	nint_t i, recip = 0;
	ndouble_t y = 1.0, z;
	if (n < 0) { recip = 1; n *= -1.0; }

	for (i = 0; i < 10; i++)
	{
		z = NMATH_CORDIC_LOGS[i];
		while (n >= z)
		{
			n -= z;
			y += (y / rpow(10, i));
		}
	}

	if (recip) { return (1.0 / y); }	
	return y;
}

ndouble_t npow(ndouble_t x, ndouble_t n)
{
	/* x^n = exp(n * ln(x)) */
	ndouble_t ret;
	if ((n < 1) && (x < 0))
	{
		// imaginary
		errno = ERR_NIMAG;
		return -NINF;
	}

	if (n < 0)
	{
		ret = nexp(-1.0*n * nln(x));
		return (1.0 / ret);
	}
	
	return nexp(n * nln(x));
}

ndouble_t nmin(ndouble_t a, ndouble_t b)
{
	if (a > b) { return b; }
	
	return a;
}

ndouble_t nmax(ndouble_t a, ndouble_t b)
{
	if (a < b) { return b; }

	return a;
}

ndouble_t nbern(nint_t n)
{
	if (n < 0) { errno = ERR_NBOUNDS; return -NINF; }
	
	nint_t k, j, r;
	ndouble_t ret = 0.0, inner;
	
	for (k = 0; k <= n; k++)
	{
		inner = 0.0;
		for (j = 0; j <= k; j++)
		{
			r = (j % 2 == 0 ? 1 : -1);
			inner += (ndouble_t)r * (ndouble_t)ncomb(k, j) * rpow(j, n);
		}
		ret += 1.0 / (k + 1.0) * inner;
	}
	
	return ret;
}

nint_t ngcd(nint_t a, nint_t b)
{
	if ((a < 0) || (b < 0)) { return 0; }

	if (b == 0) { return a; }
	else { return ngcd(b, a % b); }
}

nint_t nlcm(nint_t a, nint_t b)
{
	if ((a < 0) || (b < 0)) { return 0; }

	return ((a * b) / ngcd(a, b));
}

nint_t npoly(nint_t n, nint_t r)
{
	nint_t ret;

	if (n < 0) { errno = ERR_NBOUNDS; return -NINF; }

	ret = ncomb(n + r - 1, r);
	return ret;
}

nint_t ncat(nint_t n)
{
	nint_t ret;

	if (n < 0) { errno = ERR_NBOUNDS; return -NINF; }

	ret = (1.0 / ((ndouble_t)n + 1.0)) * ncomb(2 * n, n);
	return ret;
} 

nint_t nfloor(ndouble_t n)
{
	if (n < 0)
	{
		return ((nint_t)n - 1);
	} else {
		return (nint_t)n;
	}
}

nint_t nceil(ndouble_t n)
{
	if (n < 0)
	{
		return (nint_t)n;
	} else {
		return ((nint_t)n + 1);
	}
}

nint_t nsign(ndouble_t n)
{
	return (n < 0 ? -1 : 1);
}

ndouble_t nsqrt(ndouble_t n)
{
	if (n < 0)
	{
		errno = ERR_NIMAG;
		return -NINF;
	}

	return npow(n, 0.5);
}
