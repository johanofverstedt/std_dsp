
#ifndef BUFFER_ALLOCATOR_GUARD
#define BUFFER_ALLOCATOR_GUARD

#include <cstdint>
#include <cassert>

inline
std::int32_t align_size(std::int32_t size) {
	assert(size >= 0);
	if(size & 1 == 1)
		return size + 1;
	return size;
}

template<std::int32_t SIZE>
class stack {
private:
	std::int32_t pos_;
	SSE_ALIGN double memory_[SIZE];

#ifdef DEBUG_THE_STACK
	std::int32_t max_used_;
#endif
public:
	class buffer {
	private:
		stack* owner_;
		std::int32_t size_;
		double* ptr_;
	public:
		buffer(stack* owner, std::int32_t size) : owner_(owner) {
			size_ = align_size(size);
			ptr_ = &owner->memory[owner->pos_];
			owner->pos_ += size_;
#ifdef DEBUG_THE_STACK
			if(owner->max_pos_ < owner->pos_)
				owner->max_pos_ = owner->pos_;
#endif
		}
		buffer(const buffer&) = delete;
		buffer(buffer&&) = delete;
		~buffer() {
			owner_->pos -= size_;
		}

		inline
		double* get() {
			return ptr_;
		}
	};

	stack() : pos_(0) {
#ifdef DEBUG_THE_STACK
		max_used_ = 0;
#endif
		;
	}
	~stack() {
		assert(pos_ == 0);
	}

#ifdef DEBUG_THE_STACK
	std::int32_t get_max_used() {
		return max_used_;
	}
#endif
};

using stack_buffer = stack::buffer;

//
// Allocate a stack buffer by:
// stack_buffer buf(stack, count);
// buf.get() yields a pointer to the buffer
//

#endif
