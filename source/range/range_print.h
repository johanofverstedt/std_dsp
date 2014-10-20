
#pragma once

#include <strstream>
#include <typeinfo>
#include <iterator>

namespace std_dsp {

	template <typename R>
	std::string range_to_array_string(R r, std::string name) {
		std::strstream str;
		
		str << typeid(std::iterator_traits<decltype(range.begin())>::value_type).name() << " " << name << "[" << r.size() << "] = ";

		str << "{ ";

		auto i = r.begin();
		auto end_it = r.end();
		if(i != end_it) {
			str << (*i);
			++i;
		}
		for(; i != end_it; ++i) {
			str << ", ";
			str << (*i);
		}

		str << " };";

		return str.str();
	}

}
