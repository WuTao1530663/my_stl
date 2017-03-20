#include<new>
#include<iostream>
#include<type_traits>
template<class T1,class T2>
inline void construct(T1 *p, const T2& value){
	new (p)T1(value); //placement new;call T1::T1(value);
}

//destroy第一个版本，接受一个指针
template<class T>
inline void destroy(T* pointer){
	pointer->~T();
}

//第二个版本接受两个迭代器，释放一个范围的内存，利用type_traits<>根据不同类型求得最适当的措施
template<class ForwardIterator>
inline void destroy(ForwardIterator first, ForwardIterator last){
	__destroy(first, last, std::_Val_type(first));
}

template<class ForwardIterator,class T>
inline void __destroy(ForwardIterator first, ForwardIterator last, T*){
	__destroy_aux(first, last, std::has_trivial_destructor<T>());
}

template<class ForwardIterator>
inline void __destroy_aux(ForwardIterator first, ForwardIterator last, std::false_type){
	std::cout << "in false_type" << std::endl;
	for (; first < last; ++first)
		destroy(&(*first));
}

template<class ForwardIterator>
inline void __destroy_aux(ForwardIterator first, ForwardIterator last, std::true_type){
	std::cout << "in true_type" << std::endl;
}
