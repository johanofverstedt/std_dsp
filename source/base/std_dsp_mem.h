
#ifndef STD_DSP_MEM_GUARD
#define STD_DSP_MEM_GUARD

#include <cstdint>
#include <cstdlib>
#include <cassert>

#ifdef WIN32
#include <Windows.h>
#endif

#include "std_dsp_alignment.h"

#include "base.h"

namespace std_dsp {
	template <typename N>
	inline
	double* alloc_buf(N n) {
		assert(n >= N(0));

#ifdef WIN32
		double* x = static_cast<double*>(_aligned_malloc(static_cast<std::size_t>(n) * sizeof(double), 16));
#else
		double* x = static_cast<double*>(malloc(static_cast<std::size_t>(n) * sizeof(double)));
#endif

		//If the underlying implementation returns zero rather than throwing an exception
		//we throw the exception ourselves on failure
		if(x == 0)
			throw std::bad_alloc();

		return x;
	}

	inline
	void free_buf(double* buf) {
#ifdef WIN32
		_aligned_free(buf);
#else
		free(buf);
#endif
	}

	namespace detail {
		template <typename N>
		inline
		constexpr N make_even(N n) {
			if ((n & N(1)) == N(1))
				return n + N(1);
			return n;
		}

		inline
		integer_t calc_physical_size(integer_t ch, integer_t n) { return ch * make_even(n); }

		//CRTP static polymorphism base class for storage implementation

		template <typename STORAGE>
		class storage_base {
		private:
			inline
			integer_t channel_offset(integer_t channel) const {
				static const integer_t STRIDE = detail::make_even(size());
				return STRIDE * channel;
			}
			inline
			integer_t end_offset(integer_t channel) const {
				return channel_offset(channel) + size();
			}
		public:
			using value_type = double;
			using pointer = value_type*;
			using const_pointer = const pointer;
			using reference = value_type&;
			using const_reference = const reference;
			using difference_type = integer_t;
			using iterator = pointer;
			using const_iterator = const_pointer;

			//Interface to be provided by storage implementations (computational basis for storage types)
			//Dispatch the overriding implementation with static polymorphism

			inline
			double* get() { return static_cast<STORAGE*>(this)->get(); }
			inline
			const double* get() const { return static_cast<const STORAGE*>(this)->get(); }
			inline
			integer_t channels() const { return static_cast<const STORAGE*>(this)->channels(); }
			inline
			integer_t size() const { return static_cast<const STORAGE*>(this)->size(); }

			//Implementation of common methods

			inline
			integer_t physical_size() const { return calc_physical_size(channels(), size()); }

			//Iterator range methods

			inline
			double* begin() { return get(); }
			inline
			const double* cbegin() const { return get(); }
			inline
			const double* begin() const { return cbegin(); }
			inline
			double* end() { return get() + size() * channels(); }
			inline
			const double* end() const { return cend(); }
			inline
			const double* cend() const { return get() + size() * channels(); }
			inline
			double* begin(integer_t channel) { return get() + channel_offset(channel); }
			inline
			const double* cbegin(integer_t channel) const { return get() + channel_offset(channel); }
			inline
			const double* begin(integer_t channel) const { return cbegin(channel); }
			inline
			double* end(integer_t channel) { return get() + end_offset(channel); }
			inline
			const double* cend(integer_t channel) const { return get() + end_offset(channel); }
			inline
			const double* end(integer_t channel) const { return cend(channel); }

			inline
			std::pair<double*, double*> range() { return make_pair(begin(), end()); }
			inline
			std::pair<const double*, const double*> crange() const { return make_pair(begin(), end()); }
			inline
			std::pair<const double*, const double*> range() const { return crange(); }
			inline
			std::pair<double*, double*> range(integer_t channel) { return make_pair(begin(channel), end(channel)); }
			inline
			std::pair<const double*, const double*> crange(integer_t channel) const { return make_pair(begin(channel), end(channel)); }
			inline
			std::pair<const double*, const double*> range(integer_t channel) const { return crange(channel); }

			//Algorithms

			inline
			void clear() {
				const integer_t c = channels();
				for (integer_t i = 0; i < c; ++i)
					std::fill(begin(i), end(i), 0.0);
			}
		};
	}

	template <integer_t CHANNELS, integer_t SIZE>
	class static_storage : public detail::storage_base<static_storage<CHANNELS, SIZE>> {
	private:
		SSE_ALIGN double data[CHANNELS * SIZE + ((SIZE & 1) == 1) * (CHANNELS - 1)];
	public:
		inline
		double* get() { return data; }
		inline
		const double* get() const { return data; }
		inline
		integer_t channels() const { return CHANNELS; }
		inline
		integer_t size() const { return SIZE; }
	};

	template <integer_t CHANNELS>
	class static_storage<CHANNELS, 0LL> : public detail::storage_base<static_storage<CHANNELS, 0LL>> {
	public:
		inline
		double* get() { return nullptr; }
		inline
		const double* get() const { return nullptr; }
		inline
		integer_t channels() const { return CHANNELS; }
		inline
		integer_t size() const { return 0LL; }
	};

	template <>
	class static_storage<0LL, 0LL> : public detail::storage_base<static_storage<0LL, 0LL>>{
	public:
		inline
		double* get() { return nullptr; }
		inline
		const double* get() const { return nullptr; }
		inline
		integer_t channels() const { return 0LL; }
		inline
		integer_t size() const { return 0LL; }
	};

	template <integer_t CHANNELS = 0LL>
	class dynamic_storage : public detail::storage_base<dynamic_storage<CHANNELS>> {
	private:
		double* data;
		integer_t buf_size;
	public:
		dynamic_storage() : data(nullptr), buf_size(0LL) {}
		explicit dynamic_storage(integer_t n) : data(nullptr), buf_size(0LL) { resize(n); }
		dynamic_storage(const dynamic_storage& x) {
			data = nullptr;
			resize(x.size());
			copy_n(x.begin(), x.physical_size(), data);
		}
		dynamic_storage(dynamic_storage&& x) {
			data = x.data;
			buf_size = x.buf_size;
			x.data = nullptr;
			x.buf_size = 0LL;
		}
		~dynamic_storage() {
			free_buf(data);
		}

		dynamic_storage& operator=(const dynamic_storage& x) {
			using std::swap;
			dynamic_storage tmp(x);
			swap(*this, tmp);
		}
		dynamic_storage& operator=(dynamic_storage&& x) {
			if(this == &x)
				return *this;
			
			//Free allocated memory before moving in x's data
			free_buf(data);

			data = x.data;
			buf_size = x.buf_size;
			x.data = nullptr;
			x.buf_size = 0LL;
		}

		inline
		void swap(dynamic_storage& x) {
			using std::swap;
			swap(data, x.data);
			swap(buf_size, x.buf_size);
		}

		inline
		double* get() { return data; }
		inline
		const double* get() const { return data; }
		inline
		integer_t channels() const { return CHANNELS; }
		inline
		integer_t size() const { return buf_size; }

		inline
		void resize(integer_t n) {
			if(n == buf_size)
				return;

			double* p = alloc_buf(detail::calc_physical_size(channels(), n));
			if(data != nullptr)
				free_buf(data);
			data = p;
			buf_size = n;
		}
	};

	template <>
	class dynamic_storage<0LL> : public detail::storage_base<dynamic_storage<0LL>> {
	private:
		double* data;
		integer_t channel_count;
		integer_t buf_size;
	public:
		dynamic_storage() : data(nullptr), channel_count(0LL), buf_size(0LL) {}
		dynamic_storage(integer_t ch, integer_t n) : data(nullptr), channel_count(0LL), buf_size(0LL) { resize(ch, n); }
		dynamic_storage(const dynamic_storage& x) {
			data = nullptr;
			channel_count = 0LL;
			buf_size = 0LL;
			resize(x.channels(), x.size());
			std::copy_n(x.begin(), x.channels() * x.size(), data);
		}
		dynamic_storage(dynamic_storage&& x) {
			data = x.data;
			channel_count = x.channel_count;
			buf_size = x.buf_size;

			x.data = nullptr;
			x.channel_count = 0LL;
			x.buf_size = 0LL;
		}
		~dynamic_storage() {
			free_buf(data);
		}

		dynamic_storage& operator=(const dynamic_storage& x) {
			using std::swap;
			dynamic_storage tmp(x);
			swap(*this, tmp);
		}
		dynamic_storage& operator=(dynamic_storage&& x) {
			if (this == &x)
				return *this;
			
			//Free allocated memory before moving in x's data
			free_buf(data);

			data = x.data;
			channel_count = x.channel_count;
			buf_size = x.buf_size;
			
			//Clear x
			x.data = nullptr;
			x.channel_count = 0LL;
			x.buf_size = 0LL;
			return *this;
		}

		inline
		void swap(dynamic_storage& x) {
			using std::swap;
			swap(data, x.data);
			swap(buf_size, x.buf_size);
			swap(channel_count, x.channel_count);
		}

		inline
		double* get() { return data; }
		inline
		const double* get() const { return data; }
		inline
		integer_t channels() const { return channel_count; }
		inline
		integer_t size() const { return buf_size; }

		inline
		void resize(integer_t ch, integer_t n) {
			if (ch == channels() && n == buf_size)
				return;

			double* p = alloc_buf(detail::calc_physical_size(ch, n));
			if (data != nullptr)
				free_buf(data);
			data = p;
			channel_count = ch;
			buf_size = n;
		}

		inline
		void resize(integer_t n) {
			resize(channel_count, n);
		}
	};

	template <integer_t CHANNELS>
	inline
	void swap(dynamic_storage<CHANNELS>&x, dynamic_storage<CHANNELS>&y) {
		x.swap(y);
	}

	template <typename S1, typename S2>
	inline
	bool compare_split(const S1& x, const S2& y) {
		if (x.channels() != y.channels())
			return false;
		for (integer_t i = 0; i < x.channels(); ++i) {
			if (!compare(x.cbegin(i), y.cbegin(i), x.size()))
				return false;
		}
		return true;
	}
	template <typename S1, typename S2>
	inline
	bool compare_interleaved(const S1& x, const S2& y) {
		if (x.channels() != y.channels())
			return false;
		if (!compare(x.cbegin(), y.cbegin(), x.channels() * x.size()))
			return false;
		return true;
	}
}

#endif
