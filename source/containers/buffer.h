
#ifndef STD_DSP_BUFFER_GUARD
#define STD_DSP_BUFFER_GUARD

#include <cstdint>
#include <cassert>
#include <algorithm>
#include <utility>

#include "../base/std_dsp_mem.h"
#include "../iterators/channel_iterator.h"

#include "../stateless_algorithms/mono.h"

namespace std_dsp {
	template <typename STORAGE>
	class buffer_t final {
	private:
		STORAGE storage;
	public:
		using value_type = typename STORAGE::value_type;
		using difference_type = typename STORAGE::difference_type;
		using pointer = typename STORAGE::pointer;
		using const_pointer = const pointer;
		using reference = typename STORAGE::reference;
		using const_reference = const reference;
		using iterator = typename STORAGE::iterator;
		using const_iterator = const iterator;

		buffer_t() {}
		buffer_t(difference_type n) { storage.resize(n); }

		const channel_iterator cbegin() const {
			return channel_iterator(storage.cbegin(), storage.size());
		}
		const channel_iterator cend() const {
			return channel_iterator(storage.cend(), storage.size());
		}

		channel_iterator begin() {
			return channel_iterator(storage.begin(), storage.size());
		}
		channel_iterator end() {
			return channel_iterator(storage.end(), storage.size());
		}

		inline
		iterator begin(difference_type channel) {
			return begin()[channel];
		}
		iterator end(difference_type channel) {
			return end()[channel];
		}

		const_iterator cbegin(difference_type channel) {
			return cbegin()[channel];
		}
		const_iterator cend(difference_type channel) {
			return cbegin()[channel];
		}

		inline
		iterator operator*() {
			return begin(0);
		}
		inline
		const_iterator operator*() const {
			return begin(0);
		}

		inline
		iterator operator[](difference_type channel) {
			return begin(channel);
		}

		inline
		const_iterator operator[](integer_t channel) const {
			return cbegin(channel);
		}

		//Query methods

		inline
		difference_type channels() const { return storage.channels(); }
		inline
		difference_type size() const { return storage.size(); }
		inline
		difference_type capacity() const { return storage.size(); }

		//Utility methods

		inline
		void clear() { std_dsp::zero(storage.channels() * storage.size(), begin()); }
		inline
		void clear(integer_t n) {
			for(auto c : *this)
				std_dsp::zero(n, c);
		}

		inline
		void fill(double x) { std_dsp::assign(storage.channels() * storage.size(), begin(), x); }
		inline
		void fill(integer_t n, double x) {
			for(auto c : *this)
				std_dsp::assign(n, c, x);
		}

		inline
		void randomize(double a = -1.0, double b = 1.0) {
			std_dsp::randomize(storage.channels() * storage.size(), begin(), a, b);
		}
		inline
		void randomize(integer_t n, double a = -1.0, double b = 1.0) {
			for(auto c : *this)
				std_dsp::randomize(n, c, a, b);
		}

		inline
		void undenormalize() {
			std_dsp::undenormalize(storage(), storage.channels() * storage.size());
		}
		inline
		void undenormalize(integer_t n) {
			for(auto c : *this)
				std_dsp::undenormalize(c, n);
		}

		inline
		void resize(difference_type n, bool clear_storage = true) {
			storage.resize(n);
			if(clear_storage)
				clear();
		}

		//Equality/inequality
		friend
		bool operator==(const buffer_t& x, const buffer_t& y) {
			if(x.size() != y.size())
				return false;
			return std::equal(x.storage.cbegin(), x.storage.cend(), y.storage.cbegin());
		}

		inline
		friend
		bool operator!=(const buffer_t& x, const buffer_t& y) {
			return !(x == y);
		}
	};

	template <integer_t CHANNELS, integer_t SIZE>
	using static_buffer = buffer_t<static_storage<CHANNELS, SIZE>>;

	template <integer_t SIZE>
	using static_mono_buffer = static_buffer<1, SIZE>;

	template <integer_t SIZE>
	using static_stereo_buffer = static_buffer<2, SIZE>;

	template <integer_t CHANNELS>
	using buffer = buffer_t<dynamic_storage<CHANNELS>>;

	using mono_buffer = buffer<1>;
	using stereo_buffer = buffer<2>;
}

#endif
