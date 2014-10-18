
#pragma once

#ifdef WIN32

#define ALWAYS_INLINE __forceinline
#define NEVER_INLINE __declspec(noinline)

#else

#define ALWAYS_INLINE inline __attribute__((__always_inline__))
#define NEVER_INLINE __attribute__((__noinline__))

#endif
