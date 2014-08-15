#include <iostream>
#include <vector>
#include <algorithm>
#include <iterator>

#include <assert.h>

typedef std::chrono::high_resolution_clock Clock;

template<typename T>
void print( const std::vector<T>& V ) {
    std::copy(V.begin(), V.end(), std::ostream_iterator<T>(std::cout,","));
    std::cout << "\n";
}

//! Return the highest power of 3 less or equal to the array lenght (2n)
size_t highest_leader( size_t len )
{
    size_t i = 1;
    while ( i * 3 < len ) i *= 3;
    return i;
}

//! Permutation (sigma) function returning the next index in the cycle. Permutation indices start at 1.
inline size_t next_index( size_t j, size_t len )
{
    return ( j * 2 ) % ( len + 1 );
}

//! Apply the cycle leader algorithm for the in-shuﬄe permutation
template<typename T>
void cycle_rotate( std::vector<T>& A, size_t leader, size_t section_offset, size_t section_len )
{
    size_t i = next_index(leader, section_len);
    while( i != leader ) {
        std::swap( A[section_offset + i - 1], A[section_offset + leader - 1] );
        i = next_index(i, section_len);
    }
}

template<typename T>
void interleave_inplace( std::vector<T>& A )
{
	if (A.size() <= 2)
		return;
	
	// Array size should be even
	assert( A.size() % 2 == 0 );
	
   	auto it = A.begin();
	size_t m=0, n=1;
	
	while( m < n ) {
		size_t section_len = std::distance( it, A.end() );
		size_t h = highest_leader(section_len);
		m = ( h > 1 ) ? h/2 : 1;
		n = section_len/2;
		
		//right cyclic shift
		std::rotate( it + m, it + n, it + m + n );
		
		for( size_t leader = 1; leader < 2*m; leader *= 3 ) {
			cycle_rotate<T>( A, leader, std::distance( A.begin(), it ), 2*m );
		}
		//move iterator to remaining un-interleaved section
		std::advance(it, 2*m);
    }
}

template <typename T>
void interleave( std::vector<T>& A )
{
	std::vector<T> B(A);
	size_t half = A.size() / 2;

	for( size_t i =0; i < B.size(); i += 2 ) {
		A[i]	= B[half + i/2];
		A[i+1]	= B[i/2];
	}
}

int main(int argc, const char * argv[])
{
	int length = 256;
	int nb_applications = 1;
	
    std::vector<int> A(length);
    for(int i=0; i<A.size(); i++) {
        A[i] = ( i < A.size()/2 ) ? -i-1 : int(i - A.size()/2)+1;
    }
	std::vector<int> B(A);
	
	{
		std::cout << "Trivial copy algo: ";
		print(A);
		auto start = Clock::now();
		for( int i=0; i < nb_applications; ++i ) {
			interleave( A );
		}
		print(A);
		
		auto end = Clock::now();
		auto nsdur = std::chrono::duration_cast<std::chrono::nanoseconds>( end - start );
		auto msdur = std::chrono::duration_cast<std::chrono::milliseconds>( end - start );
		std::cout << nsdur.count() << "ns,  " << msdur.count() << "ms" << std::endl;
	}
	
	{
		std::cout << "Inplace algo: ";
		auto start = Clock::now();
		
		print(B);
		for( int i=0; i < nb_applications; ++i ) {
			interleave_inplace( B );
		}
		print(B);

		auto end = Clock::now();
		auto nsdur = std::chrono::duration_cast<std::chrono::nanoseconds>( end - start );
		auto msdur = std::chrono::duration_cast<std::chrono::milliseconds>( end - start );
		std::cout << nsdur.count() << "ns,  " << msdur.count() << "ms" << std::endl;
	}
	
	assert( A == B );
}
