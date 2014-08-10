
#ifndef STD_DSP_BUFFER_GUARD
#define STD_DSP_BUFFER_GUARD

#include <cstdint>
#include <cassert>
#include <algorithm>
#include <utility>

#include "../base/std_dsp_mem.h"

#include "../stateless_algorithms/mono.h"

namespace std_dsp {
	template <storage_size_t CHANNELS, typename STORAGE>
	class buffer_template final {
	private:
		STORAGE storage;
	public:
		using value_type = double;
		using difference_type = storage_size_t;
		using pointer = double*;
		using const_pointer = const double*;
		using reference = double&;
		using const_reference = const double&;
		using iterator = pointer;
		using const_iterator = const iterator;

		buffer_template() {}
		buffer_template(difference_type n) { storage.resize(n); }

		//Interleaved iterators

		const_iterator cbegin() const {
			return const_iterator(storage.cbegin());
		}
		const_iterator cend() const {
			return const_iterator(storage.cend());
		}

		iterator begin() {
			return iterator(storage.begin());
		}
		iterator end() {
			return iterator(storage.end());
		}

		inline
		const_iterator begin() const {
			return const_iterator(storage.cbegin());
		}
		inline
		const_iterator end() const {
			return const_iterator(storage.cend());
		}

		//Split channel iterators

		iterator begin(storage_size_t channel) {
			return iterator(storage() + (channel * storage.size()));
		}
		iterator end(storage_size_t channel) {
			return iterator(storage() + ((channel + 1) * storage.size()));
		}

		const_iterator begin(storage_size_t channel) const {
			return const_iterator(storage() + (channel * storage.size()));
		}
		const_iterator end(storage_size_t channel) const {
			return const_iterator(storage() + ((channel + 1) * storage.size()));
		}

		const_iterator cbegin(storage_size_t channel) {
			return const_iterator(storage() + (channel * storage.size()));
		}
		const_iterator cend(storage_size_t channel) {
			return const_iterator(storage() + ((channel + 1) * storage.size()));
		}

		inline
		iterator operator*() {
			return storage();
		}
		inline
		const_iterator operator*() const {
			return storage();
		}

		inline
		iterator operator[](storage_size_t channel) {
			return begin(channel);
		}

		inline
		const_iterator operator[](storage_size_t channel) const {
			return begin(channel);
		}

		//Query methods

		inline
		storage_size_t channels() const { return CHANNELS; }
		inline
		difference_type size() const { return storage.size(); }
		inline
		difference_type capacity() const { return storage.size(); }

		//Utility methods

		inline
		void clear() { zero(CHANNELS * storage.size(), begin()); }
		inline
		void clear(storage_size_t channel) {
			zero(storage.size(), begin() + (storage.size() * channel));
		}
		inline
		void clear(storage_size_t channel, difference_type n) {
			zero(n, begin() + (storage.size() * channel));
		}

		inline
		void fill(double x) { assign(CHANNELS * storage.size(), begin(), x); }
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
			std_dsp::randomize(CHANNELS * storage.size(), begin(), a, b);
		}

		inline
		void undenormalize() {
			std_dsp::undenormalize(storage(), CHANNELS * storage.size());
		}

		inline
		void resize(difference_type n) {
			storage.resize(n);
		}

		//Equality/inequality
		friend
		bool operator==(const buffer_template& x, const buffer_template& y) {
			if(x.size() != y.size())
				return false;
			return std::equal(x.cbegin(), x.cend(), y.cbegin());
		}

		inline
		friend
		bool operator!=(const buffer_template& x, const buffer_template& y) {
			return !(x == y);
		}
	};

	template <typename STORAGE>
	class buffer_template final<1, STORAGE> {
	private:
		STORAGE storage;
	public:
		using value_type = double;
		using difference_type = storage_size_t;
		using pointer = double*;
		using const_pointer = const double*;
		using reference = double&;
		using const_reference = const double&;
		using iterator = pointer;
		using const_iterator = const iterator;

		buffer_template() {}
		buffer_template(difference_type n) { storage.resize(n); }

		const_iterator cbegin() const {
			return const_iterator(storage.cbegin());
		}
		const_iterator cend() const {
			return const_iterator(storage.cend());
		}

		iterator begin() {
			return iterator(storage.begin());
		}
		iterator end() {
			return iterator(storage.end());
		}

		inline
		const_iterator begin() const {
			return const_iterator(storage.cbegin());
		}
		inline
		const_iterator end() const {
			return const_iterator(storage.cend());
		}

		//Split channel iterators

		iterator begin(storage_size_t channel) {
			assert(channel == 0);
			return iterator(storage() + (channel * storage.size()));
		}
		iterator end(storage_size_t channel) {
			assert(channel == 0);
			return iterator(storage() + ((channel + 1) * storage.size()));
		
		const_iterator begin(storage_size_t channel) const {
			assert(channel == 0);
			return const_iterator(storage() + (channel * storage.size()));
		}
		const_iterator end(storage_size_t channel) const {
			assert(channel == 0);
			return const_iterator(storage() + ((channel + 1) * storage.size()));
		}

		const_iterator cbegin(storage_size_t channel) {
			assert(channel == 0);
			return const_iterator(storage() + (channel * storage.size()));
		}
		const_iterator cend(storage_size_t channel) {
			assert(channel == 0);
			return const_iterator(storage() + ((channel + 1) * storage.size()));
		}

		inline
		iterator operator*() {
			return storage();
		}
		inline
		const_iterator operator*() const {
			return storage();
		}

		inline
		iterator operator[](storage_size_t channel) {
			return begin(channel);
		}

		inline
		const_iterator operator[](storage_size_t channel) const {
			return begin(channel);
		}

		//Query methods

		inline
		storage_size_t channels() const { return 1; }
		inline
		difference_type size() const { return storage.size(); }
		inline
		difference_type capacity() const { return storage.size(); }

		//Utility methods

		inline
		void clear() { zero(CHANNELS * storage.size(), begin()); }
		inline
		void clear(storage_size_t channel) {
			zero(storage.size(), begin() + (storage.size() * channel));
		}
		inline
		void clear(storage_size_t channel, difference_type n) {
			zero(n, begin() + (storage.size() * channel));
		}

		inline
		void fill(double x) { assign(CHANNELS * storage.size(), begin(), x); }
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
			std_dsp::randomize(CHANNELS * storage.size(), begin(), a, b);
		}

		inline
		void undenormalize() {
			std_dsp::undenormalize(storage(), CHANNELS * storage.size());
		}

		inline
		void resize(difference_type n) {
			storage.resize(n);
		}

		//Equality/inequality
		friend
		bool operator==(const buffer_template& x, const buffer_template& y) {
			if(x.size() != y.size())
				return false;
			return std::equal(x.cbegin(), x.cend(), y.cbegin());
		}

		inline
		friend
		bool operator!=(const buffer_template& x, const buffer_template& y) {
			return !(x == y);
		}
	};

	template <storage_size_t CHANNELS, storage_size_t SIZE>
	using static_buffer = buffer_template<CHANNELS, static_storage<CHANNELS, SIZE>>;

	template <storage_size_t SIZE>
	using static_mono_buffer = static_buffer<1, SIZE>;

	template <storage_size_t SIZE>
	using static_stereo_buffer = static_buffer<2, SIZE>;

	template <storage_size_t CHANNELS>
	using buffer = buffer_template<CHANNELS, dynamic_storage<CHANNELS>>;

	using mono_buffer = buffer<1>;

	using stereo_buffer = buffer<2>;
}

#endif
