// tinySTL.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include<vector>
#include<ctime>
#include<Windows.h>
#include<new>
//#include"stl_construct.h"
#include<iostream>
#include"stl_alloc.h"
#include"jjalloc.h"
using namespace std;


int _tmain(int argc, _TCHAR* argv[])
{
vector<int,simple_alloc<int,alloc>>vec(2);
cout << "a";
}

