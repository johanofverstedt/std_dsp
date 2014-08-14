
#ifndef STD_DSP_PERF_TIME_GUARD
#define STD_DSP_PERF_TIME_GUARD

#ifdef _WIN32
#include <Windows.h>
#else
#include <sys/time.h>
#endif

namespace std_dsp {
#ifdef _WIN32
#else
	double perf_time() {
		timeval tm;
		gettimeofday(&tm, NULL);
		return 1000.0 * tm.tv_sec + (double)tm.tv_usec / 1000.0;
	}
#endif
}

#endif
