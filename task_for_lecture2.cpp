#include <cilk/cilk.h>
#include <cilk/cilk_api.h>
#include <cilk/reducer_max.h>
#include <cilk/reducer_min.h>
#include <cilk/reducer_vector.h>
#include <chrono>
#include <iostream>

using namespace std::chrono;
using namespace std;

/// ������� ReducerMaxTest() ���������� ������������ ������� �������,
/// ����������� �� � �������� ���������, � ��� �������
/// mass_pointer - ��������� �������� ������ ����� �����
/// size - ���������� ��������� � �������
void ReducerMaxTest(int *mass_pointer, const long size)
{
	cilk::reducer<cilk::op_max_index<long, int>> maximum;
	cilk_for(long i = 0; i < size; ++i)
	{
		maximum->calc_max(i, mass_pointer[i]);
	}
	printf("Maximal element = %d has index = %d\n\n",
		maximum->get_reference(), maximum->get_index_reference());
}

void ReducerMinTest(int *mass_pointer, const long size)
{
	cilk::reducer<cilk::op_min_index<long, int>> minimun;
	cilk_for(long i = 0; i < size; ++i){
		minimun->calc_min(i, mass_pointer[i]);
	}
	printf("Minimal element = %d has index = %d\n\n",
		minimun->get_reference(), minimun->get_index_reference());
}


/// ������� ParallelSort() ��������� ������ � ������� �����������
/// begin - ��������� �� ������ ������� ��������� �������
/// end - ��������� �� ��������� ������� ��������� �������
void ParallelSort(int *begin, int *end)
{
	if (begin != end)
	{
		--end;
		int *middle = std::partition(begin, end, std::bind2nd(std::less<int>(), *end));
		std::swap(*end, *middle);
		cilk_spawn ParallelSort(begin, middle);
		ParallelSort(++middle, ++end);
		cilk_sync;
	}
}

void CompareForAndCilk_For(size_t sz){

	std::vector <int> array;
	cilk::reducer<cilk::op_vector<int>> red_vec;

	// ���� FOR
	high_resolution_clock::time_point t1 = high_resolution_clock::now();
	for (int i = 0; i < sz; i++){
		array.push_back(rand() % 20000 + 1);
	}
	high_resolution_clock::time_point t2 = high_resolution_clock::now();

	duration<double> durationFor = t2 - t1;


	high_resolution_clock::time_point t1_cilk = high_resolution_clock::now();
	for (int i = 0; i < sz; i++){
		red_vec->push_back(rand() % 20000 + 1);
	}
	high_resolution_clock::time_point t2_cilk = high_resolution_clock::now();
	duration<double> durationForCilk = t2_cilk - t1_cilk;

	cout << "--------------------------------------" << endl;
	cout << "Size" << sz << endl;
	cout << "Duration  cycle <for> :" << durationFor.count() << "sec" << endl;
	cout << "Duration  cycle <forCilk> :" << durationForCilk.count() << "sec" << endl;
}


int main()
{
	srand((unsigned)time(0));

	// ������������� ���������� ���������� ������� = 4
	__cilkrts_set_param("nworkers", "4");

	long i;
	const long mass_size = 500000;
	int *mass_begin, *mass_end;
	int *mass = new int[mass_size];

	for(i = 0; i < mass_size; ++i)
	{
		mass[i] = (rand() % 25000) + 1;
	}

	mass_begin = mass;
	mass_end = mass_begin + mass_size;
	ReducerMaxTest(mass, mass_size);
	ReducerMinTest(mass, mass_size);





	high_resolution_clock::time_point t1 = high_resolution_clock::now();
	ParallelSort(mass_begin, mass_end);
	high_resolution_clock::time_point t2 = high_resolution_clock::now();

	duration<double> duration = (t2 - t1);
	cout << "Duration is:" << duration.count() << "seconds" << "\r\n" << endl;

	ReducerMaxTest(mass, mass_size);
	ReducerMinTest(mass, mass_size);

	CompareForAndCilk_For(10);
	CompareForAndCilk_For(50);
	CompareForAndCilk_For(100);
	CompareForAndCilk_For(500);
	CompareForAndCilk_For(1000);
	CompareForAndCilk_For(10000);
	CompareForAndCilk_For(100000);
	CompareForAndCilk_For(1000000);


	delete[]mass;
	system("pause");
	return 0;
}
