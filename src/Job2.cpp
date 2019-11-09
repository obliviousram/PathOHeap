#include "Job2.h"
#include "ServerStorage.h"
#include "RandomForOHeap.h"
#include "OHeapReadPathEviction.h"
#include <cmath>

void Job2::run() {
  int bucket_size = 3;
  int num_blocks = pow(2, 12);
  UntrustedStorageInterface* storage = new ServerStorage();
  RandForOHeapInterface* random = new RandomForOHeap();
  OHeapInterface* oheap = new OHeapReadPathEviction(storage, random, bucket_size, num_blocks, false);

  std::array<std::byte, Block::BLOCK_SIZE> sample_data;
  sample_data.fill(std::byte{0xa});

  // Sample computation: fill the heap, then start overwriting.

  // overwrite requires knowing the leaf and t of the block to be overwritten, so keep them around as the heap is filled
  auto leaves = std::vector<int>();
  auto ts = std::vector<int>();

  for (int i = 0; i < num_blocks; i++) {
    std::pair<int, int> leaf_t = oheap->insertBlock(i, sample_data);
    leaves.push_back(leaf_t.first);
    ts.push_back(leaf_t.second);
  }

  for (int i = 0; i < 300000; i++) {
    std::pair<int, int> leaf_t = oheap->increaseKey(leaves.at(i % num_blocks), ts.at(i % num_blocks));
    leaves.at(i % num_blocks) = leaf_t.first;
    ts.at(i % num_blocks) = leaf_t.second;

    if (i % 10000 == 0) {
      std::cout << "After block " << i << " stash size is " << oheap->getStashSize() << std::endl;
    }
  }

  delete oheap;
  delete random;
  delete storage;
}
