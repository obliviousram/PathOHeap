#ifndef POHEAP_RANDOMFOROHEAP_H
#define POHEAP_RANDOMFOROHEAP_H

#include "RandForOHeapInterface.h"
#include <random>

class RandomForOHeap : public RandForOHeapInterface {
 public:
  RandomForOHeap();

  /** Returns a random integer between 0 (inclusive) and bound (exclusive) */
  int getRandomLeaf();
  
  void setBound(int num_leaves);

 private:
  static bool is_initialized;
  int bound;
  std::linear_congruential_engine<unsigned long, 0x5DEECE66DL, 11, 281474976710656> rnd_generator;
  long seed = 0L;
};

#endif 
