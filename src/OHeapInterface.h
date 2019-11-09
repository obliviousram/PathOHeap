#ifndef POHEAP_OHEAPINTERFACE_H
#define POHEAP_OHEAPINTERFACE_H

#include "Block.h"
#include <optional>
#include <vector>
#include <utility>

class OHeapInterface {
 public:
  virtual Block findMin() = 0;
  virtual std::pair<int, int> insertBlock(int k, std::array<std::byte, Block::BLOCK_SIZE> v) = 0;
  virtual Block deleteBlock(int leaf, long t) = 0;
  virtual Block extractMin() = 0;
  virtual std::pair<int, int> decreaseKey(int leaf, long t) = 0;
  virtual std::pair<int, int> increaseKey(int leaf, long t) = 0;
  virtual int P(int leaf, int level) = 0;
  virtual std::vector<Block>& getStash() = 0;
  virtual int getStashSize() = 0;
  virtual int getNumLeaves() = 0;
  virtual int getNumLevels() = 0;
  virtual int getNumBlocks() = 0;
  virtual int getNumBuckets() = 0;
};

#endif
