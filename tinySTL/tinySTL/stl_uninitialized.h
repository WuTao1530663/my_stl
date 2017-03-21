#ifndef __UNINITIALIZED__
#define __UNINITIALIZED__
#include<type_traits>
#include<iterator>
#include"stl_construct.h"
#include"stl_algorithm.h"
namespace WT{
	/*
	* 	uninitialized_copy������ȡ��������ָ���ͣ����������Ƿ�ΪPOD(Plain Old Data,
	��Ȼ��trivial ���죬��������������͸�ֵ����)����ģ�庯�������Ƶ����ƾ�����һ��
	ִ�еĺ�����non-POD��һ��һ��ִ�й��캯����POD������ֱ�ӵ����ϲ��㷨copy()�����
	char*��wchar_t*�����ͱ𣬿��Բ���memove(ֱ���ƶ��ڴ�����)��ִ�и�����Ϊ����˶���
	������ػ��汾��
	*
	*/
	template<class InputIterator, class ForwardIterator>
	ForwardIterator	uninitialized_copy(InputIterator first, InputIterator last, ForwardIterator result);
	
	template<class InputIterator, class ForwardIterator>
	ForwardIterator	uninitialized_copy<char*>(InputIterator first, InputIterator last, ForwardIterator result);
	
	template<class InputIterator, class ForwardIterator>
	ForwardIterator	uninitialized_copy<wchar_t*>(InputIterator first, InputIterator last, ForwardIterator result);

	template<class InputIterator, class ForwardIterator, class T>
	ForwardIterator
		__uninitialized_copy(InputIterator first, InputIterator last, ForwardIterator result, T*);

	template<class InputIterator, class ForwardIterator, class T>
	ForwardIterator	__uninitialized_copy__aux(InputIterator first, InputIterator last, ForwardIterator result, std::true_type);

	template<class InputIterator, class ForwardIterator, class T>
	ForwardIterator	__uninitialized_copy__aux(InputIterator first, InputIterator last, ForwardIterator result, std::false_type);

	
	template<>
	char* uninitialized_copy(char* first, char* last, char* result){
		memmove(result, first, last - first);
		return result + (last - first);
	}
	template<>
	wchar_t* uninitialized_copy(wchar_t*first,wchar_t*last, wchar_t*result){
		memmove(result, first, sizeof(wchar_t)*(last - first));
		return result + (last - first);
	}

	template<class InputIterator, class ForwardIterator>
	ForwardIterator	uninitialized_copy(InputIterator first, InputIterator last, ForwardIterator result){
		return __uninitialized_copy(first, last, result, std::_Val_type(first));
	}

	template<class InputIterator, class ForwardIterator, class T>
	ForwardIterator
		__uninitialized_copy(InputIterator first, InputIterator last, ForwardIterator result, T*){

			return __uninitialized_copy_aux(first, last, result, std::is_pod<T>());
		}

	template<class InputIterator, class ForwardIterator, class T>
	ForwardIterator
		__uninitialized_copy__aux(InputIterator first, InputIterator last, ForwardIterator result, std::true_type){
			copy(first, last, result);
		}

	template<class InputIterator, class ForwardIterator>
	ForwardIterator
		__uninitialized_copy_aux(InputIterator first, InputIterator last, ForwardIterator result, std::false_type){
			ForwardIterator cur = result;
			for (; first != last; ++first, ++cur)
				construct(&*cur, *first);
			return cur;
		}


	



	/* uninitialized_fill_n��uninitialized_fill����,�ֱ�Ϊ[first,first + n)�Լ�[first,last)�����ֵvalue
	ͬ�������Ƿ�ΪPOD����ִ�в�ͬ����
	*
	*/
	
	template<class ForwardIterator, class Size, class T>
	void uninitialized_fill_n(ForwardIterator first,Size n, const T&);
	template<class ForwardIterator, class Size, class T,class T1>
	void __uninitialized_fill_n(ForwardIterator first,Size n, const T&,T1*);

	template<class ForwardIterator, class Size, class T>
	void __uninitialized_fill_n_aux(ForwardIterator first, Size n, const T&,std::false_type);

	template<class ForwardIterator, class Size, class T>
	void __uninitialized_fill_n_aux(ForwardIterator first, Size n, const T&, std::true_type);

	template<class ForwardIterator,  class T>
	void uninitialized_fill(ForwardIterator first, ForwardIterator last, const T&);
	template<class ForwardIterator, class T, class T1>
	void __uninitialized_fill(ForwardIterator first, ForwardIterator last, const T&, T1*);

	template<class ForwardIterator,  class T>
	void __uninitialized_fill_aux(ForwardIterator first, ForwardIterator last, const T&, std::false_type);

	template<class ForwardIterator, class T>
	void __uninitialized_fill_aux(ForwardIterator first, ForwardIterator last, const T&, std::true_type);



	
	template<class ForwardIterator, class Size, class T>
	void uninitialized_fill_n(ForwardIterator first, Size n, const T& value){
		__uninitialized_fill_n(first,  n, value, std::_Val_type(first));
	}
	template<class ForwardIterator, class Size, class T, class T1>
	void __uninitialized_fill_n(ForwardIterator first, Size n, const T& value, T1*){
		__uninitialized_fill_n_aux(first, n, value, std::is_pod<T1>());
	}
	template<class ForwardIterator, class Size, class T>
	void __uninitialized_fill_n_aux(ForwardIterator first,Size n, const T& value, std::false_type){
		for (; n != 0; ++first, --n)
			WT::construct(&*first, value);
	}
	template<class ForwardIterator, class Size, class T>
	void __uninitialized_fill_n_aux(ForwardIterator first, Size n, const T& value, std::true_type){
		WT::fill_n(first, n, value);
	}


	template<class ForwardIterator,  class T>
	void uninitialized_fill(ForwardIterator first, ForwardIterator last, const T& value){
		return __uninitialized_fill(first, last,value, std::_Val_type(first));
	}
	template<class ForwardIterator, class T, class T1>
	void __uninitialized_fill(ForwardIterator first, ForwardIterator last, const T& value, T1*){
		__uninitialized_fill_aux(first, last, value, std::is_pod<T1>());
	}

	template<class ForwardIterator,  class T>
	void __uninitialized_fill_aux(ForwardIterator first, ForwardIterator last, const T& value, std::false_type){
		for (; first != last; ++first)
			WT::construct(&*first, value);
	}

	template<class ForwardIterator, class T>
	void __uninitialized_fill_aux(ForwardIterator first, ForwardIterator last, const T& value, std::true_type){
		WT::fill(first, last, value);
	}

}

#endif
