#include "RandomForOHeap.h"
#include <stdexcept>

bool RandomForOHeap::is_initialized = false;

RandomForOHeap::RandomForOHeap() {
  if (RandomForOHeap::is_initialized) {
    throw std::runtime_error("Only one random instance can be used at a time!");
  }

  this->bound = -1;
  this->rng(long());
  RandomForOHeap::is_initialized  = true;
}

int RandomForOHeap::getRandomLeaf() {
  if (this->bound == -1) {
    throw std::runtime_error("Bound is not set.");
  }

  long next = this->rng();
  int randVal = next % this->bound;
  if (randVal < 0) {
    randVal += this->bound;
  }
  return randVal;
}

void RandomForOHeap::setBound(int num_leaves) {
  this->bound = num_leaves;
}
