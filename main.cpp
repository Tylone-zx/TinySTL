#include <iostream>
#include "RBTree.h"
#include "Functional.h"

using namespace TinySTL;

rb_tree<int, int, identity<int>, less<int>> itree;

int main(){

	std::cout << itree.size() << '\n';
	itree.insert_unique(5);
	itree.insert_unique(8);
	itree.insert_unique(47);
	itree.insert_unique(23);
	itree.insert_unique(12);
	itree.insert_unique(9);
	itree.insert_unique(8);
	itree.insert_unique(12);
	itree.insert_unique(11);
	std::cout << itree.size() << '\n';
	for (auto it = itree.begin(); it != itree.end(); it++) std::cout << *it << ' ';
	return 0;
}