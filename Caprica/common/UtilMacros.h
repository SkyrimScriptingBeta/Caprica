#pragma once

#ifdef _MSC_VER

#define ALWAYS_INLINE __forceinline
#define NEVER_INLINE __declspec(noinline)

#else

#define ALWAYS_INLINE __attribute__((always_inline)) inline
#define NEVER_INLINE __attribute__((noinline))

#endif
