
#ifndef STD_DSP_DELAY_LINE_GUARD
#define STD_DSP_DELAY_LINE_GUARD

#include <cstdint>
#include <cassert>
#include <algorithm>
#include <utility>

#include "type_tag.h"

#include "../base/std_dsp_mem.h"

#include "../stateless_algorithms/mono.h"

#ifdef WIN32
#define constexpr
#else
#endif

namespace std_dsp {
	namespace detail {	
		inline
		storage_size_t wrap_forward(storage_size_t i, storage_size_t buf_size) {
			if (i >= buf_size)
				return i % buf_size;
			return i;
		}
		inline
		storage_size_t wrap_reverse(storage_size_t i, storage_size_t buf_size) {
			while (i < 0)
				i += buf_size;
			return i;
		}
		inline
		storage_size_t wrap_bidirectional(storage_size_t i, storage_size_t buf_size) {
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
	constexpr storage_size_t minimum_buffer_size(storage_size_t max_delay, storage_size_t block_size) {
		return max_delay + block_size + 1;
	}

	//
	//  delay_line_iterator is an intermediate iterator 0
	//
	template <storage_size_t CHANNELS, typename CHANNELS_TAG = split_channels_tag>
	class delay_line_iterator; //Must use a specialization

	template <storage_size_t CHANNELS>
	class delay_line_iterator<CHANNELS, split_channels_tag> {
	private:
		double* data;
		storage_size_t pos;
		storage_size_t size;
	public:
		delay_line_iterator() : data(nullptr), pos(0), size(0) {}
		delay_line_iterator(double* data, storage_size_t pos, storage_size_t size) : data(data), pos(pos), size(size) {}

		inline
		bool channels() const { return CHANNELS; }
		inline
		storage_size_t channels() const { return CHANNELS; }

		inline
		std::pair<double*, storage_size_t> begin(storage_size_t channel) {
			return std::make_pair(data + (channel * size) + pos, size - pos);
		}

		void rotate(storage_size_t n) {
			assert(n >= 0);
			pos = wrap_forward(n, size);
		}
	};

	template <storage_size_t CHANNELS>
	class delay_line_iterator<CHANNELS, interleaved_channels_tag> {
	private:
		double* data;
		storage_size_t pos;
		storage_size_t size;

		inline
		double* indexed_ptr(storage_size_t i) { return data + pos * CHANNELS; }
		inline
		const double* indexed_ptr(storage_size_t i) const { return data + pos * CHANNELS; }
	public:
		delay_line_iterator() : data(nullptr), pos(0), size(0) {}
		delay_line_iterator(double* data, storage_size_t pos, storage_size_t size) : data(data), pos(pos), size(size) {}

		inline
		std::pair<double*, storage_size_t> begin() {
			return std::make_pair(indexed_ptr(pos), size - pos);
		}
		inline
		std::pair<const double*, storage_size_t> begin() const {
			return std::make_pair(indexed_ptr(pos), size - pos);
		}

		double& operator[](storage_size_t channel, storage_size_t i) {
			return *(indexed_ptr(i) + channel);
		}
		const double& operator[](storage_size_t channel, storage_size_t i) const {
			return *(indexed_ptr(i) + channel);
		}

		void rotate(storage_size_t n) {
			assert(n >= 0);
			pos = wrap_forward(n, size);
		}
	};

	template <std::size_t CHANNELS, typename STORAGE, typename STEREO_TAG = SplitStereoTag>
	class delay_line_buffer {
	private:
		STORAGE storage;
		storage_size_t write_head;
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
		void rotate(storage_size_t n) {
			assert(n >= 0);
			write_head = wrap_forward(write_head + n, storage.size());
		}

		//
		//  Resizes the underlying storage if its allowed by the storage
		//  implementation. Otherwise gives a compile-time error.
		//  Also resets the delay and fills the memory with zeros.
		//
		inline
		void resize(storage_size_t n) {
			storage.resize(n);
			write_head = 0;
			clear();
		}
	};
}

#endif
