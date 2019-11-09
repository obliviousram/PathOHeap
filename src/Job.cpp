#include "Job.h"
#include "ServerStorage.h"
#include "RandomForOHeap.h"
#include "OHeapReadPathEviction.h"
#include <cmath>

std::array<std::byte, Block::BLOCK_SIZE> Job::sampleData(int i) {
  std::array<std::byte, Block::BLOCK_SIZE> ret;
  ret.fill(std::byte{0});
  ret.at(0) = static_cast<std::byte>((i >> 24) & 0xFF);
  ret.at(1) = static_cast<std::byte>((i >> 16) & 0xFF);
  ret.at(2) = static_cast<std::byte>((i >> 8) & 0xFF);
  ret.at(3) = static_cast<std::byte>(i & 0xFF);
  return ret;
}

void Job::run() {
  int bucket_size = 4;
  int num_blocks = pow(2, 5);
  UntrustedStorageInterface* storage = new ServerStorage();
  RandForOHeapInterface* random = new RandomForOHeap();
  OHeapInterface* oheap = new OHeapReadPathEviction(storage, random, bucket_size, num_blocks, false);

  // Sample computation: fill the heap with some numbers, then read it back.
  for (int i = 0; i < num_blocks; i++) {
    oheap->insertBlock(i, Job::sampleData(i));
    std::cout << "Inserting Block " << i << " to OHeap. The stash size is: " <<
        oheap->getStashSize() << std::endl;
  }

  for (int i = 0; i < num_blocks; i++) {
    std::cout << "Extracting Block " << i << " from OHeap. " << oheap->extractMin() << std::endl;
  }

  delete oheap;
  delete random;
  delete storage;
}
