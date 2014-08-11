
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
	using storage_size_t = std::int64_t;

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

	inline
	double* split_channel_begin(double* p, storage_size_t channel, storage_size_t size) {
		return p + channel * size;
	}
	inline
	const double* split_channel_cbegin(const double* p, storage_size_t channel, storage_size_t size) {
		return p + channel * size;
	}
	inline
	double* split_channel_end(double* p, storage_size_t channel, storage_size_t size) {
		return p + (channel + storage_size_t(1)) * size;
	}
	inline
	const double* split_channel_end(const double* p, storage_size_t channel, storage_size_t size) {
		return p + (channel + storage_size_t(1)) * size;
	}

	inline
	double* storage_end(double* p, storage_size_t channels, storage_size_t size) {
		return p + (channels * size);
	}
	inline
	const double* storage_cend(const double* p, storage_size_t channels, storage_size_t size) {
		return p + (channels * size);
	}

	template <storage_size_t CHANNELS, storage_size_t SIZE>
	class static_storage {
	private:
		SSE_ALIGN double data[CHANNELS * SIZE];
	public:

		inline
		double* begin() { return data; }
		inline
		const double* cbegin() const { return data; }
		inline
		double* end() { return storage_end(data, CHANNELS, SIZE); }
		inline
		const double* cend() const { return storage_cend(data, CHANNELS, SIZE); }
		inline
		double* operator()() { return begin(); }
		inline
		const double* operator()() const { return cbegin(); }

		storage_size_t channels() { return CHANNELS;}
		inline
		storage_size_t size() const { return SIZE; }
		inline
		storage_size_t raw_size() const { return CHANNELS * SIZE; }

		void resize(storage_size_t n); //Not possible for static storage
	};

	template <storage_size_t CHANNELS>
	class dynamic_storage {
	private:
		double* data;
		storage_size_t buf_size;
	public:
		dynamic_storage() : data(nullptr), buf_size(0LL) {}
		dynamic_storage(const dynamic_storage& x) {
			data = nullptr;
			resize(x.size());
			copy(x.get(), x.size(), data);
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
			data = x.data;
			buf_size = x.buf_size;
			x.data = nullptr;
			x.buf_size = 0LL;
		}

		friend
		void swap(dynamic_storage& x, dynamic_storage& y) {
			using std::swap;
			swap(x.data, y.data);
			swap(x.buf_size, y.buf_size);
		}

		inline
		double* begin() { return data; }
		inline
		const double* cbegin() const { return data; }
		inline
		double* end() { return storage_end(data, CHANNELS, buf_size); }
		inline
		const double* cend() const { return storage_cend(data, CHANNELS, buf_size); }
		inline
		double* operator()() { return begin(); }
		inline
		const double* operator()() const { return cbegin(); }		
		inline
		storage_size_t size() const { return buf_size; }
		inline
		storage_size_t raw_size() const { return CHANNELS * SIZE; }

		inline
		void resize(storage_size_t n) {
			if(n == buf_size)
				return;

			double* p = alloc_buf(CHANNELS * n);
			if(data != nullptr)
				free_buf(data);
			data = p;
			buf_size = n;
		}
	};
}

#endif
