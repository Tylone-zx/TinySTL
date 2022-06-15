#include <iostream>
#include "Deque.h"

template<class T>
using Tsl = TinySTL::deque<T>;

int main(){

	Tsl<int> tsl;
	for (int i = 1; i <= 5; i++) tsl.push_back(i);
	for (auto it = tsl.begin(); it != tsl.end(); it++) std::cout << *it << ' ';
	return 0;
}