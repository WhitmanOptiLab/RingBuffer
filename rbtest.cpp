#include "ringbuffer.hpp"
#include <cassert>
#include <iostream>

int main() {
   RingBuffer<int> rbf;
   rbf[0] = 1;
   int num_elements;
   for (int count = 2; count < num_elements; count++) {
     rbf.extend();
     rbf[0] = count;
     for (int i = 0; i < count; i++) {
       assert(rbf[i] == count - i);
     }
   }
   for (int count = 0; count < 100; count++) {
     rbf.advance();
     rbf[0] = count+num_elements;
     for (int i = 0; i < num_elements; i++) {
       assert(rbf[i] == count + num_elements - i);
     }
   }
   std::cout << "RB tests passed!" << std::endl;
}