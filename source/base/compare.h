
#ifndef STD_DSP_COMPARE_GUARD
#define STD_DSP_COMPARE_GUARD

namespace std_dsp {
	template <typename I1, typename I2, typename N>
	inline
	bool compare(I1 first1, I2 first2, N n, double tolerance = 0.00000001) {
		while(n) {
			--n;
			if(fabs(*first1 - *first2) > tolerance)
				return false;
			++first1;
			++first2;
		}
		return true;
	}
	template <typename I, typename N>
	inline
	bool compare(I first, double value, N n, double tolerance = 0.00000001) {
		while(n) {
			--n;
			if(fabs(*first - value) > tolerance)
				return false;
			++first;
		}
		return true;
	}
}

#endif
