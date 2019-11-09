#include "PlainHeapComparisonTest.h"
#include "Block.h"
#include "ServerStorage.h"
#include "RandomForOHeap.h"
#include "OHeapReadPathEviction.h"
#include <cmath>
#include <cstdlib>
#include <algorithm>

PlainHeapComparisonTest::PlainHeapComparisonTest() : plain_heap(), max_heap_size(-1) {}

std::array<std::byte, Block::BLOCK_SIZE> PlainHeapComparisonTest::sampleData(int i) {
  std::array<std::byte, Block::BLOCK_SIZE> ret;
  ret.fill(std::byte{0});
  ret.at(0) = static_cast<std::byte>((i >> 24) & 0xFF);
  ret.at(1) = static_cast<std::byte>((i >> 16) & 0xFF);
  ret.at(2) = static_cast<std::byte>((i >> 8) & 0xFF);
  ret.at(3) = static_cast<std::byte>(i & 0xFF);
  return ret;
}

std::pair<PlainHeapComparisonTest::Operation, int> PlainHeapComparisonTest::randomOp() {
  // Bias towards push over pop so the heap eventually fills up, but never push if
  // full or pop if empty.
  Operation op;
  if (this->plain_heap.size() == 0) {
    op = PUSH;
  } else if (this->plain_heap.size() == this->max_heap_size) {
    op = POP;
  } else {
    int r = rand() % 3;
    op = (r >= 1) ? PUSH : POP;
  }

  return std::make_pair(op, rand() % 1000000);
}

void PlainHeapComparisonTest::run() {
  int bucket_size = 3;
  int num_blocks = pow(2, 20);
  this->max_heap_size = num_blocks;
  UntrustedStorageInterface* storage = new ServerStorage();
  RandForOHeapInterface* random = new RandomForOHeap();
  OHeapInterface* oheap = new OHeapReadPathEviction(storage, random, bucket_size, num_blocks, false);

  // Test: compare the OHeap to a 'plain' heap by performing several million random operations on
  // both, making sure that the results match.

  // Since C++ heaps use "pop max" while this OHeap implementation uses "pop min", negate all items
  // in the plain heap.

  const int num_ops = 10000000;
  const int frequency = 10000;

  for (int i = 1; i <= num_ops; i++) {
    std::pair<Operation, int> op_v = this->randomOp();
    Operation op = op_v.first;
    int v = op_v.second;

    if (op == PUSH) {
      this->plain_heap.push_back(-v);
      std::push_heap(this->plain_heap.begin(), this->plain_heap.end());
      
      oheap->insertBlock(v, this->sampleData(v));
    } else if (op == POP) {
      std::pop_heap(this->plain_heap.begin(), this->plain_heap.end());
      int plain_v = -this->plain_heap.back();
      this->plain_heap.pop_back();

      int oheap_v = oheap->extractMin().k;

      if (plain_v != oheap_v) {
        std::cout << "Error! " << plain_v << " and " << oheap_v << " don't match!" << std::endl;
      }
    }

    if (i % frequency == 0) {
      std::cout << "Performed " << i << " operations; current heap size is " << this->plain_heap.size() << std::endl;
    }
  }

  delete oheap;
  delete random;
  delete storage;
}
