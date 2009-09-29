#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>

#define DC_CRC24_INIT	0xb704ceL
#define DC_CRC24_POLY	0x1864cfbL

#define DC_ADLER_BASE	65521L
#define DC_ADLER_NMAX	5552
#define _ADLER_DO1(buf,i)	{s1 += buf[i]; s2 += s1;}
#define _ADLER_DO2(buf,i)	_ADLER_DO1(buf,i); _ADLER_DO1(buf,i+1);
#define _ADLER_DO4(buf,i)	_ADLER_DO2(buf,i); _ADLER_DO2(buf,i+2);
#define _ADLER_DO8(buf,i)	_ADLER_DO4(buf,i); _ADLER_DO4(buf,i+4);
#define DC_ADLER_DO16(buf)	_ADLER_DO8(buf,0); _ADLER_DO8(buf,8);

#define DC_ARMOR_PAD	'='
const char dc_armor_ascii[64] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

#define DC_ARMOR_1		0xfc
#define DC_ARMOR_2A	0x03
#define DC_ARMOR_2B	0xf0
#define DC_ARMOR_3A	0x0f
#define DC_ARMOR_3B	0xc0
#define DC_ARMOR_4		0x3f

#ifndef _DPL_CRYPTO_CRC_DEFINED
#define _DPL_CRYPTO_CRC_DEFINED
typedef long crc24;
#endif

#ifndef _DPL_CRYPTO_UL32_DEFINED
#define _DPL_CRYPTO_UL32_DEFINED
typedef unsigned long ul32;
#endif

/**
  Calculate the block-wise CRC 24 Radix-64 checksum.  The crc variable
  should be the output of a previous block's crc24 or CRC24_INIT.

  As represented in RFC 4880.
 */
crc24 dc_crc24_rotate(unsigned char *octets, size_t len, crc24 crc)
{
	int i;

	while (len--)
	{
		crc ^= (*octets++) << 16;
		for (i = 0; i < 8; i++)
		{
			crc <<= 1;
			if (crc & 0x1000000)
			{
				crc ^= DC_CRC24_POLY;
			}
		}
	}

	return crc & 0xffffffL;
}

/**
  Gray Coding.  Use s < 0 for inverse s >= 0 for forward.
  
  Based on code from Numerical Recipes in C.
 */
ul32 dc_gray32(ul32 n, int s)
{
	int ish;
	ul32 ans, idiv;

	if (s >= 0)
	{
		return (n ^ (n >> 1));
	}

	ish = 1;
	ans = n;

	for (;;)
	{
		ans ^= (idiv = ans >> ish);
		if ((idiv <= 1) || (ish == 16)) return ans;
		ish <<= 1;
	}
}

/**
  block-wise adler32 CRC (where the first argument is the
  CRC from the previous block)
  
  Adapted from zlib.
 */
ul32 dc_adler32(ul32 adler, const char *buf, size_t len)
{
	ul32 s1 = adler & 0xffff;
	ul32 s2 = (adler >> 16) & 0xffff;
	int k;

	if (buf == NULL) { return 1L; }

	while (len > 0)
	{
		k = len < DC_ADLER_NMAX ? len : DC_ADLER_NMAX;
		len -= k;
		while (k >= 16)
		{
			DC_ADLER_DO16(buf);
			buf += 16;
			k -= 16;
		}
		if (k != 0) do {
			s1 += *buf++;
			s2 += s1;
		} while (--k);

		s1 %= DC_ADLER_BASE;
		s2 %= DC_ADLER_BASE;
	}

	return (s2 << 16) | s1;
}

/**
  Adler32 CRC
 */
ul32 dc_adler32_ck(const char *buf, size_t len)
{
	return dc_adler32(1L, buf, len);
}

void dc_armor_chunk24(char *octets, char *index, size_t octet_len)
{
	int i;
	
	index[0] = 0x0 | ((octets[0] & DC_ARMOR_1) >> 2);
	index[1] = 0x0 | ((octets[0] & DC_ARMOR_2A) << 4);
	
	index[0] = dc_armor_ascii[index[0]];

	if (octet_len >= 2)
	{
		index[1] |= ((octets[1] & DC_ARMOR_2B) >> 4);
		index[2] = 0x0 | ((octets[1] & DC_ARMOR_3A) << 2);
		if (octet_len == 3)
		{
			index[2] |= ((octets[2] & DC_ARMOR_3B) >> 6);
			index[3] = 0x0 | (octets[2] & DC_ARMOR_4);
			index[3] = dc_armor_ascii[index[3]];
		} else {
			index[3] = DC_ARMOR_PAD;
		}

		index[1] = dc_armor_ascii[index[1]];
		index[2] = dc_armor_ascii[index[2]];
	} else {
		index[1] = dc_armor_ascii[index[1]];
		index[2] = DC_ARMOR_PAD;
		index[3] = DC_ARMOR_PAD;
	}
}

/**
  ASCII Armor Constructor
 */
size_t dc_armor_radix64_init(char **index, size_t len)
{
	size_t n = len;
	if (len % 3) n += 3 - (len % 3);

	n /= 3;
	n *= 4;

	(*index) = (char *)malloc(n+1);
	memset(*index, 0, n+1);

	return n;
}

/**
  ASCII Armor Deconstructor
 */
void dc_armor_radix64_destroy(char *index)
{
	free(index);
}

/**
  Performs ASCII Armor encoding on given input data.  The index
  variable must be of proper length (which it will be if the
  armor_radix64_init function is used to allocate it).
 */
void dc_armor_radix64(char *octets, char *index, size_t len)
{
	int i, k;
	int chunk;

	for (k = 0, i = 0; i < len; i+=3, k+=4)
	{
		chunk = len - i;
		if (chunk > 3) { chunk = 3; }

		dc_armor_chunk24(octets+i, index+k, chunk);
	}
}

/**
  Returns the size of a given armor given the size of a string of
  octets.
 */
size_t dc_armor_radix64_size(size_t octets)
{
	size_t ret = octets;
	if (octets % 3) ret += 3 - (octets % 3);

	ret /= 3;
	ret *= 4;

	return ret;
}


#ifdef DEBUG_MAIN
/**
  Example program which reads from stdin and outputs the ASCII armor
  of the input and the ASCII armor of the crc24 checksum of the input
  as outlined in section 6 of RFC 4880.
 */
int main(int argc, char **argv)
{
	char *index;
	char pbuf[65];
	char fbuf[3072];
	int i, len;

	FILE *fp;
	size_t bytes_read;
	size_t index_size;

	crc24 crc = DC_CRC24_INIT;
	char crc_radix64[5] = {0,0,0,0,0};
	char crc_buf[4] = {0,0,0,0};
	
	if (argc > 1)
	{
		len = strlen(argv[1]);

		dc_armor_radix64_init(&index, len);
		dc_armor_radix64(argv[1], index, len);
	
		for (i = 0; i < len; i+=64)
		{
			memset(pbuf, 0, 65);
			memcpy(pbuf, index+i, 64);
			printf("%s\n", pbuf);
		}
	
		dc_armor_radix64_destroy(index);
	} else {
		fp = stdin;
		if (!isatty(STDIN_FILENO))
		{
			freopen(NULL, "rb", stdin);
		}

		index_size = dc_armor_radix64_init(&index, 3072);

		while ((bytes_read = fread(fbuf, 1, 3072, fp)) > 0)
		{
			dc_armor_radix64(fbuf, index, bytes_read);
			crc = dc_crc24_rotate(fbuf, bytes_read, crc);

			for (i = 0; i < dc_armor_radix64_size(bytes_read); i+=64)
			{
				memset(pbuf, 0, 65);
				memcpy(pbuf, index+i, 64);
				printf("%s\n", pbuf);
			}

			if (feof(fp)) break;
			memset(index, 0, index_size);
		}

		dc_armor_radix64_destroy(index);

		/* crc checksum */
		memcpy(crc_buf, &crc, 4);
		crc_buf[0] = crc_buf[1];
		crc_buf[1] = crc_buf[2];
		crc_buf[2] = crc_buf[3];
		dc_armor_radix64(crc_buf, crc_radix64, 3);
		printf("=%s\n", crc_radix64);
	}

	return EXIT_SUCCESS;
}

#endif

