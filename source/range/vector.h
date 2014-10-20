
#pragma once

#include <cstdint>

namespace std_dsp {
	template <std::int32_t N>
	class vector_base {
	public:
		enum count_enum {
			count = N/2,
		};
		__m128d x[count];

	};


	template <std::int32_t CHANNELS, std::int32_t N, bool REVERSE>
	class vector;

	template <bool REVERSE>
	class vector<1, 1, REVERSE> {

	};

	template <>
	class vector<1, 2, false> {
	public:
		__m128d x[N];

		inline
		vector() {}
		inline
		vector(const double* p) {
			load(p);
		}

/*
		inline
		void load(double a){
			x[0] = _mm_load_dup(a);
		}
		inline
		void load(double a0, double a1) {
			x[0] = _mm_set_pd(a0, a1);
		}

		template <std::int32_t INDEX>
		inline
		void load(const double* p) {
			x[0] = _mm_load_pd(p + INDEX);
		}
		template <std::int32_t INDEX>
		inline
		void load_orderless(const double* p) {
			x[0] = _mm_load_pd(p + INDEX);
		}
		template <std::int32_t INDEX>
		inline
		void store(double* p) {
			_mm_store_pd(x[0], p+INDEX);
		}
		template <std::int32_t INDEX>
		inline
		void store_orderless(double* p) {
			_mm_store_pd(x[0], p+INDEX);
		}
		*/
	};
	template <>
	class vector<1, 2, true> {
	public:
		__m128d x[N];

		inline
		vector() {}
		inline
		vector(const double* p) {
			load(p);
		}

/*
		inline
		void load(double a){
			x[0] = _mm_load_dup(a);
		}
		inline
		void load(double a0, double a1) {
			x[0] = _mm_load_pd(a0, a1);
		}

		template <std::int32_t INDEX>
		inline
		void load(const double* p) {
			x[0] = _mm_load_pd(p + INDEX);
		}
		template <std::int32_t INDEX>
		inline
		void load_orderless(const double* p) {
			x[0] = _mm_load_pd((p - INDEX) - 2);
		}
		template <std::int32_t INDEX>
		inline
		void store(double* p) {
			_mm_store_pd(x[0], p+INDEX);
		}
		template <std::int32_t INDEX>
		inline
		void store_orderless(double* p) {
			_mm_store_pd(x[0], p+INDEX);
		}
		*/
	};

	template <std::int32_t CHANNELS, std::int32_t N, bool REVERSE>
	vector<CHANNELS, N, REVERSE>& load(vector<CHANNELS, N, REVERSE>&, const double*);

	template <>
	vector<1, 1, false>& load<1, 1, false>(vector<1, 1, false>& v, const double* p) {
		v.x = *p;
	}
	template <>
	vector<1, 1, true>& load<1, 1, true>(vector<1, 1, false>& v, const double* p) {
		v.x = *p;
	}

	template <>
	vector<1, 2, false>& load<1, 2, false>(vector<1, 2, false>& v, const double* p) {
		v.x[0] = _mm_load_pd(p);
	}
	template <>
	vector<1, 2, true>& load<1, 2, true>(vector<1, 2, true>& v, const double* p) {
		v.x[0] = _mm_load_pd(p-2);
		v.x[0] = _mm_shuffle_pd(v.x[0], v.x[0], 1);
	}

	template <>
	vector<1, 4, false>& load<1, 4, false>(vector<1, 4, false>& v, const double* p) {
		v.x[0] = _mm_load_pd(p);
		v.x[1] = _mm_load_pd(p+2);
	}
	template <>
	vector<1, 4, true>& load<1, 4, true>(vector<1, 4, true>& v, const double* p) {
		v.x[0] = _mm_load_pd(p-2);
		v.x[1] = _mm_load_pd(p-4);
		v.x[0] = _mm_shuffle_pd(v.x[0], v.x[0], 1);
		v.x[1] = _mm_shuffle_pd(v.x[1], v.x[1], 1);
	}

	template <>
	vector<1, 8, false>& load<1, 8, false>(vector<1, 8, false>& v, const double* p) {
		v.x[0] = _mm_load_pd(p);
		v.x[1] = _mm_load_pd(p+2);
		v.x[2] = _mm_load_pd(p+4);
		v.x[3] = _mm_load_pd(p+6);
	}
	template <>
	vector<1, 8, true>& load<1, 8, true>(vector<1, 8, true>& v, const double* p) {
		v.x[0] = _mm_load_pd(p-2);
		v.x[1] = _mm_load_pd(p-4);
		v.x[2] = _mm_load_pd(p-6);
		v.x[3] = _mm_load_pd(p-8);
		v.x[0] = _mm_shuffle_pd(v.x[0], v.x[0], 1);
		v.x[1] = _mm_shuffle_pd(v.x[1], v.x[1], 1);
		v.x[2] = _mm_shuffle_pd(v.x[2], v.x[2], 1);
		v.x[3] = _mm_shuffle_pd(v.x[3], v.x[3], 1);
	}

	template <>
	vector<1, 16, false>& load<1, 16, false>(vector<1, 16, false>& v, const double* p) {
		v.x[0] = _mm_load_pd(p);
		v.x[1] = _mm_load_pd(p+2);
		v.x[2] = _mm_load_pd(p+4);
		v.x[3] = _mm_load_pd(p+6);
		v.x[4] = _mm_load_pd(p+8);
		v.x[5] = _mm_load_pd(p+10);
		v.x[6] = _mm_load_pd(p+12);
		v.x[7] = _mm_load_pd(p+14);
	}
	template <>
	vector<1, 16, true>& load<1, 16, true>(vector<1, 16, true>& v, const double* p) {
		v.x[0] = _mm_load_pd(p-2);
		v.x[1] = _mm_load_pd(p-4);
		v.x[2] = _mm_load_pd(p-6);
		v.x[3] = _mm_load_pd(p-8);
		v.x[4] = _mm_load_pd(p-10);
		v.x[5] = _mm_load_pd(p-12);
		v.x[6] = _mm_load_pd(p-14);
		v.x[7] = _mm_load_pd(p-16);
		v.x[0] = _mm_shuffle_pd(v.x[0], v.x[0], 1);
		v.x[1] = _mm_shuffle_pd(v.x[1], v.x[1], 1);
		v.x[2] = _mm_shuffle_pd(v.x[2], v.x[2], 1);
		v.x[3] = _mm_shuffle_pd(v.x[3], v.x[3], 1);
		v.x[4] = _mm_shuffle_pd(v.x[4], v.x[4], 1);
		v.x[5] = _mm_shuffle_pd(v.x[5], v.x[5], 1);
		v.x[6] = _mm_shuffle_pd(v.x[6], v.x[6], 1);
		v.x[7] = _mm_shuffle_pd(v.x[7], v.x[7], 1);
	}

	//Load 2-channels

	template <>
	vector<2, 2, false>& load<2, 2, false>(vector<2, 2, false>& v, const double* p) {
		v.x[0] = _mm_load_pd(p);
	}
	template <>
	vector<2, 2, true>& load<2, 2, true>(vector<2, 2, true>& v, const double* p) {
		v.x[0] = _mm_load_pd(p-2);
	}

	template <>
	vector<2, 4, false>& load<2, 4, false>(vector<2, 4, false>& v, const double* p) {
		v.x[0] = _mm_load_pd(p);
		v.x[1] = _mm_load_pd(p+2);
	}
	template <>
	vector<2, 4, true>& load<2, 4, true>(vector<2, 4, true>& v, const double* p) {
		v.x[0] = _mm_load_pd(p-2);
		v.x[1] = _mm_load_pd(p-4);
	}

	template <>
	vector<2, 8, false>& load<2, 8, false>(vector<2, 8, false>& v, const double* p) {
		v.x[0] = _mm_load_pd(p);
		v.x[1] = _mm_load_pd(p+2);
		v.x[2] = _mm_load_pd(p+4);
		v.x[3] = _mm_load_pd(p+6);
	}
	template <>
	vector<2, 8, true>& load<2, 8, true>(vector<2, 8, true>& v, const double* p) {
		v.x[0] = _mm_load_pd(p-2);
		v.x[1] = _mm_load_pd(p-4);
		v.x[2] = _mm_load_pd(p-6);
		v.x[3] = _mm_load_pd(p-8);
	}

	template <>
	vector<2, 16, false>& load<2, 16, false>(vector<2, 16, false>& v, const double* p) {
		v.x[0] = _mm_load_pd(p);
		v.x[1] = _mm_load_pd(p+2);
		v.x[2] = _mm_load_pd(p+4);
		v.x[3] = _mm_load_pd(p+6);
		v.x[4] = _mm_load_pd(p+8);
		v.x[5] = _mm_load_pd(p+10);
		v.x[6] = _mm_load_pd(p+12);
		v.x[7] = _mm_load_pd(p+14);
	}
	template <>
	vector<2, 16, true>& load<2, 16, true>(vector<2, 16, true>& v, const double* p) {
		v.x[0] = _mm_load_pd(p-2);
		v.x[1] = _mm_load_pd(p-4);
		v.x[2] = _mm_load_pd(p-6);
		v.x[3] = _mm_load_pd(p-8);
		v.x[4] = _mm_load_pd(p-10);
		v.x[5] = _mm_load_pd(p-12);
		v.x[6] = _mm_load_pd(p-14);
		v.x[7] = _mm_load_pd(p-16);
	}


	//Store


}
