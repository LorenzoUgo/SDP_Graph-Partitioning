#define MUTATION

#include "Individual/Individual.cpp"
#include <iostream>

using namespace std;

int main(int argc, char** argv) {
	Individual I1(4,10), I2(4,10), I3(4,10);

	cout << "I1: ";
	for(auto g: I1.getGenotype())
		cout << g << " ";
	cout << endl;

	cout << "I2:  ";
	for(auto g: I2.getGenotype())
		cout << g << " ";
	cout << endl;

	cout << "I3:  ";
	for(auto g: I3.getGenotype())
		cout << g << " ";
	cout << endl;

	cout << endl << "Mutating I1, I2, I3" << endl << endl;
	I1.mutation();
	I2.mutation();
	I3.mutation();

	cout << "I1: ";
	for(auto g: I1.getGenotype())
		cout << g << " ";
	cout << endl;

	cout << "I2:  ";
	for(auto g: I2.getGenotype())
		cout << g << " ";
	cout << endl;

	cout << "I3:  ";
	for(auto g: I3.getGenotype())
		cout << g << " ";
	cout << endl;

	return 0;
}