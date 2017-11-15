/*
  * This program is free software: you can redistribute it and/or modify
  * it under the terms of the GNU General Public License as published by
  * the Free Software Foundation, either version 3 of the License, or
  * any later version.
  *
  * This program is distributed in the hope that it will be useful,
  * but WITHOUT ANY WARRANTY; without even the implied warranty of
  * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
  * GNU General Public License for more details.
  *
  * You should have received a copy of the GNU General Public License
  * along with this program.  If not, see <http://www.gnu.org/licenses/>.
  *
  */
#pragma once

#include "cryptonight.h"
#include <memory.h>
#include <stdio.h>

#ifdef __GNUC__
#include <x86intrin.h>
static inline uint64_t _umul128(uint64_t a, uint64_t b, uint64_t* hi)
{
	unsigned __int128 r = (unsigned __int128)a * (unsigned __int128)b;
	*hi = r >> 64;
	return (uint64_t)r;
}
#define _mm256_set_m128i(v0, v1)  _mm256_insertf128_si256(_mm256_castsi128_si256(v1), (v0), 1)
#else
#include <intrin.h>
#endif // __GNUC__

#if !defined(_LP64) && !defined(_WIN64)
#error You are trying to do a 32-bit build. This will all end in tears. I know it.
#endif

extern "C"
{
	void keccak(const uint8_t *in, int inlen, uint8_t *md, int mdlen);
	void keccakf(uint64_t st[25], int rounds);
	extern void(*const extra_hashes[4])(const void *, size_t, char *);

	__m128i soft_aesenc(__m128i in, __m128i key);
	__m128i soft_aeskeygenassist(__m128i key, uint8_t rcon);
}

// This will shift and xor tmp1 into itself as 4 32-bit vals such as
// sl_xor(a1 a2 a3 a4) = a1 (a2^a1) (a3^a2^a1) (a4^a3^a2^a1)
static inline __m128i sl_xor(__m128i tmp1)
{
	__m128i tmp4;
	tmp4 = _mm_slli_si128(tmp1, 0x04);
	tmp1 = _mm_xor_si128(tmp1, tmp4);
	tmp4 = _mm_slli_si128(tmp4, 0x04);
	tmp1 = _mm_xor_si128(tmp1, tmp4);
	tmp4 = _mm_slli_si128(tmp4, 0x04);
	tmp1 = _mm_xor_si128(tmp1, tmp4);
	return tmp1;
}

template<uint8_t rcon>
static inline void aes_genkey_sub(__m128i* xout0, __m128i* xout2)
{
	__m128i xout1 = _mm_aeskeygenassist_si128(*xout2, rcon);
	xout1 = _mm_shuffle_epi32(xout1, 0xFF); // see PSHUFD, set all elems to 4th elem
	*xout0 = sl_xor(*xout0);
	*xout0 = _mm_xor_si128(*xout0, xout1);
	xout1 = _mm_aeskeygenassist_si128(*xout0, 0x00);
	xout1 = _mm_shuffle_epi32(xout1, 0xAA); // see PSHUFD, set all elems to 3rd elem
	*xout2 = sl_xor(*xout2);
	*xout2 = _mm_xor_si128(*xout2, xout1);
}

static inline void soft_aes_genkey_sub(__m128i* xout0, __m128i* xout2, uint8_t rcon)
{
	__m128i xout1 = soft_aeskeygenassist(*xout2, rcon);
	xout1 = _mm_shuffle_epi32(xout1, 0xFF); // see PSHUFD, set all elems to 4th elem
	*xout0 = sl_xor(*xout0);
	*xout0 = _mm_xor_si128(*xout0, xout1);
	xout1 = soft_aeskeygenassist(*xout0, 0x00);
	xout1 = _mm_shuffle_epi32(xout1, 0xAA); // see PSHUFD, set all elems to 3rd elem
	*xout2 = sl_xor(*xout2);
	*xout2 = _mm_xor_si128(*xout2, xout1);
}

template<bool SOFT_AES>
static inline void aes_genkey(const __m128i* memory, __m128i* k0, __m128i* k1, __m128i* k2, __m128i* k3,
	__m128i* k4, __m128i* k5, __m128i* k6, __m128i* k7, __m128i* k8, __m128i* k9)
{
	__m128i xout0, xout2;

	xout0 = _mm_load_si128(memory);
	xout2 = _mm_load_si128(memory+1);
	*k0 = xout0;
	*k1 = xout2;

	if(SOFT_AES)
		soft_aes_genkey_sub(&xout0, &xout2, 0x01);
	else
		aes_genkey_sub<0x01>(&xout0, &xout2);
	*k2 = xout0;
	*k3 = xout2;

	if(SOFT_AES)
		soft_aes_genkey_sub(&xout0, &xout2, 0x02);
	else
		aes_genkey_sub<0x02>(&xout0, &xout2);
	*k4 = xout0;
	*k5 = xout2;

	if(SOFT_AES)
		soft_aes_genkey_sub(&xout0, &xout2, 0x04);
	else
		aes_genkey_sub<0x04>(&xout0, &xout2);
	*k6 = xout0;
	*k7 = xout2;

	if(SOFT_AES)
		soft_aes_genkey_sub(&xout0, &xout2, 0x08);
	else
		aes_genkey_sub<0x08>(&xout0, &xout2);
	*k8 = xout0;
	*k9 = xout2;
}

static inline void aes_round(__m128i key, __m128i* x0, __m128i* x1, __m128i* x2, __m128i* x3, __m128i* x4, __m128i* x5, __m128i* x6, __m128i* x7)
{
	*x0 = _mm_aesenc_si128(*x0, key);
	*x1 = _mm_aesenc_si128(*x1, key);
	*x2 = _mm_aesenc_si128(*x2, key);
	*x3 = _mm_aesenc_si128(*x3, key);
	*x4 = _mm_aesenc_si128(*x4, key);
	*x5 = _mm_aesenc_si128(*x5, key);
	*x6 = _mm_aesenc_si128(*x6, key);
	*x7 = _mm_aesenc_si128(*x7, key);
}

static inline void soft_aes_round(__m128i key, __m128i* x0, __m128i* x1, __m128i* x2, __m128i* x3, __m128i* x4, __m128i* x5, __m128i* x6, __m128i* x7)
{
	*x0 = soft_aesenc(*x0, key);
	*x1 = soft_aesenc(*x1, key);
	*x2 = soft_aesenc(*x2, key);
	*x3 = soft_aesenc(*x3, key);
	*x4 = soft_aesenc(*x4, key);
	*x5 = soft_aesenc(*x5, key);
	*x6 = soft_aesenc(*x6, key);
	*x7 = soft_aesenc(*x7, key);
}

template<size_t MEM, bool SOFT_AES, bool PREFETCH>
void cn_explode_scratchpad(const __m128i* input, __m128i* output)
{
	// This is more than we have registers, compiler will assign 2 keys on the stack
	__m128i xin0, xin1, xin2, xin3, xin4, xin5, xin6, xin7;
	__m128i k0, k1, k2, k3, k4, k5, k6, k7, k8, k9;

	aes_genkey<SOFT_AES>(input, &k0, &k1, &k2, &k3, &k4, &k5, &k6, &k7, &k8, &k9);

	xin0 = _mm_load_si128(input + 4);
	xin1 = _mm_load_si128(input + 5);
	xin2 = _mm_load_si128(input + 6);
	xin3 = _mm_load_si128(input + 7);
	xin4 = _mm_load_si128(input + 8);
	xin5 = _mm_load_si128(input + 9);
	xin6 = _mm_load_si128(input + 10);
	xin7 = _mm_load_si128(input + 11);

	for (size_t i = 0; i < MEM / sizeof(__m128i); i += 8)
	{
		if(SOFT_AES)
		{
			soft_aes_round(k0, &xin0, &xin1, &xin2, &xin3, &xin4, &xin5, &xin6, &xin7);
			soft_aes_round(k1, &xin0, &xin1, &xin2, &xin3, &xin4, &xin5, &xin6, &xin7);
			soft_aes_round(k2, &xin0, &xin1, &xin2, &xin3, &xin4, &xin5, &xin6, &xin7);
			soft_aes_round(k3, &xin0, &xin1, &xin2, &xin3, &xin4, &xin5, &xin6, &xin7);
			soft_aes_round(k4, &xin0, &xin1, &xin2, &xin3, &xin4, &xin5, &xin6, &xin7);
			soft_aes_round(k5, &xin0, &xin1, &xin2, &xin3, &xin4, &xin5, &xin6, &xin7);
			soft_aes_round(k6, &xin0, &xin1, &xin2, &xin3, &xin4, &xin5, &xin6, &xin7);
			soft_aes_round(k7, &xin0, &xin1, &xin2, &xin3, &xin4, &xin5, &xin6, &xin7);
			soft_aes_round(k8, &xin0, &xin1, &xin2, &xin3, &xin4, &xin5, &xin6, &xin7);
			soft_aes_round(k9, &xin0, &xin1, &xin2, &xin3, &xin4, &xin5, &xin6, &xin7);
		}
		else
		{
			aes_round(k0, &xin0, &xin1, &xin2, &xin3, &xin4, &xin5, &xin6, &xin7);
			aes_round(k1, &xin0, &xin1, &xin2, &xin3, &xin4, &xin5, &xin6, &xin7);
			aes_round(k2, &xin0, &xin1, &xin2, &xin3, &xin4, &xin5, &xin6, &xin7);
			aes_round(k3, &xin0, &xin1, &xin2, &xin3, &xin4, &xin5, &xin6, &xin7);
			aes_round(k4, &xin0, &xin1, &xin2, &xin3, &xin4, &xin5, &xin6, &xin7);
			aes_round(k5, &xin0, &xin1, &xin2, &xin3, &xin4, &xin5, &xin6, &xin7);
			aes_round(k6, &xin0, &xin1, &xin2, &xin3, &xin4, &xin5, &xin6, &xin7);
			aes_round(k7, &xin0, &xin1, &xin2, &xin3, &xin4, &xin5, &xin6, &xin7);
			aes_round(k8, &xin0, &xin1, &xin2, &xin3, &xin4, &xin5, &xin6, &xin7);
			aes_round(k9, &xin0, &xin1, &xin2, &xin3, &xin4, &xin5, &xin6, &xin7);
		}

		_mm_store_si128(output + i + 0, xin0);
		_mm_store_si128(output + i + 1, xin1);
		_mm_store_si128(output + i + 2, xin2);
		_mm_store_si128(output + i + 3, xin3);

		if(PREFETCH)
			_mm_prefetch((const char*)output + i + 0, _MM_HINT_T2);

		_mm_store_si128(output + i + 4, xin4);
		_mm_store_si128(output + i + 5, xin5);
		_mm_store_si128(output + i + 6, xin6);
		_mm_store_si128(output + i + 7, xin7);

		if(PREFETCH)
			_mm_prefetch((const char*)output + i + 4, _MM_HINT_T2);
	}
}

template<size_t MEM, bool SOFT_AES, bool PREFETCH>
void cn_implode_scratchpad(const __m128i* input, __m128i* output)
{
	// This is more than we have registers, compiler will assign 2 keys on the stack
	__m128i xout0, xout1, xout2, xout3, xout4, xout5, xout6, xout7;
	__m128i k0, k1, k2, k3, k4, k5, k6, k7, k8, k9;

	aes_genkey<SOFT_AES>(output + 2, &k0, &k1, &k2, &k3, &k4, &k5, &k6, &k7, &k8, &k9);

	xout0 = _mm_load_si128(output + 4);
	xout1 = _mm_load_si128(output + 5);
	xout2 = _mm_load_si128(output + 6);
	xout3 = _mm_load_si128(output + 7);
	xout4 = _mm_load_si128(output + 8);
	xout5 = _mm_load_si128(output + 9);
	xout6 = _mm_load_si128(output + 10);
	xout7 = _mm_load_si128(output + 11);

	for (size_t i = 0; i < MEM / sizeof(__m128i); i += 8)
	{
		if(PREFETCH)
			_mm_prefetch((const char*)input + i + 0, _MM_HINT_NTA);

		xout0 = _mm_xor_si128(_mm_load_si128(input + i + 0), xout0);
		xout1 = _mm_xor_si128(_mm_load_si128(input + i + 1), xout1);
		xout2 = _mm_xor_si128(_mm_load_si128(input + i + 2), xout2);
		xout3 = _mm_xor_si128(_mm_load_si128(input + i + 3), xout3);

		if(PREFETCH)
			_mm_prefetch((const char*)input + i + 4, _MM_HINT_NTA);

		xout4 = _mm_xor_si128(_mm_load_si128(input + i + 4), xout4);
		xout5 = _mm_xor_si128(_mm_load_si128(input + i + 5), xout5);
		xout6 = _mm_xor_si128(_mm_load_si128(input + i + 6), xout6);
		xout7 = _mm_xor_si128(_mm_load_si128(input + i + 7), xout7);

		if(SOFT_AES)
		{
			soft_aes_round(k0, &xout0, &xout1, &xout2, &xout3, &xout4, &xout5, &xout6, &xout7);
			soft_aes_round(k1, &xout0, &xout1, &xout2, &xout3, &xout4, &xout5, &xout6, &xout7);
			soft_aes_round(k2, &xout0, &xout1, &xout2, &xout3, &xout4, &xout5, &xout6, &xout7);
			soft_aes_round(k3, &xout0, &xout1, &xout2, &xout3, &xout4, &xout5, &xout6, &xout7);
			soft_aes_round(k4, &xout0, &xout1, &xout2, &xout3, &xout4, &xout5, &xout6, &xout7);
			soft_aes_round(k5, &xout0, &xout1, &xout2, &xout3, &xout4, &xout5, &xout6, &xout7);
			soft_aes_round(k6, &xout0, &xout1, &xout2, &xout3, &xout4, &xout5, &xout6, &xout7);
			soft_aes_round(k7, &xout0, &xout1, &xout2, &xout3, &xout4, &xout5, &xout6, &xout7);
			soft_aes_round(k8, &xout0, &xout1, &xout2, &xout3, &xout4, &xout5, &xout6, &xout7);
			soft_aes_round(k9, &xout0, &xout1, &xout2, &xout3, &xout4, &xout5, &xout6, &xout7);
		}
		else
		{
			aes_round(k0, &xout0, &xout1, &xout2, &xout3, &xout4, &xout5, &xout6, &xout7);
			aes_round(k1, &xout0, &xout1, &xout2, &xout3, &xout4, &xout5, &xout6, &xout7);
			aes_round(k2, &xout0, &xout1, &xout2, &xout3, &xout4, &xout5, &xout6, &xout7);
			aes_round(k3, &xout0, &xout1, &xout2, &xout3, &xout4, &xout5, &xout6, &xout7);
			aes_round(k4, &xout0, &xout1, &xout2, &xout3, &xout4, &xout5, &xout6, &xout7);
			aes_round(k5, &xout0, &xout1, &xout2, &xout3, &xout4, &xout5, &xout6, &xout7);
			aes_round(k6, &xout0, &xout1, &xout2, &xout3, &xout4, &xout5, &xout6, &xout7);
			aes_round(k7, &xout0, &xout1, &xout2, &xout3, &xout4, &xout5, &xout6, &xout7);
			aes_round(k8, &xout0, &xout1, &xout2, &xout3, &xout4, &xout5, &xout6, &xout7);
			aes_round(k9, &xout0, &xout1, &xout2, &xout3, &xout4, &xout5, &xout6, &xout7);
		}
	}

	_mm_store_si128(output + 4, xout0);
	_mm_store_si128(output + 5, xout1);
	_mm_store_si128(output + 6, xout2);
	_mm_store_si128(output + 7, xout3);
	_mm_store_si128(output + 8, xout4);
	_mm_store_si128(output + 9, xout5);
	_mm_store_si128(output + 10, xout6);
	_mm_store_si128(output + 11, xout7);
}

template<size_t ITERATIONS, size_t MEM, bool SOFT_AES, bool PREFETCH>
void cryptonight_hash(const void* input, size_t len, void* output, cryptonight_ctx* ctx0)
{
	keccak((const uint8_t *)input, len, ctx0->hash_state, 200);

	// Optim - 99% time boundary
	cn_explode_scratchpad<MEM, SOFT_AES, PREFETCH>((__m128i*)ctx0->hash_state, (__m128i*)ctx0->long_state);

	uint8_t* l0 = ctx0->long_state;
	uint64_t* h0 = (uint64_t*)ctx0->hash_state;

	uint64_t al0 = h0[0] ^ h0[4];
	uint64_t ah0 = h0[1] ^ h0[5];
	__m128i bx0 = _mm_set_epi64x(h0[3] ^ h0[7], h0[2] ^ h0[6]);

	uint64_t idx0 = h0[0] ^ h0[4];

	// Optim - 90% time boundary
	for(size_t i = 0; i < ITERATIONS; i++)
	{
		__m128i cx;
		cx = _mm_load_si128((__m128i *)&l0[idx0 & 0x1FFFF0]);

		if(SOFT_AES)
			cx = soft_aesenc(cx, _mm_set_epi64x(ah0, al0));
		else
			cx = _mm_aesenc_si128(cx, _mm_set_epi64x(ah0, al0));

		_mm_store_si128((__m128i *)&l0[idx0 & 0x1FFFF0], _mm_xor_si128(bx0, cx));
		idx0 = _mm_cvtsi128_si64(cx);
		bx0 = cx;

		if(PREFETCH)
			_mm_prefetch((const char*)&l0[idx0 & 0x1FFFF0], _MM_HINT_T0);

		uint64_t hi, lo, cl, ch;
		cl = ((uint64_t*)&l0[idx0 & 0x1FFFF0])[0];
		ch = ((uint64_t*)&l0[idx0 & 0x1FFFF0])[1];

		lo = _umul128(idx0, cl, &hi);

		al0 += hi;
		ah0 += lo;
		((uint64_t*)&l0[idx0 & 0x1FFFF0])[0] = al0;
		((uint64_t*)&l0[idx0 & 0x1FFFF0])[1] = ah0;
		ah0 ^= ch;
		al0 ^= cl;
		idx0 = al0;

		if(PREFETCH)
			_mm_prefetch((const char*)&l0[idx0 & 0x1FFFF0], _MM_HINT_T0);
	}

	// Optim - 90% time boundary
	cn_implode_scratchpad<MEM, SOFT_AES, PREFETCH>((__m128i*)ctx0->long_state, (__m128i*)ctx0->hash_state);

	// Optim - 99% time boundary

	keccakf((uint64_t*)ctx0->hash_state, 24);
	extra_hashes[ctx0->hash_state[0] & 3](ctx0->hash_state, 200, (char*)output);
}

// This lovely creation will do 2 cn hashes at a time. We have plenty of space on silicon
// to fit temporary vars for two contexts. Function will read len*2 from input and write 64 bytes to output
// We are still limited by L3 cache, so doubling will only work with CPUs where we have more than 2MB to core (Xeons)
template<size_t ITERATIONS, size_t MEM, bool SOFT_AES, bool PREFETCH>
void cryptonight_double_hash(const void* input, size_t len, void* output, cryptonight_ctx* __restrict ctx0, cryptonight_ctx* __restrict ctx1)
{
	keccak((const uint8_t *)input, len, ctx0->hash_state, 200);
	keccak((const uint8_t *)input+len, len, ctx1->hash_state, 200);

	// Optim - 99% time boundary
	cn_explode_scratchpad<MEM, SOFT_AES, PREFETCH>((__m128i*)ctx0->hash_state, (__m128i*)ctx0->long_state);
	cn_explode_scratchpad<MEM, SOFT_AES, PREFETCH>((__m128i*)ctx1->hash_state, (__m128i*)ctx1->long_state);

	uint8_t* l0 = ctx0->long_state;
	uint64_t* h0 = (uint64_t*)ctx0->hash_state;
	uint8_t* l1 = ctx1->long_state;
	uint64_t* h1 = (uint64_t*)ctx1->hash_state;

	uint64_t axl0 = h0[0] ^ h0[4];
	uint64_t axh0 = h0[1] ^ h0[5];
	__m128i bx0 = _mm_set_epi64x(h0[3] ^ h0[7], h0[2] ^ h0[6]);
	uint64_t axl1 = h1[0] ^ h1[4];
	uint64_t axh1 = h1[1] ^ h1[5];
	__m128i bx1 = _mm_set_epi64x(h1[3] ^ h1[7], h1[2] ^ h1[6]);

	uint64_t idx0 = h0[0] ^ h0[4];
	uint64_t idx1 = h1[0] ^ h1[4];

	// Optim - 90% time boundary
	for (size_t i = 0; i < ITERATIONS; i++)
	{
		__m128i cx;
		cx = _mm_load_si128((__m128i *)&l0[idx0 & 0x1FFFF0]);

		if(SOFT_AES)
			cx = soft_aesenc(cx, _mm_set_epi64x(axh0, axl0));
		else
			cx = _mm_aesenc_si128(cx, _mm_set_epi64x(axh0, axl0));

		_mm_store_si128((__m128i *)&l0[idx0 & 0x1FFFF0], _mm_xor_si128(bx0, cx));
		idx0 = _mm_cvtsi128_si64(cx);
		bx0 = cx;

		if(PREFETCH)
			_mm_prefetch((const char*)&l0[idx0 & 0x1FFFF0], _MM_HINT_T0);

		cx = _mm_load_si128((__m128i *)&l1[idx1 & 0x1FFFF0]);

		if(SOFT_AES)
			cx = soft_aesenc(cx, _mm_set_epi64x(axh1, axl1));
		else
			cx = _mm_aesenc_si128(cx, _mm_set_epi64x(axh1, axl1));

		_mm_store_si128((__m128i *)&l1[idx1 & 0x1FFFF0], _mm_xor_si128(bx1, cx));
		idx1 = _mm_cvtsi128_si64(cx);
		bx1 = cx;

		if(PREFETCH)
			_mm_prefetch((const char*)&l1[idx1 & 0x1FFFF0], _MM_HINT_T0);

		uint64_t hi, lo, cl, ch;
		cl = ((uint64_t*)&l0[idx0 & 0x1FFFF0])[0];
		ch = ((uint64_t*)&l0[idx0 & 0x1FFFF0])[1];

		lo = _umul128(idx0, cl, &hi);

		axl0 += hi;
		axh0 += lo;
		((uint64_t*)&l0[idx0 & 0x1FFFF0])[0] = axl0;
		((uint64_t*)&l0[idx0 & 0x1FFFF0])[1] = axh0;
		axh0 ^= ch;
		axl0 ^= cl;
		idx0 = axl0;

		if(PREFETCH)
			_mm_prefetch((const char*)&l0[idx0 & 0x1FFFF0], _MM_HINT_T0);

		cl = ((uint64_t*)&l1[idx1 & 0x1FFFF0])[0];
		ch = ((uint64_t*)&l1[idx1 & 0x1FFFF0])[1];

		lo = _umul128(idx1, cl, &hi);

		axl1 += hi;
		axh1 += lo;
		((uint64_t*)&l1[idx1 & 0x1FFFF0])[0] = axl1;
		((uint64_t*)&l1[idx1 & 0x1FFFF0])[1] = axh1;
		axh1 ^= ch;
		axl1 ^= cl;
		idx1 = axl1;

		if(PREFETCH)
			_mm_prefetch((const char*)&l1[idx1 & 0x1FFFF0], _MM_HINT_T0);
	}

	// Optim - 90% time boundary
	cn_implode_scratchpad<MEM, SOFT_AES, PREFETCH>((__m128i*)ctx0->long_state, (__m128i*)ctx0->hash_state);
	cn_implode_scratchpad<MEM, SOFT_AES, PREFETCH>((__m128i*)ctx1->long_state, (__m128i*)ctx1->hash_state);

	// Optim - 99% time boundary

	keccakf((uint64_t*)ctx0->hash_state, 24);
	extra_hashes[ctx0->hash_state[0] & 3](ctx0->hash_state, 200, (char*)output);
	keccakf((uint64_t*)ctx1->hash_state, 24);
	extra_hashes[ctx1->hash_state[0] & 3](ctx1->hash_state, 200, (char*)output + 32);
}

// This lovelier creation will do 4 cn hashes at a time.
template<size_t ITERATIONS, size_t MEM, bool SOFT_AES, bool PREFETCH>
void cryptonight_quad_hash(const void* input, size_t len, void* output, cryptonight_ctx* __restrict ctx[4])
{
	keccak((const uint8_t *)input,       len, ctx[0]->hash_state, 200);
	keccak((const uint8_t *)input+len,   len, ctx[1]->hash_state, 200);
	keccak((const uint8_t *)input+len*2, len, ctx[2]->hash_state, 200);
	keccak((const uint8_t *)input+len*3, len, ctx[3]->hash_state, 200);

	// Optim - 99% time boundary
	cn_explode_scratchpad<MEM, SOFT_AES, PREFETCH>((__m128i*)ctx[0]->hash_state, (__m128i*)ctx[0]->long_state);
	cn_explode_scratchpad<MEM, SOFT_AES, PREFETCH>((__m128i*)ctx[1]->hash_state, (__m128i*)ctx[1]->long_state);
	cn_explode_scratchpad<MEM, SOFT_AES, PREFETCH>((__m128i*)ctx[2]->hash_state, (__m128i*)ctx[2]->long_state);
	cn_explode_scratchpad<MEM, SOFT_AES, PREFETCH>((__m128i*)ctx[3]->hash_state, (__m128i*)ctx[3]->long_state);

	uint8_t* l0 = ctx[0]->long_state;
	uint64_t* h0 = (uint64_t*)ctx[0]->hash_state;
	uint8_t* l1 = ctx[1]->long_state;
	uint64_t* h1 = (uint64_t*)ctx[1]->hash_state;
	uint8_t* l2 = ctx[2]->long_state;
	uint64_t* h2 = (uint64_t*)ctx[2]->hash_state;
	uint8_t* l3 = ctx[3]->long_state;
	uint64_t* h3 = (uint64_t*)ctx[3]->hash_state;

	__m128i ax0 = _mm_set_epi64x(h0[1] ^ h0[5], h0[0] ^ h0[4]);
	__m128i bx0 = _mm_set_epi64x(h0[3] ^ h0[7], h0[2] ^ h0[6]);
	__m128i ax1 = _mm_set_epi64x(h1[1] ^ h1[5], h1[0] ^ h1[4]);
	__m128i bx1 = _mm_set_epi64x(h1[3] ^ h1[7], h1[2] ^ h1[6]);
	__m128i ax2 = _mm_set_epi64x(h2[1] ^ h2[5], h2[0] ^ h2[4]);
	__m128i bx2 = _mm_set_epi64x(h2[3] ^ h2[7], h2[2] ^ h2[6]);
	__m128i ax3 = _mm_set_epi64x(h3[1] ^ h3[5], h3[0] ^ h3[4]);
	__m128i bx3 = _mm_set_epi64x(h3[3] ^ h3[7], h3[2] ^ h3[6]);
	__m128i cx0, cx1, cx2, cx3;

	uint64_t idx0 = h0[0] ^ h0[4];
	uint64_t idx1 = h1[0] ^ h1[4];
	uint64_t idx2 = h2[0] ^ h2[4];
	uint64_t idx3 = h3[0] ^ h3[4];
	uint64_t hi, lo;

	// Optim - 90% time boundary
	__m128i *ptr0 = (__m128i *)&l0[idx0 & 0x1FFFF0];
	__m128i *ptr1 = (__m128i *)&l1[idx1 & 0x1FFFF0];
	__m128i *ptr2 = (__m128i *)&l2[idx2 & 0x1FFFF0];
	__m128i *ptr3 = (__m128i *)&l3[idx3 & 0x1FFFF0];
	for (size_t i = 0; i < ITERATIONS/2; i++)
	{
		// EVEN ROUND
		cx0 = _mm_load_si128(ptr0);
		cx1 = _mm_load_si128(ptr1);
		cx2 = _mm_load_si128(ptr2);
		cx3 = _mm_load_si128(ptr3);

		if(SOFT_AES)
		{
			cx0 = soft_aesenc(cx0, ax0);
			cx1 = soft_aesenc(cx1, ax1);
			cx2 = soft_aesenc(cx2, ax2);
			cx3 = soft_aesenc(cx3, ax3);
		}
		else
		{
			cx0 = _mm_aesenc_si128(cx0, ax0);
			cx1 = _mm_aesenc_si128(cx1, ax1);
			cx2 = _mm_aesenc_si128(cx2, ax2);
			cx3 = _mm_aesenc_si128(cx3, ax3);
		}

		_mm_store_si128(ptr0, _mm_xor_si128(bx0, cx0));
		_mm_store_si128(ptr1, _mm_xor_si128(bx1, cx1));
		_mm_store_si128(ptr2, _mm_xor_si128(bx2, cx2));
		_mm_store_si128(ptr3, _mm_xor_si128(bx3, cx3));

		idx0 = _mm_cvtsi128_si64(cx0);
		idx1 = _mm_cvtsi128_si64(cx1);
		idx2 = _mm_cvtsi128_si64(cx2);
		idx3 = _mm_cvtsi128_si64(cx3);

		ptr0 = (__m128i *)&l0[idx0 & 0x1FFFF0];
		ptr1 = (__m128i *)&l1[idx1 & 0x1FFFF0];
		ptr2 = (__m128i *)&l2[idx2 & 0x1FFFF0];
		ptr3 = (__m128i *)&l3[idx3 & 0x1FFFF0];
		if(PREFETCH)
		{
			_mm_prefetch((const char*)ptr0, _MM_HINT_T0);
			_mm_prefetch((const char*)ptr1, _MM_HINT_T0);
			_mm_prefetch((const char*)ptr2, _MM_HINT_T0);
			_mm_prefetch((const char*)ptr3, _MM_HINT_T0);
		}

		bx0 = _mm_load_si128(ptr0);
		bx1 = _mm_load_si128(ptr1);
		bx2 = _mm_load_si128(ptr2);
		bx3 = _mm_load_si128(ptr3);

		lo = _umul128(idx0, _mm_cvtsi128_si64(bx0), &hi);
		ax0 = _mm_add_epi64(ax0, _mm_set_epi64x(lo, hi));
		_mm_store_si128(ptr0, ax0);

		lo = _umul128(idx1, _mm_cvtsi128_si64(bx1), &hi);
		ax1 = _mm_add_epi64(ax1, _mm_set_epi64x(lo, hi));
		_mm_store_si128(ptr1, ax1);

		lo = _umul128(idx2, _mm_cvtsi128_si64(bx2), &hi);
		ax2 = _mm_add_epi64(ax2, _mm_set_epi64x(lo, hi));
		_mm_store_si128(ptr2, ax2);

		lo = _umul128(idx3, _mm_cvtsi128_si64(bx3), &hi);
		ax3 = _mm_add_epi64(ax3, _mm_set_epi64x(lo, hi));
		_mm_store_si128(ptr3, ax3);

		ax0 = _mm_xor_si128(ax0, bx0);
		ax1 = _mm_xor_si128(ax1, bx1);
		ax2 = _mm_xor_si128(ax2, bx2);
		ax3 = _mm_xor_si128(ax3, bx3);

		idx0 = _mm_cvtsi128_si64(ax0);
		idx1 = _mm_cvtsi128_si64(ax1);
		idx2 = _mm_cvtsi128_si64(ax2);
		idx3 = _mm_cvtsi128_si64(ax3);

		ptr0 = (__m128i *)&l0[idx0 & 0x1FFFF0];
		ptr1 = (__m128i *)&l1[idx1 & 0x1FFFF0];
		ptr2 = (__m128i *)&l2[idx2 & 0x1FFFF0];
		ptr3 = (__m128i *)&l3[idx3 & 0x1FFFF0];
		if(PREFETCH)
		{
			_mm_prefetch((const char*)ptr0, _MM_HINT_T0);
			_mm_prefetch((const char*)ptr1, _MM_HINT_T0);
			_mm_prefetch((const char*)ptr2, _MM_HINT_T0);
			_mm_prefetch((const char*)ptr3, _MM_HINT_T0);
		}

		// ODD ROUND
		bx0 = _mm_load_si128(ptr0);
		bx1 = _mm_load_si128(ptr1);
		bx2 = _mm_load_si128(ptr2);
		bx3 = _mm_load_si128(ptr3);

		if(SOFT_AES)
		{
			bx0 = soft_aesenc(bx0, ax0);
			bx1 = soft_aesenc(bx1, ax1);
			bx2 = soft_aesenc(bx2, ax2);
			bx3 = soft_aesenc(bx3, ax3);
		}
		else
		{
			bx0 = _mm_aesenc_si128(bx0, ax0);
			bx1 = _mm_aesenc_si128(bx1, ax1);
			bx2 = _mm_aesenc_si128(bx2, ax2);
			bx3 = _mm_aesenc_si128(bx3, ax3);
		}

		_mm_store_si128(ptr0, _mm_xor_si128(bx0, cx0));
		_mm_store_si128(ptr1, _mm_xor_si128(bx1, cx1));
		_mm_store_si128(ptr2, _mm_xor_si128(bx2, cx2));
		_mm_store_si128(ptr3, _mm_xor_si128(bx3, cx3));

		idx0 = _mm_cvtsi128_si64(bx0);
		idx1 = _mm_cvtsi128_si64(bx1);
		idx2 = _mm_cvtsi128_si64(bx2);
		idx3 = _mm_cvtsi128_si64(bx3);

		ptr0 = (__m128i *)&l0[idx0 & 0x1FFFF0];
		ptr1 = (__m128i *)&l1[idx1 & 0x1FFFF0];
		ptr2 = (__m128i *)&l2[idx2 & 0x1FFFF0];
		ptr3 = (__m128i *)&l3[idx3 & 0x1FFFF0];
		if(PREFETCH)
		{
			_mm_prefetch((const char*)ptr0, _MM_HINT_T0);
			_mm_prefetch((const char*)ptr1, _MM_HINT_T0);
			_mm_prefetch((const char*)ptr2, _MM_HINT_T0);
			_mm_prefetch((const char*)ptr3, _MM_HINT_T0);
		}

		cx0 = _mm_load_si128(ptr0);
		cx1 = _mm_load_si128(ptr1);
		cx2 = _mm_load_si128(ptr2);
		cx3 = _mm_load_si128(ptr3);

		lo = _umul128(idx0, _mm_cvtsi128_si64(cx0), &hi);
		ax0 = _mm_add_epi64(ax0, _mm_set_epi64x(lo, hi));
		_mm_store_si128(ptr0, ax0);

		lo = _umul128(idx1, _mm_cvtsi128_si64(cx1), &hi);
		ax1 = _mm_add_epi64(ax1, _mm_set_epi64x(lo, hi));
		_mm_store_si128(ptr1, ax1);

		lo = _umul128(idx2, _mm_cvtsi128_si64(cx2), &hi);
		ax2 = _mm_add_epi64(ax2, _mm_set_epi64x(lo, hi));
		_mm_store_si128(ptr2, ax2);

		lo = _umul128(idx3, _mm_cvtsi128_si64(cx3), &hi);
		ax3 = _mm_add_epi64(ax3, _mm_set_epi64x(lo, hi));
		_mm_store_si128(ptr3, ax3);

		ax0 = _mm_xor_si128(ax0, cx0);
		ax1 = _mm_xor_si128(ax1, cx1);
		ax2 = _mm_xor_si128(ax2, cx2);
		ax3 = _mm_xor_si128(ax3, cx3);

		idx0 = _mm_cvtsi128_si64(ax0);
		idx1 = _mm_cvtsi128_si64(ax1);
		idx2 = _mm_cvtsi128_si64(ax2);
		idx3 = _mm_cvtsi128_si64(ax3);

		ptr0 = (__m128i *)&l0[idx0 & 0x1FFFF0];
		ptr1 = (__m128i *)&l1[idx1 & 0x1FFFF0];
		ptr2 = (__m128i *)&l2[idx2 & 0x1FFFF0];
		ptr3 = (__m128i *)&l3[idx3 & 0x1FFFF0];
		if(PREFETCH)
		{
			_mm_prefetch((const char*)ptr0, _MM_HINT_T0);
			_mm_prefetch((const char*)ptr1, _MM_HINT_T0);
			_mm_prefetch((const char*)ptr2, _MM_HINT_T0);
			_mm_prefetch((const char*)ptr3, _MM_HINT_T0);
		}
	}

	// Optim - 90% time boundary
	cn_implode_scratchpad<MEM, SOFT_AES, PREFETCH>((__m128i*)ctx[0]->long_state, (__m128i*)ctx[0]->hash_state);
	cn_implode_scratchpad<MEM, SOFT_AES, PREFETCH>((__m128i*)ctx[1]->long_state, (__m128i*)ctx[1]->hash_state);
	cn_implode_scratchpad<MEM, SOFT_AES, PREFETCH>((__m128i*)ctx[2]->long_state, (__m128i*)ctx[2]->hash_state);
	cn_implode_scratchpad<MEM, SOFT_AES, PREFETCH>((__m128i*)ctx[3]->long_state, (__m128i*)ctx[3]->hash_state);

	// Optim - 99% time boundary

	keccakf((uint64_t*)ctx[0]->hash_state, 24);
	extra_hashes[ctx[0]->hash_state[0] & 3](ctx[0]->hash_state, 200, (char*)output);
	keccakf((uint64_t*)ctx[1]->hash_state, 24);
	extra_hashes[ctx[1]->hash_state[0] & 3](ctx[1]->hash_state, 200, (char*)output + 32);
	keccakf((uint64_t*)ctx[2]->hash_state, 24);
	extra_hashes[ctx[2]->hash_state[0] & 3](ctx[2]->hash_state, 200, (char*)output + 64);
	keccakf((uint64_t*)ctx[3]->hash_state, 24);
	extra_hashes[ctx[3]->hash_state[0] & 3](ctx[3]->hash_state, 200, (char*)output + 96);
}


// This even lovelier creation will do 5 cn hashes at a time.
template<size_t ITERATIONS, size_t MEM, bool SOFT_AES, bool PREFETCH>
void cryptonight_pent_hash(const void* input, size_t len, void* output, cryptonight_ctx* __restrict ctx[5])
{
	keccak((const uint8_t *)input,       len, ctx[0]->hash_state, 200);
	keccak((const uint8_t *)input+len,   len, ctx[1]->hash_state, 200);
	keccak((const uint8_t *)input+len*2, len, ctx[2]->hash_state, 200);
	keccak((const uint8_t *)input+len*3, len, ctx[3]->hash_state, 200);
	keccak((const uint8_t *)input+len*4, len, ctx[4]->hash_state, 200);

	// Optim - 99% time boundary
	cn_explode_scratchpad<MEM, SOFT_AES, PREFETCH>((__m128i*)ctx[0]->hash_state, (__m128i*)ctx[0]->long_state);
	cn_explode_scratchpad<MEM, SOFT_AES, PREFETCH>((__m128i*)ctx[1]->hash_state, (__m128i*)ctx[1]->long_state);
	cn_explode_scratchpad<MEM, SOFT_AES, PREFETCH>((__m128i*)ctx[2]->hash_state, (__m128i*)ctx[2]->long_state);
	cn_explode_scratchpad<MEM, SOFT_AES, PREFETCH>((__m128i*)ctx[3]->hash_state, (__m128i*)ctx[3]->long_state);
	cn_explode_scratchpad<MEM, SOFT_AES, PREFETCH>((__m128i*)ctx[4]->hash_state, (__m128i*)ctx[4]->long_state);

	uint8_t* l0 = ctx[0]->long_state;
	uint64_t* h0 = (uint64_t*)ctx[0]->hash_state;
	uint8_t* l1 = ctx[1]->long_state;
	uint64_t* h1 = (uint64_t*)ctx[1]->hash_state;
	uint8_t* l2 = ctx[2]->long_state;
	uint64_t* h2 = (uint64_t*)ctx[2]->hash_state;
	uint8_t* l3 = ctx[3]->long_state;
	uint64_t* h3 = (uint64_t*)ctx[3]->hash_state;
	uint8_t* l4 = ctx[4]->long_state;
	uint64_t* h4 = (uint64_t*)ctx[4]->hash_state;

	__m128i ax0 = _mm_set_epi64x(h0[1] ^ h0[5], h0[0] ^ h0[4]);
	__m128i bx0 = _mm_set_epi64x(h0[3] ^ h0[7], h0[2] ^ h0[6]);
	__m128i ax1 = _mm_set_epi64x(h1[1] ^ h1[5], h1[0] ^ h1[4]);
	__m128i bx1 = _mm_set_epi64x(h1[3] ^ h1[7], h1[2] ^ h1[6]);
	__m128i ax2 = _mm_set_epi64x(h2[1] ^ h2[5], h2[0] ^ h2[4]);
	__m128i bx2 = _mm_set_epi64x(h2[3] ^ h2[7], h2[2] ^ h2[6]);
	__m128i ax3 = _mm_set_epi64x(h3[1] ^ h3[5], h3[0] ^ h3[4]);
	__m128i bx3 = _mm_set_epi64x(h3[3] ^ h3[7], h3[2] ^ h3[6]);
	__m128i ax4 = _mm_set_epi64x(h4[1] ^ h4[5], h4[0] ^ h4[4]);
	__m128i bx4 = _mm_set_epi64x(h4[3] ^ h4[7], h4[2] ^ h4[6]);
	__m128i cx0, cx1, cx2, cx3, cx4;

	uint64_t idx0 = h0[0] ^ h0[4];
	uint64_t idx1 = h1[0] ^ h1[4];
	uint64_t idx2 = h2[0] ^ h2[4];
	uint64_t idx3 = h3[0] ^ h3[4];
	uint64_t idx4 = h4[0] ^ h4[4];
	uint64_t hi, lo;

	// Optim - 90% time boundary
	__m128i *ptr0 = (__m128i *)&l0[idx0 & 0x1FFFF0];
	__m128i *ptr1 = (__m128i *)&l1[idx1 & 0x1FFFF0];
	__m128i *ptr2 = (__m128i *)&l2[idx2 & 0x1FFFF0];
	__m128i *ptr3 = (__m128i *)&l3[idx3 & 0x1FFFF0];
	__m128i *ptr4 = (__m128i *)&l4[idx4 & 0x1FFFF0];
	for (size_t i = 0; i < ITERATIONS/2; i++)
	{
		// EVEN ROUND
		cx0 = _mm_load_si128(ptr0);
		cx1 = _mm_load_si128(ptr1);
		cx2 = _mm_load_si128(ptr2);
		cx3 = _mm_load_si128(ptr3);
		cx4 = _mm_load_si128(ptr4);

		if(SOFT_AES)
		{
			cx0 = soft_aesenc(cx0, ax0);
			cx1 = soft_aesenc(cx1, ax1);
			cx2 = soft_aesenc(cx2, ax2);
			cx3 = soft_aesenc(cx3, ax3);
			cx4 = soft_aesenc(cx4, ax4);
		}
		else
		{
			cx0 = _mm_aesenc_si128(cx0, ax0);
			cx1 = _mm_aesenc_si128(cx1, ax1);
			cx2 = _mm_aesenc_si128(cx2, ax2);
			cx3 = _mm_aesenc_si128(cx3, ax3);
			cx4 = _mm_aesenc_si128(cx4, ax4);
		}

		_mm_store_si128(ptr0, _mm_xor_si128(bx0, cx0));
		_mm_store_si128(ptr1, _mm_xor_si128(bx1, cx1));
		_mm_store_si128(ptr2, _mm_xor_si128(bx2, cx2));
		_mm_store_si128(ptr3, _mm_xor_si128(bx3, cx3));
		_mm_store_si128(ptr4, _mm_xor_si128(bx4, cx4));

		idx0 = _mm_cvtsi128_si64(cx0);
		idx1 = _mm_cvtsi128_si64(cx1);
		idx2 = _mm_cvtsi128_si64(cx2);
		idx3 = _mm_cvtsi128_si64(cx3);
		idx4 = _mm_cvtsi128_si64(cx4);

		ptr0 = (__m128i *)&l0[idx0 & 0x1FFFF0];
		ptr1 = (__m128i *)&l1[idx1 & 0x1FFFF0];
		ptr2 = (__m128i *)&l2[idx2 & 0x1FFFF0];
		ptr3 = (__m128i *)&l3[idx3 & 0x1FFFF0];
		ptr4 = (__m128i *)&l4[idx4 & 0x1FFFF0];
		if(PREFETCH)
		{
			_mm_prefetch((const char*)ptr0, _MM_HINT_T0);
			_mm_prefetch((const char*)ptr1, _MM_HINT_T0);
			_mm_prefetch((const char*)ptr2, _MM_HINT_T0);
			_mm_prefetch((const char*)ptr3, _MM_HINT_T0);
			_mm_prefetch((const char*)ptr4, _MM_HINT_T0);
		}

		bx0 = _mm_load_si128(ptr0);
		bx1 = _mm_load_si128(ptr1);
		bx2 = _mm_load_si128(ptr2);
		bx3 = _mm_load_si128(ptr3);
		bx4 = _mm_load_si128(ptr4);

		lo = _umul128(idx0, _mm_cvtsi128_si64(bx0), &hi);
		ax0 = _mm_add_epi64(ax0, _mm_set_epi64x(lo, hi));
		_mm_store_si128(ptr0, ax0);

		lo = _umul128(idx1, _mm_cvtsi128_si64(bx1), &hi);
		ax1 = _mm_add_epi64(ax1, _mm_set_epi64x(lo, hi));
		_mm_store_si128(ptr1, ax1);

		lo = _umul128(idx2, _mm_cvtsi128_si64(bx2), &hi);
		ax2 = _mm_add_epi64(ax2, _mm_set_epi64x(lo, hi));
		_mm_store_si128(ptr2, ax2);

		lo = _umul128(idx3, _mm_cvtsi128_si64(bx3), &hi);
		ax3 = _mm_add_epi64(ax3, _mm_set_epi64x(lo, hi));
		_mm_store_si128(ptr3, ax3);

		lo = _umul128(idx4, _mm_cvtsi128_si64(bx4), &hi);
		ax4 = _mm_add_epi64(ax4, _mm_set_epi64x(lo, hi));
		_mm_store_si128(ptr4, ax4);

		ax0 = _mm_xor_si128(ax0, bx0);
		ax1 = _mm_xor_si128(ax1, bx1);
		ax2 = _mm_xor_si128(ax2, bx2);
		ax3 = _mm_xor_si128(ax3, bx3);
		ax4 = _mm_xor_si128(ax4, bx4);

		idx0 = _mm_cvtsi128_si64(ax0);
		idx1 = _mm_cvtsi128_si64(ax1);
		idx2 = _mm_cvtsi128_si64(ax2);
		idx3 = _mm_cvtsi128_si64(ax3);
		idx4 = _mm_cvtsi128_si64(ax4);

		ptr0 = (__m128i *)&l0[idx0 & 0x1FFFF0];
		ptr1 = (__m128i *)&l1[idx1 & 0x1FFFF0];
		ptr2 = (__m128i *)&l2[idx2 & 0x1FFFF0];
		ptr3 = (__m128i *)&l3[idx3 & 0x1FFFF0];
		ptr4 = (__m128i *)&l4[idx4 & 0x1FFFF0];
		if(PREFETCH)
		{
			_mm_prefetch((const char*)ptr0, _MM_HINT_T0);
			_mm_prefetch((const char*)ptr1, _MM_HINT_T0);
			_mm_prefetch((const char*)ptr2, _MM_HINT_T0);
			_mm_prefetch((const char*)ptr3, _MM_HINT_T0);
			_mm_prefetch((const char*)ptr4, _MM_HINT_T0);
		}

		// ODD ROUND
		bx0 = _mm_load_si128(ptr0);
		bx1 = _mm_load_si128(ptr1);
		bx2 = _mm_load_si128(ptr2);
		bx3 = _mm_load_si128(ptr3);
		bx4 = _mm_load_si128(ptr4);

		if(SOFT_AES)
		{
			bx0 = soft_aesenc(bx0, ax0);
			bx1 = soft_aesenc(bx1, ax1);
			bx2 = soft_aesenc(bx2, ax2);
			bx3 = soft_aesenc(bx3, ax3);
			bx4 = soft_aesenc(bx4, ax4);
		}
		else
		{
			bx0 = _mm_aesenc_si128(bx0, ax0);
			bx1 = _mm_aesenc_si128(bx1, ax1);
			bx2 = _mm_aesenc_si128(bx2, ax2);
			bx3 = _mm_aesenc_si128(bx3, ax3);
			bx4 = _mm_aesenc_si128(bx4, ax4);
		}

		_mm_store_si128(ptr0, _mm_xor_si128(bx0, cx0));
		_mm_store_si128(ptr1, _mm_xor_si128(bx1, cx1));
		_mm_store_si128(ptr2, _mm_xor_si128(bx2, cx2));
		_mm_store_si128(ptr3, _mm_xor_si128(bx3, cx3));
		_mm_store_si128(ptr4, _mm_xor_si128(bx4, cx4));

		idx0 = _mm_cvtsi128_si64(bx0);
		idx1 = _mm_cvtsi128_si64(bx1);
		idx2 = _mm_cvtsi128_si64(bx2);
		idx3 = _mm_cvtsi128_si64(bx3);
		idx4 = _mm_cvtsi128_si64(bx4);

		ptr0 = (__m128i *)&l0[idx0 & 0x1FFFF0];
		ptr1 = (__m128i *)&l1[idx1 & 0x1FFFF0];
		ptr2 = (__m128i *)&l2[idx2 & 0x1FFFF0];
		ptr3 = (__m128i *)&l3[idx3 & 0x1FFFF0];
		ptr4 = (__m128i *)&l4[idx4 & 0x1FFFF0];
		if(PREFETCH)
		{
			_mm_prefetch((const char*)ptr0, _MM_HINT_T0);
			_mm_prefetch((const char*)ptr1, _MM_HINT_T0);
			_mm_prefetch((const char*)ptr2, _MM_HINT_T0);
			_mm_prefetch((const char*)ptr3, _MM_HINT_T0);
			_mm_prefetch((const char*)ptr4, _MM_HINT_T0);
		}

		cx0 = _mm_load_si128(ptr0);
		cx1 = _mm_load_si128(ptr1);
		cx2 = _mm_load_si128(ptr2);
		cx3 = _mm_load_si128(ptr3);
		cx4 = _mm_load_si128(ptr4);

		lo = _umul128(idx0, _mm_cvtsi128_si64(cx0), &hi);
		ax0 = _mm_add_epi64(ax0, _mm_set_epi64x(lo, hi));
		_mm_store_si128(ptr0, ax0);

		lo = _umul128(idx1, _mm_cvtsi128_si64(cx1), &hi);
		ax1 = _mm_add_epi64(ax1, _mm_set_epi64x(lo, hi));
		_mm_store_si128(ptr1, ax1);

		lo = _umul128(idx2, _mm_cvtsi128_si64(cx2), &hi);
		ax2 = _mm_add_epi64(ax2, _mm_set_epi64x(lo, hi));
		_mm_store_si128(ptr2, ax2);

		lo = _umul128(idx3, _mm_cvtsi128_si64(cx3), &hi);
		ax3 = _mm_add_epi64(ax3, _mm_set_epi64x(lo, hi));
		_mm_store_si128(ptr3, ax3);

		lo = _umul128(idx4, _mm_cvtsi128_si64(cx4), &hi);
		ax4 = _mm_add_epi64(ax4, _mm_set_epi64x(lo, hi));
		_mm_store_si128(ptr4, ax4);

		ax0 = _mm_xor_si128(ax0, cx0);
		ax1 = _mm_xor_si128(ax1, cx1);
		ax2 = _mm_xor_si128(ax2, cx2);
		ax3 = _mm_xor_si128(ax3, cx3);
		ax4 = _mm_xor_si128(ax4, cx4);

		idx0 = _mm_cvtsi128_si64(ax0);
		idx1 = _mm_cvtsi128_si64(ax1);
		idx2 = _mm_cvtsi128_si64(ax2);
		idx3 = _mm_cvtsi128_si64(ax3);
		idx4 = _mm_cvtsi128_si64(ax4);

		ptr0 = (__m128i *)&l0[idx0 & 0x1FFFF0];
		ptr1 = (__m128i *)&l1[idx1 & 0x1FFFF0];
		ptr2 = (__m128i *)&l2[idx2 & 0x1FFFF0];
		ptr3 = (__m128i *)&l3[idx3 & 0x1FFFF0];
		ptr4 = (__m128i *)&l4[idx4 & 0x1FFFF0];
		if(PREFETCH)
		{
			_mm_prefetch((const char*)ptr0, _MM_HINT_T0);
			_mm_prefetch((const char*)ptr1, _MM_HINT_T0);
			_mm_prefetch((const char*)ptr2, _MM_HINT_T0);
			_mm_prefetch((const char*)ptr3, _MM_HINT_T0);
			_mm_prefetch((const char*)ptr4, _MM_HINT_T0);
		}
	}

	// Optim - 90% time boundary
	cn_implode_scratchpad<MEM, SOFT_AES, PREFETCH>((__m128i*)ctx[0]->long_state, (__m128i*)ctx[0]->hash_state);
	cn_implode_scratchpad<MEM, SOFT_AES, PREFETCH>((__m128i*)ctx[1]->long_state, (__m128i*)ctx[1]->hash_state);
	cn_implode_scratchpad<MEM, SOFT_AES, PREFETCH>((__m128i*)ctx[2]->long_state, (__m128i*)ctx[2]->hash_state);
	cn_implode_scratchpad<MEM, SOFT_AES, PREFETCH>((__m128i*)ctx[3]->long_state, (__m128i*)ctx[3]->hash_state);
	cn_implode_scratchpad<MEM, SOFT_AES, PREFETCH>((__m128i*)ctx[4]->long_state, (__m128i*)ctx[4]->hash_state);

	// Optim - 99% time boundary

	keccakf((uint64_t*)ctx[0]->hash_state, 24);
	extra_hashes[ctx[0]->hash_state[0] & 3](ctx[0]->hash_state, 200, (char*)output);
	keccakf((uint64_t*)ctx[1]->hash_state, 24);
	extra_hashes[ctx[1]->hash_state[0] & 3](ctx[1]->hash_state, 200, (char*)output + 32);
	keccakf((uint64_t*)ctx[2]->hash_state, 24);
	extra_hashes[ctx[2]->hash_state[0] & 3](ctx[2]->hash_state, 200, (char*)output + 64);
	keccakf((uint64_t*)ctx[3]->hash_state, 24);
	extra_hashes[ctx[3]->hash_state[0] & 3](ctx[3]->hash_state, 200, (char*)output + 96);
	keccakf((uint64_t*)ctx[4]->hash_state, 24);
	extra_hashes[ctx[4]->hash_state[0] & 3](ctx[4]->hash_state, 200, (char*)output + 128);
}
