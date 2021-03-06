/*
-----------------------------------------------------------------------------
This source file is part of OGRE
    (Object-oriented Graphics Rendering Engine)
For the latest info, see http://www.ogre3d.org/

Copyright (c) 2000-2006 Torus Knot Software Ltd
Also see acknowledgements in Readme.html

This program is free software; you can redistribute it and/or modify it under
the terms of the GNU Lesser General Public License as published by the Free Software
Foundation; either version 2 of the License, or (at your option) any later
version.

This program is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License along with
this program; if not, write to the Free Software Foundation, Inc., 59 Temple
Place - Suite 330, Boston, MA 02111-1307, USA, or go to
http://www.gnu.org/copyleft/lesser.txt.

You may alternatively use this source under the terms of a specific version of
the OGRE Unrestricted License provided you have obtained such a license from
Torus Knot Software Ltd.
-----------------------------------------------------------------------------
*/
#ifndef __SIMDHelper_H__
#define __SIMDHelper_H__

#include "OgrePrerequisites.h"
#include "OgrePlatformInformation.h"

// Stack-alignment hackery.
//
// If macro __OGRE_SIMD_ALIGN_STACK defined, means there requests
// special code to ensure stack align to a 16-bytes boundary.
//
// Note:
//   This macro can only guarantee callee stack pointer (esp) align
// to a 16-bytes boundary, but not that for frame pointer (ebp).
// Because most compiler might use frame pointer to access to stack
// variables, so you need to wrap those alignment required functions
// with extra function call.
//
#if defined(__INTEL_COMPILER)
// For intel's compiler, simply calling alloca seems to do the right
// thing. The size of the allocated block seems to be irrelevant.
#define __OGRE_SIMD_ALIGN_STACK()   _alloca(16)

#elif OGRE_CPU == OGRE_CPU_X86 && OGRE_COMPILER == OGRE_COMPILER_GNUC
//
// Horrible hack to align the stack to a 16-bytes boundary for gcc.
//
// We assume a gcc version >= 2.95 so that
// -mpreferred-stack-boundary works.  Otherwise, all bets are
// off.  However, -mpreferred-stack-boundary does not create a
// stack alignment, but it only preserves it.  Unfortunately,
// since Ogre are designed as a flexibility library, user might
// compile their application with wrong stack alignment, even
// if user taken care with stack alignment, but many versions
// of libc on linux call main() with the wrong initial stack
// alignment the result that the code is now pessimally aligned
// instead of having a 50% chance of being correct.
//
#define __OGRE_SIMD_ALIGN_STACK()                                   \
    {                                                               \
        /* Use alloca to allocate some memory on the stack.  */     \
        /* This alerts gcc that something funny is going on, */     \
        /* so that it does not omit the frame pointer etc.   */     \
        (void)__builtin_alloca(16);                                 \
        /* Now align the stack pointer */                           \
        __asm__ __volatile__ ("andl $-16, %esp");                   \
    }

#elif defined(_MSC_VER)
// Fortunately, MSVC will align the stack automatically

#endif


// Additional platform-dependent header files and declares.
//
// NOTE: Should be sync with __OGRE_HAVE_SSE macro.
//

#if OGRE_DOUBLE_PRECISION == 0 && OGRE_CPU == OGRE_CPU_X86

#if OGRE_COMPILER == OGRE_COMPILER_MSVC || defined(__INTEL_COMPILER)
#include "OgreNoMemoryMacros.h"
#include <xmmintrin.h>
#include "OgreMemoryMacros.h"

#elif OGRE_COMPILER == OGRE_COMPILER_GNUC
// Don't define ourself version SSE intrinsics if "xmmintrin.h" already included.
//
// Note: gcc in some platform already included "xmmintrin.h" for some reason.
// I pick up macro _XMMINTRIN_H_INCLUDED here which based on the "xmmintrin.h"
// comes with cygwin gcc 3.4.4, guess it should be solved duplicate definition
// problem on gcc for x86.
//
#if !defined(_XMMINTRIN_H_INCLUDED)

// Simulate VC/ICC intrinsics. Only used intrinsics are declared here.

typedef float __m128 __attribute__ ((mode(V4SF),aligned(16)));
typedef int __m64 __attribute__ ((mode(V2SI)));

// Macro for declare intrinsic routines always inline even if in debug build
#define __ALWAYS_INLINE    FORCEINLINE __attribute__ ((__always_inline__))

// Shuffle instruction must be declare as macro

#define _MM_SHUFFLE(fp3,fp2,fp1,fp0) \
    (((fp3) << 6) | ((fp2) << 4) | ((fp1) << 2) | ((fp0)))

#define _mm_shuffle_ps(a, b, imm8) __extension__                                        \
    ({				                                                                    \
        __m128 result;								                                    \
        __asm__("shufps %3, %2, %0" : "=x" (result) : "0" (a), "xm" (b), "N" (imm8));   \
        result;								                                            \
    })


// Load/store instructions

#define __MM_DECL_LD(name, instruction, type)                               \
    static __ALWAYS_INLINE __m128 _mm_##name(const type *addr)              \
    {                                                                       \
        __m128 result;                                                      \
        __asm__( #instruction " %1, %0" : "=x" (result) : "m" (*addr));     \
        return result;                                                      \
    }

#define __MM_DECL_LD2(name, instruction, type)                                      \
    static __ALWAYS_INLINE __m128 _mm_##name(__m128 val, const type *addr)          \
    {                                                                               \
        __m128 result;                                                              \
        __asm__( #instruction " %2, %0" : "=x" (result) : "0"(val), "m" (*addr));   \
        return result;                                                              \
    }

#define __MM_DECL_ST(name, instruction, type)                               \
    static __ALWAYS_INLINE void _mm_##name(type *addr, __m128 val)          \
    {                                                                       \
        __asm__( #instruction " %1, %0" : "=m" (*addr) : "x" (val));        \
    }

__MM_DECL_LD(loadu_ps, movups, float)
__MM_DECL_ST(storeu_ps, movups, float)

__MM_DECL_LD(load_ss, movss, float)
__MM_DECL_ST(store_ss, movss, float)

__MM_DECL_ST(storel_pi, movlps, __m64)
__MM_DECL_ST(storeh_pi, movhps, __m64)
__MM_DECL_LD2(loadl_pi, movlps, __m64)
__MM_DECL_LD2(loadh_pi, movhps, __m64)

#undef __MM_DECL_LD
#undef __MM_DECL_LD2
#undef __MM_DECL_ST

// Two operand instructions

#define __MM_DECL_OP2(name, instruction, constraint)                                    \
    static __ALWAYS_INLINE __m128 _mm_##name(__m128 a, __m128 b)                        \
    {                                                                                   \
        __m128 result;                                                                  \
        __asm__( #instruction " %2, %0" : "=x" (result) : "0" (a), #constraint (b));    \
        return result;                                                                  \
    }

__MM_DECL_OP2(add_ps, addps, xm)
__MM_DECL_OP2(add_ss, addss, xm)
__MM_DECL_OP2(sub_ps, subps, xm)
__MM_DECL_OP2(sub_ss, subss, xm)
__MM_DECL_OP2(mul_ps, mulps, xm)
__MM_DECL_OP2(mul_ss, mulss, xm)

__MM_DECL_OP2(xor_ps, xorps, xm)

__MM_DECL_OP2(unpacklo_ps, unpcklps, xm)
__MM_DECL_OP2(unpackhi_ps, unpckhps, xm)

__MM_DECL_OP2(movehl_ps, movhlps, x)
__MM_DECL_OP2(movelh_ps, movlhps, x)

__MM_DECL_OP2(cmpnle_ps, cmpnleps, xm)

#undef __MM_DECL_OP2

// Other used instructions

    static __ALWAYS_INLINE __m128 _mm_load_ps1(const float *addr)
    {
        __m128 tmp = _mm_load_ss(addr);
        return _mm_shuffle_ps(tmp, tmp, 0);
    }

    static __ALWAYS_INLINE __m128 _mm_setzero_ps(void)
    {
        __m128 result;
        __asm__("xorps %0, %0" : "=x" (result));
        return result;
    }

    static __ALWAYS_INLINE __m128 _mm_rsqrt_ps(__m128 val)
    {
        __m128 result;
        __asm__("rsqrtps %1, %0" : "=x" (result) : "xm" (val));
        //__asm__("rsqrtps %0, %0" : "=x" (result) : "0" (val));
        return result;
    }

    static __ALWAYS_INLINE int _mm_movemask_ps(__m128 val)
    {
        int result;
        __asm__("movmskps %1, %0" : "=r" (result) : "x" (val));
        return result;
    }

#endif // !defined(_XMMINTRIN_H_INCLUDED)

#endif // OGRE_COMPILER == OGRE_COMPILER_GNUC

#endif // OGRE_DOUBLE_PRECISION == 0 && OGRE_CPU == OGRE_CPU_X86



//---------------------------------------------------------------------
// SIMD macros and helpers
//---------------------------------------------------------------------


namespace Ogre {

#if __OGRE_HAVE_SSE

/** Macro __MM_RSQRT_PS calculate square root, which should be used for
    normalise normals only. It might be use NewtonRaphson reciprocal square
    root for high precision, or use SSE rsqrt instruction directly, based
    on profile to pick up perfect one.
@note:
    Prefer to never use NewtonRaphson reciprocal square root at all, since
    speed test indicate performance loss 10% for unrolled version, and loss
    %25 for general version (P4 3.0G HT). A slight loss in precision not
    that important in case of normalise normals.
*/
#if 1
#define __MM_RSQRT_PS(x)    _mm_rsqrt_ps(x)
#else
#define __MM_RSQRT_PS(x)    __mm_rsqrt_nr_ps(x) // Implemented below
#endif

/** Performing the transpose of a 4x4 matrix of single precision floating
    point values.
    Arguments r0, r1, r2, and r3 are __m128 values whose elements
    form the corresponding rows of a 4x4 matrix.
    The matrix transpose is returned in arguments r0, r1, r2, and
    r3 where r0 now holds column 0 of the original matrix, r1 now
    holds column 1 of the original matrix, etc.
*/
#define __MM_TRANSPOSE4x4_PS(r0, r1, r2, r3)                                        \
    {                                                                               \
        __m128 t3, t2, t1, t0;                                                      \
                                                                                    \
                                                            /* r00 r01 r02 r03 */   \
                                                            /* r10 r11 r12 r13 */   \
                                                            /* r20 r21 r22 r23 */   \
                                                            /* r30 r31 r32 r33 */   \
                                                                                    \
        t0 = _mm_unpacklo_ps(r0, r1);                       /* r00 r10 r01 r11 */   \
        t2 = _mm_unpackhi_ps(r0, r1);                       /* r02 r12 r03 r13 */   \
        t1 = _mm_unpacklo_ps(r2, r3);                       /* r20 r30 r21 r31 */   \
        t3 = _mm_unpackhi_ps(r2, r3);                       /* r22 r32 r23 r33 */   \
                                                                                    \
        r0 = _mm_movelh_ps(t0, t1);                         /* r00 r10 r20 r30 */   \
        r1 = _mm_movehl_ps(t1, t0);                         /* r01 r11 r21 r31 */   \
        r2 = _mm_movelh_ps(t2, t3);                         /* r02 r12 r22 r32 */   \
        r3 = _mm_movehl_ps(t3, t2);                         /* r03 r13 r23 r33 */   \
    }

/** Performing the transpose of a continuous stored rows of a 4x3 matrix to
    a 3x4 matrix of single precision floating point values.
    Arguments v0, v1, and v2 are __m128 values whose elements form the
    corresponding continuous stored rows of a 4x3 matrix.
    The matrix transpose is returned in arguments v0, v1, and v2, where
    v0 now holds column 0 of the original matrix, v1 now holds column 1
    of the original matrix, etc.
*/
#define __MM_TRANSPOSE4x3_PS(v0, v1, v2)                                            \
    {                                                                               \
        __m128 t0, t1, t2;                                                          \
                                                                                    \
                                                            /* r00 r01 r02 r10 */   \
                                                            /* r11 r12 r20 r21 */   \
                                                            /* r22 r30 r31 r32 */   \
                                                                                    \
        t0 = _mm_shuffle_ps(v0, v2, _MM_SHUFFLE(3,0,3,0));  /* r00 r10 r22 r32 */   \
        t1 = _mm_shuffle_ps(v0, v1, _MM_SHUFFLE(1,0,2,1));  /* r01 r02 r11 r12 */   \
        t2 = _mm_shuffle_ps(v1, v2, _MM_SHUFFLE(2,1,3,2));  /* r20 r21 r30 r31 */   \
                                                                                    \
        v0 = _mm_shuffle_ps(t0, t2, _MM_SHUFFLE(2,0,1,0));  /* r00 r10 r20 r30 */   \
        v1 = _mm_shuffle_ps(t1, t2, _MM_SHUFFLE(3,1,2,0));  /* r01 r11 r21 r31 */   \
        v2 = _mm_shuffle_ps(t1, t0, _MM_SHUFFLE(3,2,3,1));  /* r02 r12 r22 r32 */   \
    }

/** Performing the transpose of a 3x4 matrix to a continuous stored rows of
    a 4x3 matrix of single precision floating point values.
    Arguments v0, v1, and v2 are __m128 values whose elements form the
    corresponding columns of a 3x4 matrix.
    The matrix transpose is returned in arguments v0, v1, and v2, as a
    continuous stored rows of a 4x3 matrix.
*/
#define __MM_TRANSPOSE3x4_PS(v0, v1, v2)                                            \
    {                                                                               \
        __m128 t0, t1, t2;                                                          \
                                                                                    \
                                                            /* r00 r10 r20 r30 */   \
                                                            /* r01 r11 r21 r31 */   \
                                                            /* r02 r12 r22 r32 */   \
                                                                                    \
        t0 = _mm_shuffle_ps(v0, v2, _MM_SHUFFLE(2,0,3,1));  /* r10 r30 r02 r22 */   \
        t1 = _mm_shuffle_ps(v1, v2, _MM_SHUFFLE(3,1,3,1));  /* r11 r31 r12 r32 */   \
        t2 = _mm_shuffle_ps(v0, v1, _MM_SHUFFLE(2,0,2,0));  /* r00 r20 r01 r21 */   \
                                                                                    \
        v0 = _mm_shuffle_ps(t2, t0, _MM_SHUFFLE(0,2,2,0));  /* r00 r01 r02 r10 */   \
        v1 = _mm_shuffle_ps(t1, t2, _MM_SHUFFLE(3,1,2,0));  /* r11 r12 r20 r21 */   \
        v2 = _mm_shuffle_ps(t0, t1, _MM_SHUFFLE(3,1,1,3));  /* r22 r30 r31 r32 */   \
    }

/** Fill vector of single precision floating point with selected value.
    Argument 'fp' is a digit[0123] that represents the fp of argument 'v'.
*/
#define __MM_SELECT(v, fp)                                                          \
    _mm_shuffle_ps((v), (v), _MM_SHUFFLE((fp),(fp),(fp),(fp)))

/// Accumulate four vector of single precision floating point values.
#define __MM_ACCUM4_PS(a, b, c, d)                                                  \
    _mm_add_ps(_mm_add_ps(a, b), _mm_add_ps(c, d))

/** Performing dot-product between two of four vector of single precision
    floating point values.
*/
#define __MM_DOT4x4_PS(a0, a1, a2, a3, b0, b1, b2, b3)                              \
    __MM_ACCUM4_PS(_mm_mul_ps(a0, b0), _mm_mul_ps(a1, b1), _mm_mul_ps(a2, b2), _mm_mul_ps(a3, b3))

/** Performing dot-product between four vector and three vector of single
    precision floating point values.
*/
#define __MM_DOT4x3_PS(r0, r1, r2, r3, v0, v1, v2)                                  \
    __MM_ACCUM4_PS(_mm_mul_ps(r0, v0), _mm_mul_ps(r1, v1), _mm_mul_ps(r2, v2), r3)

/// Accumulate three vector of single precision floating point values.
#define __MM_ACCUM3_PS(a, b, c)                                                     \
    _mm_add_ps(_mm_add_ps(a, b), c)

/** Performing dot-product between two of three vector of single precision
    floating point values.
*/
#define __MM_DOT3x3_PS(r0, r1, r2, v0, v1, v2)                                      \
    __MM_ACCUM3_PS(_mm_mul_ps(r0, v0), _mm_mul_ps(r1, v1), _mm_mul_ps(r2, v2))

/// Calculate multiply of two vector and plus another vector
#define __MM_MADD_PS(a, b, c)                                                       \
    _mm_add_ps(_mm_mul_ps(a, b), c)

/// Linear interpolation
#define __MM_LERP_PS(t, a, b)                                                       \
    __MM_MADD_PS(_mm_sub_ps(b, a), t, a)

/// Calculate multiply of two single floating value and plus another floating value
#define __MM_MADD_SS(a, b, c)                                                       \
    _mm_add_ss(_mm_mul_ss(a, b), c)

/// Linear interpolation
#define __MM_LERP_SS(t, a, b)                                                       \
    __MM_MADD_SS(_mm_sub_ss(b, a), t, a)

/// Same as _mm_load_ps, but can help VC generate more optimised code.
#define __MM_LOAD_PS(p)                                                             \
    (*(__m128*)(p))

/// Same as _mm_store_ps, but can help VC generate more optimised code.
#define __MM_STORE_PS(p, v)                                                         \
    (*(__m128*)(p) = (v))


    /** Helper to load/store SSE data based on whether or not aligned.
    */
    template <bool aligned = false>
    struct SSEMemoryAccessor
    {
        static FORCEINLINE __m128 load(const float *p)
        {
            return _mm_loadu_ps(p);
        }
        static FORCEINLINE void store(float *p, const __m128& v)
        {
            _mm_storeu_ps(p, v);
        }
    };
    // Special aligned accessor
    template <>
    struct SSEMemoryAccessor<true>
    {
        static FORCEINLINE const __m128& load(const float *p)
        {
            return __MM_LOAD_PS(p);
        }
        static FORCEINLINE void store(float *p, const __m128& v)
        {
            __MM_STORE_PS(p, v);
        }
    };

    /** Check whether or not the given pointer perfect aligned for SSE.
    */
    static FORCEINLINE bool _isAlignedForSSE(const void *p)
    {
        return (((size_t)p) & 15) == 0;
    }

    /** Calculate NewtonRaphson Reciprocal Square Root with formula:
            0.5 * rsqrt(x) * (3 - x * rsqrt(x)^2)
    */
    static FORCEINLINE __m128 __mm_rsqrt_nr_ps(const __m128& x)
    {
        static const __m128 v0pt5 = { 0.5f, 0.5f, 0.5f, 0.5f };
        static const __m128 v3pt0 = { 3.0f, 3.0f, 3.0f, 3.0f };
        __m128 t = _mm_rsqrt_ps(x);
        return _mm_mul_ps(_mm_mul_ps(v0pt5, t),
            _mm_sub_ps(v3pt0, _mm_mul_ps(_mm_mul_ps(x, t), t)));
    }

// Macro to check the stack aligned for SSE
#if OGRE_DEBUG_MODE
#define __OGRE_CHECK_STACK_ALIGNED_FOR_SSE()        \
    {                                               \
        __m128 test;                                \
        assert(_isAlignedForSSE(&test));            \
    }

#else   // !OGRE_DEBUG_MODE
#define __OGRE_CHECK_STACK_ALIGNED_FOR_SSE()

#endif  // OGRE_DEBUG_MODE


#endif  // __OGRE_HAVE_SSE

}

#endif // __SIMDHelper_H__
