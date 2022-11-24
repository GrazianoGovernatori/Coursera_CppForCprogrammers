//Governatori___Coursera_Cpp_01.cpp
//C program converted to C++
//given a vector, we initialize and sum all elements

#include <iostream>
#include <vector>

using namespace std;

//size of data array
const int N = 40;

//inline function to sum elements of a vector of int
inline int sum(vector<int> inputVec) {
    int localSum = 0;
    for (const int& i : inputVec) {
        localSum += i;
    }
    return localSum;
}

int main()
{
    vector<int> data(N, 0);      //init vector with zeroes
    vector<int>::iterator iter;  //create an iterator to loop through vector elements

    // assign elements in vector a value
    iter = data.begin();
    int i = 0;
    while (iter != data.end()) {
        *iter = i++;
        iter++;
    }

    //to print elements, uncomment next lines
    //for (iter = data.begin(); iter != data.end(); ++iter) {
    //    cout << *iter;
    //}

    cout << endl << "sum is " << sum(data);
}
