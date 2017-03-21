// tinySTL.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include<vector>
#include<ctime>
#include<Windows.h>
#include<new>
//#include"stl_construct.h"
#include<iostream>
#include"stl_uninitialized.h"
#include"stl_alloc.h"
#include<type_traits>

struct int1{
	int val;
	int1() :val(0){}
	int1(int x) :val(x){}
	~int1(){ std::cout << val; }
};
template<class T>
void test(T x){
	__test(x, std::is_pod<T>());
}

template<class T>
void __test(T x, std::false_type){
	std::cout << "in false_type";
}


template<class T>
void __test(T x, std::true_type){
	std::cout << "in true_type";
}
int _tmain(int argc, _TCHAR* argv[])
{
	std::vector<char> vec1(10,'b');
	std::vector<char> vec2(10, 2);
	WT::uninitialized_copy(vec1.begin(),vec1.end(),vec2.begin());
	
	for (char i : vec2)std::cout << i<<std::endl;
	//test(int1());
}

