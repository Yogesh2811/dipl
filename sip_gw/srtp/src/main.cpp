#include "cl_util.h"

#include <vector>
#include <iostream>

using namespace std;

int main(int argc, char* argv[]) {
    initOpenCL();

    vector<int> vec;
    vec.push_back(1);
    
    for(auto b : vec){
	cout << (*b); 
    }

    return 0;
}