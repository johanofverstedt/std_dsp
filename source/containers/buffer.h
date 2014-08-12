
#ifndef STD_DSP_BUFFER_GUARD
#define STD_DSP_BUFFER_GUARD

#include <cstdint>
#include <cassert>
#include <algorithm>
#include <utility>

#include "../base/std_dsp_mem.h"

#include "../stateless_algorithms/mono.h"

namespace std_dsp {
	class channel_iterator {
	private:
		double* ptr;
		storage_size_t stride;
	public:
		channel_iterator() {}
		channel_iterator(double* ptr, storage_size_t stride) : ptr(ptr), stride(stride) {}

		inline
		double* begin() {
			return ptr;
		}
		inline
		double* end() {
			return ptr + stride;
		}
		inline
		const double* begin() const {
			return ptr;
		}
		inline
		const double* end() const {
			return ptr + stride;
		}
		inline
		const double* cbegin() {
			return ptr;
		}
		inline
		const double* cend() {
			return ptr + stride;
		}
		inline
		const double* cbegin() const {
			return ptr;
		}
		inline
		const double* cend() const {
			return ptr + stride;
		}

		inline
		channel_iterator& operator++() {
			ptr += stride;
			return *this;
		}
		inline
		channel_iterator operator++(int) {
			channel_iterator tmp = *this;
			ptr += stride;
			return *this;
		}
		inline
		channel_iterator& operator--() {
			ptr -= stride;
			return *this;
		}
		inline
		channel_iterator operator--(int) {
			channel_iterator tmp = *this;
			ptr -= stride;
			return *this;
		}
		inline
		channel_iterator& operator+=(storage_size_t n) {
			ptr += (stride * n);
			return *this;
		}
		inline
		channel_iterator& operator-=(storage_size_t n) {
			ptr -= (stride * n);
			return *this;
		}

		inline
		double* operator*() { return ptr; }
		inline
		const double* operator*() const { return ptr; }

		inline
		double* operator[](storage_size_t n) {
			return ptr + (n * stride);
		}
		inline
		const double* operator[](storage_size_t n) const {
			return ptr + (n * stride);
		}

		inline
		friend
		bool operator==(const channel_iterator& x, const channel_iterator& y) {
			return x.ptr == y.ptr;
		}
		inline
		friend
		bool operator!=(const channel_iterator& x, const channel_iterator& y) {
			return !(x == y);
		}
	};

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
		iterator operator[](difference_type channel) {
			return begin(channel);
		}

		inline
		const_iterator operator[](storage_size_t channel) const {
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
		void clear() { zero(storage.channels() * storage.size(), begin()); }
		inline
		void clear(storage_size_t channel) {
			zero(storage.size(), begin() + (storage.size() * channel));
		}
		inline
		void clear(storage_size_t channel, difference_type n) {
			zero(n, begin() + (storage.size() * channel));
		}

		inline
		void fill(double x) { assign(storage.channels() * storage.size(), begin(), x); }
		inline
		void fill(storage_size_t channel, double x) {
			assign(storage.size(), begin() + (storage.size() * channel), x);
		}
		inline
		void fill(storage_size_t channel, difference_type n, double x) {
			assign(n, begin() + (storage.size() * channel), x);
		}

		inline
		void randomize(double a = 0.0, double b = 1.0) {
			std_dsp::randomize(storage.channels() * storage.size(), begin(), a, b);
		}

		inline
		void undenormalize() {
			std_dsp::undenormalize(storage(), storage.channels() * storage.size());
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

	template <storage_size_t CHANNELS, storage_size_t SIZE>
	using static_buffer = buffer_t<static_storage<CHANNELS, SIZE>>;

	template <storage_size_t SIZE>
	using static_mono_buffer = static_buffer<1, SIZE>;

	template <storage_size_t SIZE>
	using static_stereo_buffer = static_buffer<2, SIZE>;

	template <storage_size_t CHANNELS>
	using buffer = buffer_t<dynamic_storage<CHANNELS>>;

	using mono_buffer = buffer<1>;
	using stereo_buffer = buffer<2>;
}

#endif
