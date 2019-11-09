#ifndef POHEAP_BLOCK_H
#define POHEAP_BLOCK_H

#include <array>
#include <cstddef>
#include <iostream>

class Block {
 public:
  static const int BLOCK_SIZE = 32;

  int leaf_id;  // aka pos
  long t;  // tau or timestamp
  int k;
  std::array<std::byte, BLOCK_SIZE> v;

  Block(int leaf_id, long t, int k, std::array<std::byte, BLOCK_SIZE> v);
  Block();  /** dummy index */
};

bool operator<(const Block& b1, const Block& b2);
bool operator==(const Block& b1, const Block& b2);
bool operator!=(const Block& b1, const Block& b2);
std::ostream& operator<<(std::ostream& out, const Block& b);

#endif
