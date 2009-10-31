/* Copyright (C) 2008-2009  SMe

This file is part of the Distributed Performance Library (DPL).

DPL is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

DPL is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Foobar.  If not, see <http://www.gnu.org/licenses/> or
write to the Free Software Foundation, 51 Franklin Street, Fifth
Floor, Boston, MA 02110-1301, USA.  */

#ifndef __DPL_BASE__
#define __DPL_BASE__

#define __need_size_t
#define __need_NULL
#include <stddef.h>	/* size_t, NULL */
#include <features.h>

#if (defined _ISOC99_SOURCE || defined _ISOC9X_SOURCE \
	|| (defined __STDC_VERSION__ && __STDC_VERSION__ > 199901L))
/* used to protect code that can only run in STDC >= C99 */
#define __DPL_C99_ONLY__	1
#endif

#ifdef __GNUC__
#define GCC_VERSION	(__GNUC__ * 10000       \
			 + __GNUC_MINOR__ * 100 \
			 + __GNUC_PATCHLEVEL__)
#else
#define GCC_VERSION	0
#endif

#if GCC_VERSION >= 30400
#define __inline	inline __attribute__((always_inline))
#define __pure		__attribute__((pure))
#define __const		__attribute__((const))
#define __noreturn	__attribute__((noreturn))
#define __malloc	__attribute__((malloc))
#define __must_check	__attribute__((warn_unused_result))
#define __deprecated	__attribute__((deprecated))
#define __used		__attribute__((used))
#define __unused	__attribute__((unused))
#define __packed	__attribute__((packed))
#define __fast		__attribute__((fastcall))
#define __stdcall	__attribute__((stdcall))
#define __cdecl		__attribute__((cdecl))
/* nonull is not a typo, __nonnull(params) is usually defined
   in sys/cdefs.h.  We do not use it here and redefine our
   macros for completeness. */
#define __nonull(x)	__attribute__((nonnull(x)))
#define likely(x)	__builtin_expect(!!(x), 1)
#define unlikely(x)	__builtin_expect(!!(x), 0)
#else
#define __inline	/* no inline */
#define __pure		/* no pure */
#define __const		/* no const */
#define __noreturn	/* no noreturn */
#define __malloc	/* no malloc */
#define __must_check	/* no warn_unused_result */
#define __deprecated	/* no deprecated */
#define __used		/* no used */
#define __unused	/* no unused */
#define __packed	/* no packed */
#define __fast		/* no fastcall */
#define __stdcall	/* no stdcall */
#define __cdecl		/* no cdecl */
#define __nonull(x)	/* no nonnull */
#define likely(x)	(x)
#define unlikely(x)	(x)
#endif

#define __none	/* used for DPL_BASE_FUNC */

/* naming conventions */
#define DPL_FUNC(name, type)			type dpl_##name
#define DPL_BASE_FUNC(name, type, _attr)	type dpl_##name _attr
#define DPL_TYPE(name, type)			typedef type dpl_##name##_t
#define DPL_DECLARE(name, type)			type dpl_##name

#define DPL_HANDLE	typedef struct { int unused; } __DPL_HANDLE;

extern void __dpl_fail(const char *assertion, const char *file,
		       unsigned int line, const char *function) __noreturn;

extern void* __dpl_malloc(size_t len) __malloc;
extern void __dpl_free(void *ptr);

extern void* __dpl_malloc32(int32_t n);
extern int32_t __dpl_msize32(void *ptr);

#ifdef DPL_NDEBUG
#define dpl_assert(expr)	((void)(0))
#else
#define dpl_assert(expr)		\
	((expr)				\
	 ? (void)(0)			\
	 : __dpl_fail(#expr, __FILE__, __LINE__, __func__))
#endif


#endif
