#include<cstdlib>

template<int inst>
class __malloc_alloc_template{
private:
	static void *oom_malloc(size_t);
	static void *oom_realloc(void *, size_t);
	static void(*__malloc_alloc_oom_handler)();
public:


	static void *allocate(size_t n){
		void *result = malloc(n);
		if (0 == result) result = oom_malloc(n);
		return result;
	}

	static void deallocate(void *p, size_t){
		free(p);
	}

	static void *reallocate(size_t n,size_t new_sz){
		void *result = realloc(n,new_sz);
		if (0 == result) result = oom_realloc(n);
		return result;
	}

	static void(*set_malloc_handler(void(*f)()))(){
		void(*old)() = __malloc_alloc_oom_handler;
		__malloc_alloc_oom_handler = f;
		return old;
	}
	
};
template<int inst>
void(*__malloc_alloc_template<inst>::__malloc_alloc_oom_handler)() = 0;
template<int inst>
void *__malloc_alloc_template<inst>::oom_malloc(size_t n){
	void(*my_malloc_handler)();
	void *result;
	while (1){
		my_malloc_handler = __malloc_alloc_oom_handler;
		if (0 == my_malloc_handler){
			exit(1);
		}
		(*my_malloc_handler)();
		result = malloc(n);
		if (result) return result;
	}
}
template<int inst>
void *__malloc_alloc_template<inst>::oom_realloc(void *p, size_t n){
	void(*my_malloc_handler)();
	void *result;
	while (1){
		my_malloc_handler = __malloc_alloc_oom_handler;
		if (0 == my_malloc_handler){
			exit(1);
		}
		(*my_malloc_handler)();
		result = realloc(p, n);
		if (result) return result;
	}
}


enum {__NOBJS = 20};//ÿ�����freelist�Ľڵ���
enum {__ALIGN = 8};// С��������ϵ��߽�
enum {__MAX_BYTES = 128};//С�����������
enum {__NFREELISTS = __MAX_BYTES / __ALIGN};//free-lists �ĸ��� 
template<bool threads,int inst>
class __default_alloc_template{
	private:
		static size_t ROUND_UP(size_t bytes){
			return (((bytes)+__ALIGN - 1) &~(__ALIGN - 1));
		}
private:

		union obj{
			obj *free_list_link;
			char client_data[1];
		};
	private:
		static obj * volatile free_list[__NFREELISTS];
		static size_t FREELIST_INDEX(size_t bytes){
			return (((bytes)+__ALIGN - 1) / __ALIGN - 1);
		}
		static void *refill(size_t n);
		static char*chunk_alloc(size_t size, int &nobjs);

		static char *start_free;
		static char *end_free;
		static size_t heap_size;
public:
	static void *allocate(size_t n);
	static void deallocate(void *p, size_t n);
	static void *reallocate(void *p, size_t old_sz, size_t new_sz);
	~__default_alloc_template();
	

};
template<bool threads, int inst>
__default_alloc_template<threads, inst>::~__default_alloc_template(){
	cout << "in" << endl;
	for (obj *list : free_list){
		for (; list; list = list->free_list_link){
			free(list);
		}
	}
}
template<bool threads, int inst>
char *__default_alloc_template<threads, inst>::start_free = 0;

template<bool threads, int inst>
char *__default_alloc_template<threads, inst>::end_free = 0;


template<bool threads, int inst>
size_t __default_alloc_template<threads, inst>::heap_size = 0;


template<bool threads, int inst>
typename __default_alloc_template<threads, inst>::obj * volatile __default_alloc_template<threads, inst>::free_list[__NFREELISTS] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

template<bool threads, int inst>
void * __default_alloc_template<threads, inst>::allocate(size_t n){
	obj *volatile * my_free_list;
	obj *result;
	if (n > (size_t)__MAX_BYTES)
		return (malloc_alloc::allocate(n));
	//Ѱ��free-lists���ʵ���һ��list
	my_free_list = free_list + FREELIST_INDEX(n);
	result = *my_free_list;
	if ( 0 == result){//���û�п��õ�free-list���������free-list
		void *r = refill(ROUND_UP(n));
		return r;
	}
	//��free-list���׸���ȡ������������һ����
	*my_free_list = result->free_list_link;
	return result;
}

template<bool threads, int inst>
void  __default_alloc_template<threads, inst>::deallocate(void *p, size_t n){
	obj *q = (obj *)p;
	obj * volatile *my_free_list;
	if (n > (size_t)__MAX_BYTES){
		malloc_alloc::deallocate(p, n);
		return;
	}
	my_free_list = free_list + FREELIST_INDEX(n);
	//���ͷŵ��ڴ���뵽free-list���ײ�
	q->free_list_link = *my_free_list;
	*my_free_list = q;
}

template<bool threads, int inst>
void * __default_alloc_template<threads, inst>::refill(size_t n){
	int nobjs = __NOBJS;
	char *chunk = chunk_alloc(n, nobjs);
	obj * volatile *my_free_list;
	obj *result;
	obj * current_obj, *next_obj;
	int i;
	//���ֻ���һ���ڵ���ֱ�ӷ���
	if (1 == nobjs) return chunk;

	my_free_list = free_list + FREELIST_INDEX(n);

	result = (obj *)chunk;
	*my_free_list = next_obj = (obj *)(chunk + n);
	//�����1��ʼ(��0�鷵�ظ�������)���ڵ㴮���γ�����
	for (i = 1;; ++i){
		current_obj = next_obj;
		if (nobjs - 1 == i){ //listβ����linkָ��0
			current_obj->free_list_link = 0;
			break;
		}
		current_obj->free_list_link = next_obj = ((obj *)(next_obj) + n);
	}

}

//size�Ѿ�ROUND_UP
//nobjs��������,��Ϊ�п����޸�����ֵ 
template<bool threads, int inst>
char*  __default_alloc_template<threads, inst>::chunk_alloc(size_t size, int &nobjs){
	char * result;
	size_t total_bytes = size * nobjs;
	size_t bytes_left = end_free - start_free;
	//�ڴ��ʣ���ڴ����������ڴ�Ҫ��,����start_free�������ڴ��ˮλ
	if (bytes_left >= total_bytes){
		result = start_free;
		start_free += total_bytes;
		return result;
	}
	//�ڴ�ز�����ȫ�������󣬵��ǿ��Թ�Ӧһ�����ϵ��ڴ��
	else if (bytes_left >= size){
		nobjs = bytes_left / size;
		total_bytes = size * nobjs;
		result = start_free;
		start_free += total_bytes;
		return result;
	}
	//�ڴ��һ�����鶼�����ṩ
	else{
		size_t bytes_to_get = 2 * total_bytes + ROUND_UP(heap_size >> 4);
		//�����ڴ�ز�����ڴ�,�������ڴ������ʵ�free-list��
		if (bytes_left > 0){
			obj *volatile *my_free_list = free_list + FREELIST_INDEX(bytes_left);
			((obj*)start_free)->free_list_link = *my_free_list;
			*my_free_list = (obj*)start_free;
		}
		start_free = (char*)malloc(bytes_to_get);
		//heap �ռ䲻��
		if (0 == start_free){
			int i;
			obj * volatile *my_free_list, *p;
			//�����ȵ�ǰ�����size�����free-list�Ƿ��п�������
			for (i = size; i <= __MAX_BYTES; i += __ALIGN){
				my_free_list = free_list + FREELIST_INDEX(i);
				p = *my_free_list;
				//free-list���п�������
				if (0 != p){
					*my_free_list = p->free_list_link;
					start_free = (char *)p;
					end_free = start_free + i;
					//�ݹ���ø���i�Ĵ�С����nobjs
					return (chunk_alloc(size, nobjs));
				}

			}

			//���free-list��û���㹻��Ŀ�������,���Ե�һ��allocator�ܹ������ã�������һ��Ҳ������malloc�⺯��
			//���ǵ�һ��allocator�������ڴ治��Ĵ�������п����ͷ�һЩ�ڴ���׳��쳣
			end_free = 0;
			start_free = (char *)malloc_alloc::allocate(bytes_to_get);
		}
		heap_size += bytes_to_get;
		end_free = start_free + bytes_to_get;
		return (chunk_alloc(size, nobjs));

		}
	}





typedef __malloc_alloc_template<0> malloc_alloc;
#ifdef __USE_MALLOC

//��һ��allocator
typedef malloc_alloc alloc;
#else

//�ڶ���allocator
typedef __default_alloc_template<1, 0> alloc;
#endif


template<class T, class Alloc>
class simple_alloc{
public:
	typedef T value_type;
	typedef T* pointer;
	typedef const T* const_pointer;
	typedef T& reference;
	typedef const T& const_reference;
	typedef size_t size_type;
	typedef ptrdiff_t difference_type;

	static T*allocate(size_t n){
		return 0 == n ? 0 : (T*)Alloc::allocate(n*sizeof(T));
	}
	static T*allocate(){
		return (T*)Alloc::allocate(sizeof(T));
	}
	static void deallocate(T *p, size_t n){
		if(0 != n) Alloc::deallocate(p, n * sizeof(T));

	}
	static void deallocate(T *p){
		if (0 != n) Alloc::deallocate(p, sizeof(T));
	}

};