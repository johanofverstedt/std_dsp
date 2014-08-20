
#ifndef STD_DSP_TEST_SIGNALS_GUARD
#define STD_DSP_TEST_SIGNALS_GUARD

#include <cstdint>
#include <cmath>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

namespace std_dsp {
	namespace test_signals {
		template <typename T>
		inline
		T increasing(std::size_t x) {
			return static_cast<T>(
				x
			);
		}

		template <typename T>
		inline
		T decreasing(std::size_t offset, std::size_t x) {
			return static_cast<T>(
				offset - x
			);
		}

		template <typename T>
		inline
		T alternate_sign_increasing(std::size_t x) {
			return static_cast<T>(
				(((x & 1) == 0) ? increasing<T>(x) : (-increasing<T>(x)))
			);
		}

		template <typename T>
		inline
		T alternate_sign_decreasing(std::size_t offset, std::size_t x) {
			return static_cast<T>(
				(((x & 1) == 0) ? decreasing<T>(offset, x) : (-decreasing<T>(offset, x)))
			);
		}

		template <typename T>
		inline
		T sine(std::size_t period, std::size_t x, double amplitude) {
			static const double rad_2_pi = 2.0 * M_PI;
			x = x % period;
			return static_cast<T>(
				amplitude *
				sin((x / static_cast<double>(period)) * rad_2_pi)
			);
		}
	}
}

#endif
