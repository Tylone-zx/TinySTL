#include <iostream>
#include "Vector.h"

template<class T>
using Tsl = TinySTL::vector<T>;

int main(){

	Tsl<int> tsl;
	for (int i = 1; i <= 5; i++) tsl.push_back(i);
	for (auto it = tsl.begin(); it != tsl.end(); it++) std::cout << *it << ' ';
	return 0;
}