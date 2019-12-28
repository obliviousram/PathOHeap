#ifndef POHEAP_RANDOMFOROHEAP_H
#define POHEAP_RANDOMFOROHEAP_H

#include "RandForOHeapInterface.h"
#include "duthomhas/csprng.hpp"

class RandomForOHeap : public RandForOHeapInterface {
 public:
  RandomForOHeap();

  /** Returns a random integer between 0 (inclusive) and bound (exclusive) */
  int getRandomLeaf();
  
  void setBound(int num_leaves);

 private:
  static bool is_initialized;
  int bound;
  duthomhas::csprng rng;
};

#endif 
