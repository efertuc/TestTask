
#include <iostream>
#include <list>
#include <iterator>
#include <time.h>
#include <thread>
#include <bitset>
#include <mutex>

using namespace std;

const int N = 100; //размер списка 
mutex m; 

struct Info {
	int num_bits; //количество нулевых или единичных битов 
	int num_element; // количество пройденных элементов
};

static Info res_first;
static Info res_second;

//генерация списка случайными значениями
void generate_list(list <int>& lst) 
{ 
	for (int i = 0; i < N; i++) {
		lst.push_back(rand());
	}
#if DEBUG
	copy(lst.begin(), lst.end(), ostream_iterator<int>(cout, " "));
#endif
}

//функция для подсчета единичных битов в значениях элементов
int count_bitset(int temp) {
	bitset<numeric_limits<int>::digits>bitset(temp);
	return bitset.count();
}

 //функция определяющая работу потоков
void* parallel_check_list(bool bits, list <int>* lst) 
{
	int temp;
	if(bits) // если bits = true, то первым потоком выполняется поиск нулевых битов с начала
	{
		while (!lst->empty()) {
			m.lock();
			temp = lst->front();
			lst->pop_front();
			m.unlock();

			int zeroes = sizeof(int)*8 - count_bitset(temp);
			printf("in element %d - %d zeroes \n", temp, zeroes);
			res_first.num_element++;
			res_first.num_bits += zeroes;	
		}	
	}
	else { // второй поток выполняет поиск единичных битов с конца
		while (!lst->empty()) {
			m.lock();
			temp = lst->back();
			lst->pop_back();
			m.unlock();

			int ones = count_bitset(temp);
			printf("in element %d - %d ones \n", temp, ones);
			res_second.num_element++;
			res_second.num_bits += ones;
			
		}	
	}
	return 0;
}

int main()
{
	
	setlocale(LC_ALL, "rus");
	srand(time(0));
	list <int> lst;
	generate_list(lst);

	thread thr(parallel_check_list, 1, &lst);
	thread thr2(parallel_check_list, 0, &lst);

	thr.join();
	thr2.join();

	cout << "Количество посчитаных нулевых битов: " << res_first.num_bits << "\t||\t" << " Количество пройденных элементов " << res_first.num_element << endl;
	cout << "Количество посчитаных единичных битов: " << res_second.num_bits << "\t||\t" << " Количество пройденных элементов " << res_second.num_element << endl;

	return 0;
}