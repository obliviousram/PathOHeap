#include "RandomForOHeapLCG.h"
#include <stdexcept>

bool RandomForOHeapLCG::is_initialized = false;

RandomForOHeapLCG::RandomForOHeapLCG() {
  if (RandomForOHeapLCG::is_initialized) {
    throw std::runtime_error("Only one random instance can be used at a time!");
  }

  this->bound = -1;
  rnd_generator.seed(0L);
  this->seed = (seed ^ 0x5DEECE66DL) & ((1L << 48) - 1);
  RandomForOHeapLCG::is_initialized  = true;
}

int RandomForOHeapLCG::getRandomLeaf() {
  if (this->bound == -1) {
    throw std::runtime_error("Bound is not set.");
  }

  long next = (seed * 0x5DEECE66DL + 0xBL) & ((1L << 48) - 1);
  this->seed = next;
  int bits = next >> (48 - 31);
  if ((this->bound & -this->bound) == this->bound) {  // if bound is a power of 2
    return (int)((this->bound * (long)bits) >> 31);
  }
  int val = bits % this->bound;
  while (bits - val + (this->bound-1) < 0) {
    next = (seed * 0x5DEECE66DL + 0xBL) & ((1L << 48) - 1);
    this->seed = next;
    bits = next >> (48 - 31);
    val = bits % this->bound;
  }
  return val;
}

void RandomForOHeapLCG::setBound(int num_leaves) {
  this->bound = num_leaves;
}
