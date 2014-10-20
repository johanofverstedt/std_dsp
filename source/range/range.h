
#pragma once

#include <cstdint>
#include <cassert>
#include <iterator>

namespace std_dsp {
	template <std::int32_t CHANNELS, bool REVERSE=false>
	class range;

	template <std::int32_t CHANNELS>
	class range<CHANNELS, false> {
	private:
		double* source_;
		std::int32_t count_;
	public:
		enum channels_enum {
			channels = CHANNELS,
		};
		enum reversed_enum {
			reversed = 0,
		}

		range() : count_(0) {}
		range(double* source, std::int32_t count) : source_(source), count_(count) {}
		range(const range& that) = default;
		range(range&& that) = default;

		range& operator=(const range& that) = default;
		range& operator=(const range&& that) = default;

		double* begin() { return source_; }
		const double* begin() const { return source_; }
		const double* cbegin() const { return source_; }
		double* end() { return source_ + (CHANNELS * count_); }
		const double* end() const { return source_ + (CHANNELS * count_); }
		const double* end() const { return source_ + (CHANNELS * count_); }

		void advance(std::int32_t n) {
			assert(n <= count_);
			source_ += (CHANNELS * n);
			count_ -= n;
		}

		range take(std::int32_t n) const {
			assert(n <= count_);
			return range(source_, count_ - n);
		}
		range drop(std::int32_t n) const {
			assert(n <= count_);
			return range(source_ + (CHANNELS * n), count_ - n);
		}
		range take_and_advance(std::int32_t n) {
			range r = take(n);
			advance(n);
			return r;
		}
		std::pair<range, range> split(std::int32_t n) {
			assert(n <= count_);
			assert(n >= 0);
			std::pair<range, range>(take(n), drop(n));
		}

		std::int32_t size() {
			return count_;
		}

		range<CHANNELS, true> reverse() const {
			return range<CHANNELS, true>(source + (CHANNELS * count_), count_);
		}
	};		

	template <std::int32_t CHANNELS>
	class range<CHANNELS, true> {
	private:
		double* source_;
		std::int32_t count_;
	public:
		enum channels_enum {
			channels = CHANNELS,
		};
		enum reversed_enum {
			reversed = 1,
		}

		range() : count_(0) {}
		range(double* source, std::int32_t count) : source_(source), count_(count) {}
		range(const range& that) = default;
		range(range&& that) = default;

		range& operator=(const range& that) = default;
		range& operator=(const range&& that) = default;
		
		std::reverse_iterator<double*> begin() { return source_; }
		std::reverse_iterator<const double*> begin() const { return source_; }
		std::reverse_iterator<const double*> cbegin() const { return source_; }
		std::reverse_iterator<double*> end() { return source_ + (CHANNELS * count_); }
		std::reverse_iterator<const double*> end() const { return source_ + (CHANNELS * count_); }
		std::reverse_iterator<const double*> end() const { return source_ + (CHANNELS * count_); }

		inline
		double get(std::int32_t index) {
			return source_[-index-1];
		}
		inline
		double_2 get2(std::int32_t index) {

		}

		void advance(std::int32_t n) {
			assert(n <= count_);
			assert(n >= 0);
			source_ -= (CHANNELS * n);
			count_ -= n;
		}

		range take(std::int32_t n) {
			assert(n <= count_);
			assert(n >= 0);
			return range(source_, count_ - n);
		}
		range drop(std::int32_t n) {
			assert(n <= count_);
			assert(n >= 0);
			return range(source_ - (CHANNELS * n), count_ - n);
		}
		range take_and_advance(std::int32_t n) {
			range r = take(n);
			advance(n);
			return r;
		}
		std::pair<range, range> split(std::int32_t n) {
			assert(n <= count_);
			assert(n >= 0);
			std::pair<range, range>(take(n), drop(n));
		}

		std::int32_t size() {
			return count_;
		}

		range<CHANNELS, false> reverse() {
			return range<CHANNELS, false>(source - (CHANNELS * count_), count_);
		}
	};

}
