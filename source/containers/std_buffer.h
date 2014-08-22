
#ifndef STD_DSP_STD_BUFFER_GUARD
#define STD_DSP_STD_BUFFER_GUARD

#include <cstdint>
#include <cassert>
#include <algorithm>
#include <utility>
#include <array>

#include "../base/std_dsp_mem.h"

#include "../stateless_algorithms/mono.h"

namespace std_dsp {
	template <storage_size_t CHANNELS>
	struct split_buffer {
		class iterator {
		private:
			STORAGE* s;
			double* it;
			storage_size_t ch;

			void next_channel() {
				++ch;
				it = s.begin(ch);
			}
		};

		template <typename STORAGE>
		iterator begin() {

		}

		template <typename STORAGE>
		void rotate(STORAGE &s) {
			;
		}
	};

	template <storage_size_t CHANNELS>
	struct interleaved_buffer {

	};

	template <storage_size_t CHANNELS>
	struct split_delay_line {
		storage_size_t pos;

	};
}

#endif
