#include "dsp.h"
#include <string.h> /* for memcpy */
#include <stdlib.h> /* for malloc & free */

#include <stdio.h>

#include <math.h> /* sin, cos, sqrt, exp */

const unsigned char dsp_bit_table[256] = {
	0x00, 0x80, 0x40, 0xc0, 0x20, 0xa0, 0x60, 0xe0, 0x10, 0x90, 0x50, 0xd0, 0x30, 0xb0, 0x70, 0xf0,
	0x08, 0x88, 0x48, 0xc8, 0x28, 0xa8, 0x68, 0xe8, 0x18, 0x98, 0x58, 0xd8, 0x38, 0xb8, 0x78, 0xf8,
	0x04, 0x84, 0x44, 0xc4, 0x24, 0xa4, 0x64, 0xe4, 0x14, 0x94, 0x54, 0xd4, 0x34, 0xb4, 0x74, 0xf4,
	0x0c, 0x8c, 0x4c, 0xcc, 0x2c, 0xac, 0x6c, 0xec, 0x1c, 0x9c, 0x5c, 0xdc, 0x3c, 0xbc, 0x7c, 0xfc,
	0x02, 0x82, 0x42, 0xc2, 0x22, 0xa2, 0x62, 0xe2, 0x12, 0x92, 0x52, 0xd2, 0x32, 0xb2, 0x72, 0xf2,
	0x0a, 0x8a, 0x4a, 0xca, 0x2a, 0xaa, 0x6a, 0xea, 0x1a, 0x9a, 0x5a, 0xda, 0x3a, 0xba, 0x7a, 0xfa,
	0x06, 0x86, 0x46, 0xc6, 0x26, 0xa6, 0x66, 0xe6, 0x16, 0x96, 0x56, 0xd6, 0x36, 0xb6, 0x76, 0xf6,
	0x0e, 0x8e, 0x4e, 0xce, 0x2e, 0xae, 0x6e, 0xee, 0x1e, 0x9e, 0x5e, 0xde, 0x3e, 0xbe, 0x7e, 0xfe,
	0x01, 0x81, 0x41, 0xc1, 0x21, 0xa1, 0x61, 0xe1, 0x11, 0x91, 0x51, 0xd1, 0x31, 0xb1, 0x71, 0xf1,
	0x09, 0x89, 0x49, 0xc9, 0x29, 0xa9, 0x69, 0xe9, 0x19, 0x99, 0x59, 0xd9, 0x39, 0xb9, 0x79, 0xf9,
	0x05, 0x85, 0x45, 0xc5, 0x25, 0xa5, 0x65, 0xe5, 0x15, 0x95, 0x55, 0xd5, 0x35, 0xb5, 0x75, 0xf5,
	0x0d, 0x8d, 0x4d, 0xcd, 0x2d, 0xad, 0x6d, 0xed, 0x1d, 0x9d, 0x5d, 0xdd, 0x3d, 0xbd, 0x7d, 0xfd,
	0x03, 0x83, 0x43, 0xc3, 0x23, 0xa3, 0x63, 0xe3, 0x13, 0x93, 0x53, 0xd3, 0x33, 0xb3, 0x73, 0xf3,
	0x0b, 0x8b, 0x4b, 0xcb, 0x2b, 0xab, 0x6b, 0xeb, 0x1b, 0x9b, 0x5b, 0xdb, 0x3b, 0xbb, 0x7b, 0xfb,
	0x07, 0x87, 0x47, 0xc7, 0x27, 0xa7, 0x67, 0xe7, 0x17, 0x97, 0x57, 0xd7, 0x37, 0xb7, 0x77, 0xf7,
	0x0f, 0x8f, 0x4f, 0xcf, 0x2f, 0xaf, 0x6f, 0xef, 0x1f, 0x9f, 0x5f, 0xdf, 0x3f, 0xbf, 0x7f, 0xff
};

static void __dsp_internal __bubblesort_peaks(foundpeak_t peaks[], int npeaks)
{
	int i, j;
	foundpeak_t buf;

	for (i = 0; i < npeaks; i++)
	{
		for (j = i+1; j < npeaks; j++)
		{
			if (peaks[j].value > peaks[i].value)
			{
				buf.value = peaks[i].value;
				buf.location = peaks[i].location;

				/* swap i, j */
				peaks[i].value = peaks[j].value;
				peaks[i].location = peaks[j].location;
				peaks[j].value = buf.value;
				peaks[j].location = buf.location;
			}
		}
	}
}

static dsp_fp_t __dsp_internal __threshold(const dsp_fp_t data[], unsigned long n)
{
	dsp_fp_t range, mx, mn, mean, meantri, std;
	dsp_fp_t skew, skewtri, tmp0, tmp1, tmp2;
	unsigned long i;
	int bincnt[DSP_NUMBINS], bin;

	dsp_minmax(data, n, &mn, &mx);
	std = dsp_math_stddev(data, n, dsp_math_mean(data, n));
	
	range = ((mx - mn) / (dsp_fp_t)DSP_NUMBINS);

	for (i = 0; i < DSP_NUMBINS; i++) { bincnt[i] = 0; }

	for (i = 0; i < n; i++)
	{
		bin = (int)((data[i] - mn) / range);
		if (bin < 0) { bin = 0; }
		if (bin > 255) { bin = 255; }
		
		bincnt[bin]++;
	}

	/* calculate various statistical means */
	mean = 0;
	meantri = 0;
	for (i = 0; i < DSP_NUMBINS; i++)
	{
		tmp0 = mn + ((dsp_fp_t)i + 0.5f) * ((mx - mn) / (dsp_fp_t)DSP_NUMBINS);
		mean += tmp0 * ((dsp_fp_t)bincnt[i] / (dsp_fp_t)n);
		meantri += tmp0 * (((dsp_fp_t)DSP_NUMBINS - (dsp_fp_t)i) / (dsp_fp_t)DSP_NUMBINS);
	}

	/* calculate respective skewnesses */
	for (i = 0; i < DSP_NUMBINS; i++)
	{
		tmp0 = mn + ((dsp_fp_t)i + 0.5f) * ((mx - mn) / (dsp_fp_t)DSP_NUMBINS);
		tmp1 = tmp0 - mean;
		tmp2 = tmp0 - meantri;
		tmp1 *= tmp1*tmp1;
		tmp2 *= tmp2*tmp2;

		skew = tmp1 * ((dsp_fp_t)bincnt[i] / (dsp_fp_t)n);
		skewtri = tmp2 * (((dsp_fp_t)DSP_NUMBINS - (dsp_fp_t)i) / (dsp_fp_t)DSP_NUMBINS);
	}

	/* calculate threshold */
	tmp0 = skew / skewtri;
	tmp1 = (1.0f - tmp0) * std;
	tmp1 = fabs(tmp1);
	tmp2 = mean + 1.6f*(std + tmp1); /* use nf instead of mean here probably */
	
	return tmp2;
}

/*
	calculates the minimum and maximum values of data[1..n] and stores
	them in min and max respectively
*/
void dsp_minmax(const dsp_fp_t data[], unsigned long n, dsp_fp_t *min, dsp_fp_t *max)
{
	dsp_fp_t mn, mx;
	unsigned long i;
	
	mn = data[0];
	mx = data[0];
	
	for (i = 0; i < n; i++)
	{
		if (data[i] > mx) { mx = data[i]; }
		if (data[i] < mn) { mn = data[i]; }
	}
	
	*min = mn;
	*max = mx;
}

/*
	returns the noise floor for the spectrum contained in data[1..n]
*/
dsp_fp_t dsp_noisefloor(const dsp_fp_t data[], unsigned long n)
{
	dsp_fp_t binave[256];
	int bincnt[256];
	int bigbin = 0;
	int bin;
	unsigned long i;
	dsp_fp_t range, mn, mx;
	
	dsp_minmax(data, n, &mn, &mx);
	range = ((mx - mn) / 256.0f);
	
	for (i = 0; i < 256; i++) { bincnt[i] = 0; binave[i] = 0.0f; }
	
	for (i = 0; i < n; i++)
	{
		bin = (int)((data[i] - mn) / range);
		if (bin < 0) { bin = 0; }
		if (bin > 255) { bin = 255; }
		
		bincnt[bin]++;
		binave[bin] += data[i];
		
		if (bincnt[bin] > bincnt[bigbin]) { bigbin = bin; }
	}
	
	return (binave[bigbin] / (dsp_fp_t)bincnt[bigbin]);
}

/*
	multiplies a by the complex conjugate of b and puts the result in data
	a, b, and data are all complex structs of size n
*/
void dsp_convolve(complex_t data[], unsigned long n, complex_t a[], complex_t b[])
{
	unsigned long i;
	complex_t buf;

	for (i = 0; i < n; i++)
	{
		buf.re = a[i].re*b[i].re + a[i].im*b[i].im;
		buf.im = a[i].im*b[i].re - a[i].re*b[i].im;

		data[i].re = buf.re;
		data[i].im = buf.im;
	}
}

dsp_fp_t dsp_pi()
{
#ifdef DSP_PRECISION
	return DSP_PI;
#else
	return DSP_PIf;
#endif
}

/*
	takes an array of data[1..n] of format RIRIRI...  and puts it into
	a complex_t struct of length [1..n/2]
*/
void dsp_complex_from_char(const char data[], unsigned long n, complex_t ret[])
{
	unsigned long i;

	for (i = 0; i < n; i+=2)
	{
		ret[i/2].re = (dsp_fp_t)data[i];
		ret[i/2].im = (dsp_fp_t)data[i+1];
	}
}

void dsp_complex_from_short(const short int data[], unsigned long n, complex_t ret[])
{
	unsigned long i;

	for (i = 0; i < n; i+=2)
	{
		ret[i/2].re = (dsp_fp_t)data[i];
		ret[i/2].im = (dsp_fp_t)data[i+1];
	}
}

void dsp_complex_from_int(const int data[], unsigned long n, complex_t ret[])
{
	unsigned long i;

	for (i = 0; i < n; i+=2)
	{
		ret[i/2].re = (dsp_fp_t)data[i];
		ret[i/2].im = (dsp_fp_t)data[i+1];
	}
}

void dsp_complex_from_dsp_fp_t(const dsp_fp_t data[], unsigned long n, complex_t ret[])
{
	unsigned long i;

	for (i = 0; i < n; i+=2)
	{
		ret[i/2].re = data[i];
		ret[i/2].im = data[i+1];
	}
}

void dsp_complex_create_with(const dsp_fp_t real, const dsp_fp_t imag, complex_t *c)
{
	c->re = real;
	c->im = imag;
}

complex_t dsp_complex_create_from(const dsp_fp_t real, const dsp_fp_t imag)
{
	complex_t c;

	c.re = real;
	c.im = imag;

	return c;
}

complex_t dsp_complex_from_polar(dsp_fp_t r, dsp_fp_t theta)
{
	complex_t ret;
	ret.re = r * cos(theta);
	ret.im = r * sin(theta);
	
	return ret;
}

dsp_fp_t dsp_complex_magnitude(complex_t c)
{
	//return sqrt(DSP_SQR(c.re) + DSP_SQR(c.im));
	return DSP_SQR(c.re) + DSP_SQR(c.im);
}

complex_t dsp_complex_add(complex_t a, complex_t b)
{
	complex_t ret;

	ret.re = a.re + b.re;
	ret.im = a.im + b.im;

	return ret;
}

complex_t dsp_complex_subtract(complex_t a, complex_t b)
{
	complex_t ret;

	ret.re = a.re - b.re;
	ret.im = a.im - b.im;

	return ret;
}

complex_t dsp_complex_multiply(complex_t a, complex_t b)
{
	complex_t ret;

	ret.re = (a.re*b.re - a.im*b.im);
	ret.im = (a.re*b.im + a.im*b.re);

	return ret;
}

complex_t dsp_complex_conjugate(complex_t c)
{
	complex_t ret;

	ret.re = c.re;
	ret.im = c.im * -1.0f;

	return ret;
}

void dsp_spectrum(complex_t data[], unsigned long n, dsp_fp_t output[])
{
	unsigned long i;

	for (i = 0; i < n; i++)
	{
		output[i] = DSP_SAFE_CAST(10.0) * log10(dsp_complex_magnitude(data[i]));
	}
}

void dsp_spectrum_ip(complex_t data[], unsigned long n)
{
	unsigned long i;

	for (i = 0; i < n; i++)
	{
		data[i].re = DSP_SAFE_CAST(10) * log10(dsp_complex_magnitude(data[i]));
		data[i].im = DSP_SAFE_CAST(0);
	}
}

void dsp_dwt(dsp_fp_t data[], unsigned long n)
{
	dsp_fp_t a;
#ifdef DSP_WELLDEFINED
	dsp_fp_t tempbank[DSP_LENGTH];
#else
	dsp_fp_t *tempbank;
#endif
	unsigned long i;
	
	
#ifndef DSP_WELLDEFINED
	tempbank = (dsp_fp_t *)malloc(sizeof(dsp_fp_t) * n);
	if (tempbank == NULL) { return; }
#endif

	// Predict 1
	a=-1.586134342f;
	for (i=1;i<n-2;i+=2)
	{
		data[i]+=a*(data[i-1]+data[i+1]);
	} 
	data[n-1]+=2*a*data[n-2];

	// Update 1
	a=-0.05298011854f;
	for (i=2;i<n;i+=2)
	{
		data[i]+=a*(data[i-1]+data[i+1]);
	}
	data[0]+=2*a*data[1];

	// Predict 2
	a=0.8829110762f;
	for (i=1;i<n-2;i+=2)
	{
		data[i]+=a*(data[i-1]+data[i+1]);
	}
	data[n-1]+=2*a*data[n-2];

	// Update 2
	a=0.4435068522f;
	for (i=2;i<n;i+=2)
	{
		data[i]+=a*(data[i-1]+data[i+1]);
	}
	data[0]+=2*a*data[1];

	// Scale
	a=1/1.149604398f;
	for (i=0;i<n;i++)
	{
		if (i%2) { data[i]*=a; }
		else { data[i]/=a; }
	}
	
	for (i=0;i<n;i++)
	{
		if (i%2==0) { tempbank[i/2]=data[i]; }
		else { tempbank[n/2+i/2]=data[i]; }
	}

	memcpy(data, tempbank, sizeof(dsp_fp_t)*n);
	
#ifndef DSP_WELLDEFINED
	free(tempbank);
#endif
}

void dsp_idwt(dsp_fp_t data[], unsigned long n)
{
	dsp_fp_t a;
#ifdef DSP_WELLDEFINED
	dsp_fp_t tempbank[DSP_LENGTH];
#else
	dsp_fp_t *tempbank;
#endif
	unsigned long i;
	
	
#ifndef DSP_WELLDEFINED
	tempbank = (dsp_fp_t *)malloc(sizeof(dsp_fp_t) * n);
	if (tempbank == NULL) { return; }
#endif

	// Unpack
	for (i=0;i<n/2;i++)
	{
		tempbank[i*2]=data[i];
		tempbank[i*2+1]=data[i+n/2];
	}
	memcpy(data, tempbank, sizeof(dsp_fp_t)*n);

	// Undo scale
	a=1.149604398f;
	for (i=0;i<n;i++)
	{
		if (i%2) { data[i]*=a;}
		else { data[i]/=a; }
	}

	// Undo update 2
	a=-0.4435068522f;
	for (i=2;i<n;i+=2)
	{
		data[i]+=a*(data[i-1]+data[i+1]);
	}
	data[0]+=2*a*data[1];

	// Undo predict 2
	a=-0.8829110762f;
	for (i=1;i<n-2;i+=2)
	{
		data[i]+=a*(data[i-1]+data[i+1]);
	}
	data[n-1]+=2*a*data[n-2];

	// Undo update 1
	a=0.05298011854f;
	for (i=2;i<n;i+=2)
	{
		data[i]+=a*(data[i-1]+data[i+1]);
	}
	data[0]+=2*a*data[1];

	// Undo predict 1
	a=1.586134342f;
	for (i=1;i<n-2;i+=2)
	{
		data[i]+=a*(data[i-1]+data[i+1]);
	} 
	data[n-1]+=2*a*data[n-2];
	
#ifndef DSP_WELLDEFINED
	free(tempbank);
#endif
}

void dsp_op_wvlt_denoising(const dsp_fp_t data[], dsp_fp_t res[], unsigned long n, int level)
{
	unsigned long i;
	dsp_fp_t *nn = (dsp_fp_t *)malloc(sizeof(dsp_fp_t) * n);

	memcpy(nn, data, sizeof(dsp_fp_t)*n);

	if (level < 1) { level = 1; }

	for (i = 0; i < level; i++)
	{
		if (i > 0)
		{
			memset(nn+(n/(1<<i)), 0, sizeof(dsp_fp_t)*(n/(1<<i)));
		}
		dsp_dwt(nn, n / (1 << i));
	}

	memset(nn+(n/(1<<level)), 0, sizeof(dsp_fp_t)*(n/(1<<level)));

	for (i = level; i > 0; i--)
	{
		dsp_idwt(nn, n / (1 << (i-1)));
	}

	memcpy(res, nn, sizeof(dsp_fp_t)*n);
	free(nn);
}

void dsp_ip_wvlt_denoising(dsp_fp_t data[], unsigned long n, int level)
{
	unsigned long i;

	if (level < 1) { level = 1; }

	for (i = 0; i < level; i++)
	{
		if (i > 0)
		{
			memset(data+(n/(1<<i)), 0, sizeof(dsp_fp_t)*(n/(1<<i)));
		}
		dsp_dwt(data, n / (1 << i));
	}

	memset(data+(n/(1<<level)), 0, sizeof(dsp_fp_t)*(n/(1<<level)));

	for (i = level; i > 0; i--)
	{
		dsp_idwt(data, n / (1 << (i-1)));
	}
}

/* O(n) */
dsp_fp_t dsp_math_mean(const dsp_fp_t data[], unsigned long n)
{
	unsigned long i;
	dsp_fp_t ret = 0;

	for (i = 0; i < n; i++)
	{
		ret += data[i];
	}

	return ret / (dsp_fp_t)n;
}

/* O(n) */
dsp_fp_t dsp_math_stddev(const dsp_fp_t data[], unsigned long n, dsp_fp_t mean)
{
	unsigned long i;
	dsp_fp_t ret = 0, buf;
	dsp_fp_t st;

	for (i = 0; i < n; i++)
	{
		buf = data[i] - mean;
		ret += buf*buf;
	}

	st = sqrt(ret / (dsp_fp_t)n);
	return st;
}

/* O(n) */
dsp_fp_t dsp_math_skew(const dsp_fp_t data[], unsigned long n, dsp_fp_t mean, dsp_fp_t stdd)
{
	unsigned long i;
	dsp_fp_t ret = 0, buf;
	
	for (i = 0; i < n; i++)
	{
		buf = data[i] - mean;
		ret += buf*buf*buf;
	}
	
	ret /= (dsp_fp_t)n;
	ret /= stdd*stdd*stdd;
	
	return ret;
}

/*
	returns number of peaks found or -1 if more than DSP_MAXPEAKS were found
	or -2 on error

	inputs:
		data - array of spectrum data
		n - size of array data
		nf - noisefloor
	outputs:
		peakvalues - array of size DSP_MAXPEAKS containing peak values
		peaklocations - array of size DSP_MAXPEAKS containing peak locations
*/
int dsp_peak(const dsp_fp_t data[], unsigned long n, dsp_fp_t nf, foundpeak_t *peaks)
{
	dsp_fp_t thsld;
	const int offset = 64;
	unsigned char *loc;
	int ret = 0;
	int st = 0, maxi = 0;
	unsigned long i;

	thsld = __threshold(data, n);
	loc = (unsigned char *)malloc(sizeof(unsigned char) * n);

	if (loc == NULL) { return -2; } /* malloc failed */

	/* we keep only the first 8 peaks found, if we found less than
	   8 peaks we know the largest peak is in this set, if we find
	   more than 8, then we determine the data set is noise. */
	for (i = offset; i < n; i++)
	{
		if (data[i] > thsld) /* strictly greater? */
		{
			loc[i] = 1;
		} else {
			loc[i] = 0;
		}
	}

	/* remove peaks inside of another's threshold */
	for (i = offset; i < n; i++)
	{
		if ((data[i] > nf) && (st == 0))
		{
			st = 1;
			maxi = i;
			continue;
		}

		if ((st == 1) && (loc[i] == 1) && (data[i] > data[maxi]))
		{
			loc[maxi] = 0;
			maxi = i;
		} else {
			loc[i] = 0;
		}

		if ((data[i] <= nf) && (st == 1))
		{
			st = 0;
		}
	}

	/* create the peak array */
	for (i = offset; i < n; i++)
	{
		if (loc[i] == 1)
		{
			if (ret < DSP_MAXPEAKS)
			{
				peaks[ret].location = i;
				peaks[ret].value = data[i];
				ret++;
			} else {
				/* found too many peaks */
				ret = -1;
				break;
			}
		}
	}
	
	if (ret > 0)
	{
		__bubblesort_peaks(peaks, ret);
	}

	free(loc);

	return ret;
}

void dsp_normalize(dsp_fp_t data[], unsigned long n, unsigned int method)
{
	unsigned long i;
	dsp_fp_t var1, var2;

	switch (method)
	{
	case DSP_METHOD_MINMAX:
		dsp_minmax(data, n, &var1, &var2);
		for (i = 0; i < n; i++)
		{
			data[i] = DSP_SAFE_CAST(2) * ((data[i] - var1) / (var2 - var1)) - DSP_SAFE_CAST(1);
		}
		break;
	case DSP_METHOD_MEANSTD:
		var1 = dsp_math_mean(data, n);
		var2 = dsp_math_stddev(data, n, var1);
		for (i = 0; i < n; i++)
		{
			data[i] = (data[i] - var1) / var2;
		}
		break;
	}
}

void dsp_denormalize(dsp_fp_t data[], unsigned long n, unsigned int method)
{
	unsigned long i;
	dsp_fp_t var1, var2;

	switch (method)
	{
	case DSP_METHOD_MINMAX:
		dsp_minmax(data, n, &var1, &var2);
		for (i = 0; i < n; i++)
		{
			data[i] = DSP_SAFE_CAST(0.5)*(data[i] + DSP_SAFE_CAST(1))*(var2 - var1) + var1;
		}
		break;
	case DSP_METHOD_MEANSTD:
		var1 = dsp_math_mean(data, n);
		var2 = dsp_math_stddev(data, n, var1);
		for (i = 0; i < n; i++)
		{
			data[i] = var1 + var2*data[i];
		}
		break;
	}
}

void dsp_smooth_hanning(dsp_fp_t data[], unsigned long n)
{
	unsigned long i;

	for (i = 1; i < n-1; i++)
	{
		data[i] = 0.25f*data[i-1] + 0.5f*data[i] + 0.25f*data[i+1];
	}
}

void dsp_smooth_boxcar(dsp_fp_t data[], unsigned long n, int nbox)
{
	unsigned long i;
	dsp_fp_t sum = 0;
	int pttsld, j, cnt = 0;

	if (nbox % 2 != 1) { nbox++; }

	if (nbox > 1)
	{
		pttsld = nbox / 2;

		for (i = 0; i < n; i++)
		{
			if ((i > pttsld) && (i < n-pttsld))
			{
				sum  = 0;
				for (j = 0; j < nbox; j++)
				{
					sum += data[i+j-pttsld];
				}
				data[i] = sum / (dsp_fp_t)nbox;
			}
		}
	}
}

void dsp_smooth_gaussian(dsp_fp_t data[], dsp_fp_t mask[], unsigned long n)
{
	dsp_fp_t sigma, sum, mu, buf, coef;
	dsp_fp_t *temp = (dsp_fp_t *)malloc(sizeof(dsp_fp_t) * n);
	unsigned long i, j;

	mu = dsp_math_mean(data, n);
	sigma = dsp_math_stddev(data, n, mu);

#ifdef DSP_PRECISION
	coef = 1.0f / (sqrt(2.0f * DSP_PI) * sigma);
#else
	coef = 1.0f / (sqrt(2.0f * DSP_PIf) * sigma);
#endif

	for (i = 0; i < n; i++)
	{
		sum = 0;
		for (j = 0; j < n; j++)
		{
			/* don't bother outside 6 sigma threshold */
			/* to speed up O(n^2) algorithm */
			if ((j > (i - 6*sigma)) && (j < (i + 6*sigma)))
			{
				buf = -1.0f * ((dsp_fp_t)j - i) * ((dsp_fp_t)j - i);
				buf /= 2.0f * sigma * sigma;
				mask[j] = coef * exp(buf);
				sum += mask[j] * data[j];
			}
		}

		temp[i] = sum;
	}

	for (i = 0; i < n; i++)
	{
		data[i] = temp[i];
	}

	free(temp);
}

static void __dsp_internal __dsp_fft_complex_calculate(complex_t *x, unsigned long N, int skip,
	complex_t *X, complex_t *D, complex_t *twiddles)
{
	complex_t *E = D + N/2;
	//complex_t R;
	int k;

	if (N == 1)
	{
		X[0] = x[0];
		return;
	}

	__dsp_fft_complex_calculate(x, N/2, skip*2, E, X, twiddles);
	__dsp_fft_complex_calculate(x+skip, N/2, skip*2, D, X, twiddles);

	for (k = 0; k < N/2; k++)
	{
		D[k] = dsp_complex_multiply(twiddles[k*skip], D[k]);
	}
	
	for (k = 0; k < N/2; k++)
	{
		X[k] = dsp_complex_add(E[k], D[k]);
		X[k+N/2] = dsp_complex_subtract(E[k], D[k]);
	}
}

static void __dsp_internal __dsp_fft_complex_calculate_norec(complex_t *x, unsigned long N,
		complex_t *X, complex_t *scratch, complex_t *twiddles)
{
	int k, m, n;
	int skip;
	dsp_bool_t evenI = (dsp_bool_t)(N & 0x55555555L);

	complex_t *E, *D;
	complex_t *Xp, *Xstart;

	if (N == 1)
	{
		X[0] = x[0];
		return;
	}

	E = x;

	for (n = 1; n < N; n *= 2)
	{
		Xstart = evenI ? scratch : X;
		skip = N / (2 * n);

		Xp = Xstart;
		for (k = 0; k != n; k++)
		{
			for (m = 0; m != skip; ++m)
			{
				D = E + skip;
				*D = dsp_complex_multiply(twiddles[k*skip], *D);
				*Xp = dsp_complex_add(*E, *D);
				Xp[N/2] = dsp_complex_subtract(*E, *D);
				++Xp;
				++E;
			}
			E += skip;
		}
		E = Xstart;
		evenI = !evenI;
	}
}

static void __dsp_internal __dsp_get_twiddles(dsp_fft_t *f, unsigned long N)
{
	int k;

#ifdef DSP_PRECISION
	dsp_fp_t sign = f->forward ? -1.0 : 1.0;
#else
	dsp_fp_t sign = f->forward ? -1.0f : 1.0f;
#endif

	f->twiddle = (complex_t *)malloc(sizeof(complex_t)*N/2);


	for (k = 0; k != N/2; k++)
	{
#ifdef DSP_PRECISION
		f->twiddle[k] = dsp_complex_from_polar(1.0, sign*2.0*DSP_PI*(dsp_fp_t)k/(dsp_fp_t)N);
#else
		f->twiddle[k] = dsp_complex_from_polar(1.0f, sign*2.0f*DSP_PIf*(dsp_fp_t)k/(dsp_fp_t)N);
#endif
	}
}

void dsp_fft_alloc(dsp_fft_t *f, unsigned long n, dsp_bool_t forward)
{
	__dsp_get_twiddles(f, n);
	f->scratch = (complex_t *)malloc(sizeof(complex_t) * n);
	f->alloc = 1;
	f->size = n;
	f->forward = forward;
}

void dsp_fft_dealloc(dsp_fft_t *f)
{
	if (f->alloc)
	{
		free(f->twiddle);
		free(f->scratch);
	}

	f->alloc = 0;
}

/* out can also be in for inline computation */
void dsp_fft_c2c(dsp_fft_t *f, complex_t in[], complex_t out[])
{
	if (f->alloc)
	{
		__dsp_fft_complex_calculate_norec(in, f->size, out, f->scratch, f->twiddle);
	}
}

void dsp_fft_c2c_inplace(dsp_fft_t *f, complex_t data[])
{
	if (f->alloc)
	{
		__dsp_fft_complex_calculate_norec(data, f->size, data, f->scratch, f->twiddle);
	}
}

void dsp_fft_r2c_norec(dsp_fft_t *f, dsp_fp_t in[], complex_t out[])
{
	unsigned long i;

	for (i = 0; i < f->size; i++)
	{
		out[i].re = in[i];
		out[i].im = DSP_SAFE_CAST(0);
	}

	__dsp_fft_complex_calculate_norec(out, f->size, out, f->scratch, f->twiddle);
}

void dsp_fft_r2c(dsp_fft_t *f, dsp_fp_t in[], complex_t out[])
{
	unsigned long i;

	for (i = 0; i < f->size; i++)
	{
		out[i].re = in[i];
		out[i].im = DSP_SAFE_CAST(0);
	}

	dsp_fft_c2c_inplace(f, out);
}
