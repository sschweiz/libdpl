/**
	@file dsp.h
	@brief Digital Signal Processing Algorithms
	@author Stephen Schweizer <sschweizer@nokomisinc.com>

	Copyright (c) 2008-2009 Nokomis Inc., as an unpublished work.
	
	All rights reserved.  US GOVT RESTRICTED RIGHTS.  Proprietary Information
	of Nokomis Inc. Subject to Non-Disclosure and License Agreement.  Do Not
	Copy or Distribute.
*/

#ifndef __DPL_DSP_H__
#define __DPL_DSP_H__

#include "dpl_types.h"

#ifndef __dpl_float_defined
#define __dpl_float_defined
#ifdef DPL_PRECISION
typedef double dpl_fp_t;
#else
typedef float dpl_fp_t;
#endif
#endif

#undef DPL_WELLDEFINED

#define DPL_LENGTH		4096
#define DPL_PIf			3.1415926535897932384626433832795f
#define DPL_PI			((dpl_fp_t)3.1415926535897932384626433832795)
#define DPL_2PI			((dpl_fp_t)6.2831853071795864769252867665590)
#define DPL_SQRT2		((dpl_fp_t)1.4142135623730950488016887242097)
#define DPL_SQRTHALF		((dpl_fp_t)0.7071067811865475244008443621048)
#define DPL_NUMBINS		256
#define DPL_MAXPEAKS		8
#define DPL_SQR(x)		((dpl_fp_t)(x*x))


#define DPL_DSP_METHOD_MEANSTD		0
#define DPL_DSP_METHOD_MINMAX		1

#ifndef __dpl_bool_defined
#define __dpl_bool_defined
typedef unsigned char dpl_bool_t;
#endif

#define DPL_SAFE_CAST(x) ((dpl_fp_t)x)

/**
	@brief Bit reversal table
	
	Given some unsigned char of 8 bits \f$x\f$, let \f$x^{R}\f$ denote the reversal of its bits. Hence,
	@code
	xR = dd_bit_table[x]
	x = (xR)R = dd_bit_table[dd_bit_table[x]]
	@endcode
*/
extern const unsigned char dd_bit_table[256];

/** @brief Complex number type structure */
typedef struct __dpl_dsp_complex_t
{
	/** @brief real component */
	dpl_fp_t re;
	/** @brief imaginary component */
	dpl_fp_t im;
} dd_complex_t;

typedef dpl_fp_t[2] dpl_basic_complex_t;

/** @brief Peak finding helper structure */
typedef struct __dpl_dsp_foundpeak_t
{
	/** index location of a peak in a vector */
	int location;
	/** value of the element at index location */
	dpl_fp_t value;
} dd_foundpeak_t;

typedef struct __dpl_dsp_fft_t
{
	dd_complex_t *twiddle;
	dd_complex_t *scratch;
	unsigned short alloc;
	unsigned long size;
	dpl_bool_t forward;
} dd_fft_t;

/**
	@brief Calculates the local minimum and maximum of a given vector
	@param data	A pointer to the vector of type dpl_fp_t
	@param n	The length of the data vector
	@param min	Pointer to dpl_fp_t to hold the minimum value of data
	@param max	Pointer to dpl_fp_t to hold the maximum value of data
*/
void dd_minmax(const dpl_fp_t data[], unsigned long n, dpl_fp_t *min, dpl_fp_t *max);

/** 
	@brief Calculates the historgramic noisefloor of a given input vector
	@param data	A pointer to the vector of type dpl_fp_t
	@param n	The length of the data vector
*/
dpl_fp_t dd_noisefloor(const dpl_fp_t data[], unsigned long n);

/**
	@brief Convolves the vectors a and b placing their result in data
	
	For given complex vectors \f$a\f$ and \f$b\f$, the resultant vector has components
	\f[
		c_{i} = a_{i} b_{i}^{*}
	\f]
	where \f$*\f$ denotes the complex conjugate.
	@param data	A pointer to the vector of type dpl_fp_t
	@param n	The length of the data vector
	@param a	A pointer to the first input vector of type dpl_fp_t
	@param b	A pointer to the second input vector of type dpl_fp_t
*/
void dd_convolve(dd_complex_t data[], unsigned long n, dd_complex_t a[], dd_complex_t b[]);

/**
	@brief Safe method to return \f$\pi\f$
	@return 3.14159265... type cast appropriotly
*/
dpl_fp_t dd_pi();

/*@{*/
/**
	@brief Normalizes or Denormalizes a dataset.

	The dd_normalize function takes a vector of dpl_fp_t types of length n and
	given and input method normalizes the data. If the method is DPL_METHOD_MEANSTD
	it normalizes the dataset to have a mean of 0 and a standard deviation of 1.  If
	the DPL_METHOD_MINMAX is used the data set is normalized between -1 and 1.

	The dd_denormalize function essentially reverses the transformation.  It is
	important to note that these functions perform in-place operations so the input
	data is overwritten during transformation.  The purpose of denormalization is to
	incorporate the following symmetry relationship:

	Given an input vector \f$x\f$, the dd_normalize function returns an output vector
	\f$x'\f$ which is normalized between the ranges described above.  The purpose
	of dd_denormalize is for when, given a function which takes as its input a normalized
	data vector \f$y' = f\left(x'\right)\f$, you wish to calculate some non-normalized
	vector result.  That is, strictly speaking given an input vector \f$x\f$, a normalized
	tranformation function \f$y'_{i} = f\left(x'_{i}\right)\f$, and a resultant output
	vector \f$y\f$.  The process for using these functions would be:

	@code
	x' = dd_normalize(x)
	for each i in n
	    y'_i = f(x'_i)
	y = dd_denormalize(y')
	@endcode

	@param data A pointer to the vector of type dpl_fp_t
	@param n The length of the data vector
	@param method The normalization method (either DPL_METHOD_MINMAX or DPL_METHOD_MEANSTD)
*/
void dd_normalize(dpl_fp_t data[], unsigned long n, unsigned int method);
void dd_denormalize(dpl_fp_t data[], unsigned long n, unsigned int method);
/*@}*/

/* complex number prototypes */
/*@{*/
/**
	@brief Create a complex vector from a given real vector
	@param data A pointer to the vector of type specified in function call
	@param n The length of the data vector
	@param ret A pointer to the resulting complex vector
*/
void dd_complex_from_char(const char data[], unsigned long n, dd_complex_t ret[]);
void dd_complex_from_short(const short int data[], unsigned long n, dd_complex_t ret[]);
void dd_complex_from_int(const int data[], unsigned long n, dd_complex_t ret[]);
void dd_complex_from_dpl_fp_t(const dpl_fp_t data[], unsigned long n, dd_complex_t ret[]);
/*@}*/


void dd_complex_create_with(const dpl_fp_t real, const dpl_fp_t imag, dd_complex_t *c);
dd_complex_t dd_complex_create_from(const dpl_fp_t real, const dpl_fp_t imag);

void dd_spectrum(dd_complex_t data[], unsigned long n, dpl_fp_t output[]);
void dd_spectrum_ip(dd_complex_t data[], unsigned long n);

/**
	@brief Calculates the complex conjugate of of a given complex number
	@param c	Complex number input
	@return		Complex conjugate of c
*/
dd_complex_t dd_complex_conjugate(dd_complex_t c);

/*@{*/
/**
	@brief Performs the requested mathematical operations on two complex numnbers
	@param a	First input number
	@param b	Second input number
	@return Result of a op b where op is the operation defined in the function call
*/
dd_complex_t dd_complex_multiply(dd_complex_t a, dd_complex_t b);
dd_complex_t dd_complex_subtract(dd_complex_t a, dd_complex_t b);
dd_complex_t dd_complex_add(dd_complex_t a, dd_complex_t b);
/*@}*/
	
/**
	@brief Calculates the magnitude of the of a complex number
	@param c Complex number input
	@return Magnitude of c
*/
dpl_fp_t dd_complex_magnitude(dd_complex_t c) /* __attribute__ ((pure)) */;

/**
	@brief Generates a complex number from a given r and theta value in polar coordinates
	
	Given a polar representation of a point, the analogous point in the complex plane is returned.
	This position is given by the relationship \f$r e^{i \theta} = r \cos(\theta) + r i \sin(\theta)\f$
	@param r Radius as a dpl_fp_t
	@param theta Theta in radians as a dpl_fp_t
	@return Complex number generated from the inputs
*/
dd_complex_t dd_complex_from_polar(dpl_fp_t r, dpl_fp_t theta);

/**
	@brief A Forward CDF 9/7 Discrete wavelet transform
	
	Performs a forward, inplace, discrete Cohen-Daubechies-Feauveau wavelet transform with
	lifting implementation in O(n) time.
	@param data	A pointer to the vector of type dpl_fp_t
	@param n	The length of the data vector
*/
void dd_dwt(dpl_fp_t data[], unsigned long n);
/**
	@brief An Inverse CDF 9/7 Discrete wavelet transform
	
	Performs an inverse, inplace, discrete Cohen-Daubechies-Feauveau wavelet transform with
	lifting implementation in O(n) time.
	@param data	A pointer to the vector of type dpl_fp_t
	@param n	The length of the data vector
*/
void dd_idwt(dpl_fp_t data[], unsigned long n);

/* in-place and out-of-place wavelet denoising */
/*@{*/
/**
	@brief In-place and out-of-place wavelet denoising functions
	
	Performs in-place (dsp_ip_wvlt_denoising) and out-of-place (dsp_of_wvlt_denoising)
	on a given vector.
	@param data	A pointer to the vector of type dpl_fp_t
	@param res	A pointer to the output vector if using out-of-place denoising of type dpl_fp_t
	@param n	The length of the data vector
	@param level	The level of wavelet denoising to perform
*/
void dd_op_wvlt_denoising(const dpl_fp_t data[], dpl_fp_t res[], unsigned long n, int level);
void dd_ip_wvlt_denoising(dpl_fp_t data[], unsigned long n, int level);
/*@}*/

/* based on algorithm by Timothy Hsu <thsu@nokomisinc.com> */
/**
	@brief Peak finding algorithm
	
	Statistical peak-finding algorithm proposed by Timothy Hsu <thsu@nokomisinc.com>.
	@param data	A pointer to the vector of type dpl_fp_t
	@param n	The length of the data vector
	@param nf	The noisefloor of the vector data
	@param peaks	A pointer to an array of structures which hold peak information
	@return Number of peaks found
*/
int dd_peak(const dpl_fp_t data[], unsigned long n, dpl_fp_t nf, dd_foundpeak_t *peaks);

void dd_fft_r2c(dd_fft_t *f, dpl_fp_t in[], dd_complex_t out[]);
void dd_fft_c2c(dd_fft_t *f, dd_complex_t in[], dd_complex_t out[]);
void dd_fft_c2c_inplace(dd_fft_t *f, dd_complex_t data[]);
void dd_fft_dealloc(dd_fft_t *f) __dsp_nonnull;
void dd_fft_alloc(dd_fft_t *f, unsigned long n, dpl_bool_t forward) __dsp_nonnull;

/**
	@brief Calculates the mean of a given vector
	
	Specifically, given an input vector \f$x\f$
	\f[
		\mu = \sum_{i=0}^{n-1} x_{i}
	\f]
	@param data	A pointer to the vector of type dpl_fp_t
	@param n	The length of the data vector
	@return Mean of vector data
*/
dpl_fp_t dd_math_mean(const dpl_fp_t data[], unsigned long n);

/**
	@brief Calculates the standard deviation of a given vector
	
	Specifically, given an input vector \f$x\f$ and its mean \f$\mu\f$
	\f[
		\sigma = \sqrt{\frac{1}{n} \sum_{i=0}^{n-1} \left(x_{i}-\mu\right)^{2}}
	\f]
	@param data	A pointer to the vector of type dpl_fp_t
	@param n	The length of the data vector
	@param mean	Statistical mean of the vector data
	@return Standard deviation of vector data
*/
dpl_fp_t dd_math_stddev(const dpl_fp_t data[], unsigned long n, dpl_fp_t mean);

/**
	@brief Calculates the sample skewness (third standardized moment) of a given vector
	
	Specifically, given an input vector \f$x\f$, its mean \f$\mu\f$, and its standard deviation \f$\sigma\f$
	\f[
		\gamma = \frac{\frac{1}{n} \sum_{i=0}^{n-1} \left(x_{i}-\mu\right)^{3}}{\sigma^{3}}
	\f]
	@param data	A pointer to the vector of type dpl_fp_t
	@param n	The length of the data vector
	@param mean	Statistical mean of the vector data
	@param stdd Standard deviation of the vector data
	@return Skewness of vector data
*/
dpl_fp_t dd_math_skew(const dpl_fp_t data[], unsigned long n, dpl_fp_t mean, dpl_fp_t stdd);

/* mask is a buffer of size n */
void dd_smooth_gaussian(dpl_fp_t data[], dpl_fp_t mask[], unsigned long n);
void dd_smooth_boxcar(dpl_fp_t data[], unsigned long n, int nbox);
void dd_smooth_hanning(dpl_fp_t data[], unsigned long n);


#endif /* __DPL_DSP_H__ */
