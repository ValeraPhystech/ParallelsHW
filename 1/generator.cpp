#include <condition_variable>
#include <cstddef>
#include <deque>
#include <iostream>
#include <stdio.h>
#include <pthread.h>
#include <vector>
#include <unistd.h>
#include <omp.h>
#include <mutex>
#include <stdexcept>
#include <string>
#include <fstream>



using std::ofstream;
using std::string;

int main() {


  ofstream fout("input.txt");

  string s("1");
  for( int i = 0; i < 1000; i++){
    s+="1";
  }
  for( int i = 0; i < 10000; i++){
    fout << s << "\n";
  }

  fout.close();

  return 0;
}
