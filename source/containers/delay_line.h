
#ifndef STD_DSP_DELAY_LINE_GUARD
#define STD_DSP_DELAY_LINE_GUARD

#include <cstdint>
#include <cassert>
#include <algorithm>
#include <utility>

#include "iterators.h"
#include "../base/std_dsp_mem.h"
#include "../stateless_algorithms/mono.h"

#ifdef WIN32
#define constexpr
#else
#endif

namespace std_dsp {
	namespace detail {	
		inline
		integer_t wrap_forward(integer_t i, integer_t buf_size) {
			if (i >= buf_size)
				return i % buf_size;
			return i;
		}
		inline
		integer_t wrap_reverse(integer_t i, integer_t buf_size) {
			while (i < 0)
				i += buf_size;
			return i;
		}
		inline
		integer_t wrap_bidirectional(integer_t i, integer_t buf_size) {
			if (i >= buf_size)
				return i % buf_size;
			while (i < 0)
				i += buf_size;
			return i;
		}
	}

	//
	//  Utility function for computing the minimum buffer size needed to accomodate
	//  the maximum delay the system will use and the size of the blocks of the
	//  processing system.
	//
	inline
	constexpr integer_t minimum_buffer_size(integer_t max_delay, integer_t block_size) {
		return max_delay + block_size + 1;
	}

	template <std::size_t CHANNELS, typename STORAGE, typename STEREO_TAG = SplitStereoTag>
	class delay_line_buffer {
	private:
		STORAGE storage;
		integer_t write_head;
	public:
		delay_line() : data() {}
		delay_line(difference_type n) : data(n) {}

		//
		//  Returns a zero delay write iterator
		//
		delay_line_iterator<CHANNELS, STEREO_TAG> begin() {
			return delay_line_iterator<CHANNELS, STEREO_TAG>(
				storage.begin(),
				write_head,
				storage.size()
				);
		}
		//
		//  Returns a non-negative delay read iterator
		//  Preconditions:
		//  delay is non-negative.
		//
		delay_line_iterator<CHANNELS, STEREO_TAG> begin(storage_type_t delay) const {
			assert(delay >= 0);
			return delay_line_iterator<CHANNELS, STEREO_TAG>(
				storage.begin(),
				wrap_reverse(write_head - delay, storage.size()),
				storage.size()
				);
		}

		void clear() {
			std_dsp::zero(CHANNELS * storage.size(), storage.begin());
		}

		void fill(double x) {
			std_dsp::assign(CHANNELS * storage.size(), storage.begin(), x);
		}

		//
		//  Rotates the delay line to the left so that future iterators
		//  requested will origin at the new rotated position.
		//  Does not manipulate underlying data and has O(1) complexity.
		//  Preconditions:
		//  delay is non-negative
		// 
		inline
		void rotate(integer_t n) {
			assert(n >= 0);
			write_head = wrap_forward(write_head + n, storage.size());
		}

		//
		//  Resizes the underlying storage if its allowed by the storage
		//  implementation. Otherwise gives a compile-time error.
		//  Also resets the delay and fills the memory with zeros.
		//
		inline
		void resize(integer_t n) {
			storage.resize(n);
			write_head = 0;
			clear();
		}
	};
}

#endif
