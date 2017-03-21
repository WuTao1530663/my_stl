#ifndef __ALGORITHM__
#define __ALGORITHM__

namespace WT{
	template<class OutputIterator, class Size, class T>
	OutputIterator fill_n(OutputIterator first, Size n, const T& value);

	template<class InputIterator, class T>
	void fill(InputIterator first, InputIterator last, const T&value);

	template<class InputIterator, class OutputIterator>
	OutputIterator copy(InputIterator first, InputIterator last, OutputIterator result);


	template<class InputIterator, class OutputIterator>
	OutputIterator copy(InputIterator first, InputIterator last, OutputIterator result){
		for (; first != last; ++first, ++result)
			result = *first;
		return result;
	}

	template<class InputIterator, class T>
	void fill(InputIterator first, InputIterator last, const T&value){
		for (; first != last; ++first)
			*first = value;
	}

	template<class InputIterator, class Size, class T>
	InputIterator fill_n(InputIterator first, Size n, const T&value){
		for (; n > 0; --n, ++first)
			*first = value;
		return first;
	}
}
#endif