#include "JobForStatisticalSimulation.h"
#include "ServerStorage.h"
#include "RandomForOHeap.h"
#include "OHeapReadPathEviction.h"
#include <cmath>
#include <unordered_map>

void JobForStatisticalSimulation::run() {
  int bucket_size = 2;
  int num_blocks = pow(2, 20);
  const int frequency = 500000;

  std::array<std::byte, Block::BLOCK_SIZE> sample_data;
  sample_data.fill(std::byte{0xa});

  UntrustedStorageInterface* storage = new ServerStorage();
  RandForOHeapInterface* random = new RandomForOHeap();
  OHeapInterface* oheap = new OHeapReadPathEviction(storage, random, bucket_size, num_blocks, false);

  std::cout << "Warming up" << std::endl;
  // do inserts until heap is full, then increaseKeys
  
  // as with Job2, need to keep around leaves and ts
  auto leaves = std::vector<int>();
  auto ts = std::vector<int>();

  for (int i = 1; i <= 3000000; i++) {
    if (i <= num_blocks) {
      std::pair<int, int> leaf_t = oheap->insertBlock(i, sample_data);
      leaves.push_back(leaf_t.first);
      ts.push_back(leaf_t.second);
    } else {
      std::pair<int, int> leaf_t = oheap->increaseKey(leaves.at(i % num_blocks), ts.at(i % num_blocks));
      leaves.at(i % num_blocks) = leaf_t.first;
      ts.at(i % num_blocks) = leaf_t.second;
    }

    if (i % frequency == 0) {
      std::cout << "Accesses made: " << i << std::endl;
    }
  }

  std::cout << "Start recording..." << std::endl;

  int max_stash_size = 0;
  auto histogram = std::unordered_map<int, int>();
  int stash_size;

  for (int i = 1; i <= 500000000; i++) {
    std::pair<int, int> leaf_t = oheap->increaseKey(leaves.at(i % num_blocks), ts.at(i % num_blocks));
    leaves.at(i % num_blocks) = leaf_t.first;
    ts.at(i % num_blocks) = leaf_t.second;

    stash_size = oheap->getStashSize();
    if (histogram.find(stash_size) != histogram.end()) {
      histogram[stash_size]++;
    } else {
      histogram[stash_size] = 1;
    }

    max_stash_size = (stash_size > max_stash_size) ? stash_size : max_stash_size;
    
    if (i % frequency == 0) {
      std::cout << "Accesses made (after warmup): " << i << std::endl;

      for (int i = 0; i <= max_stash_size; i++) {
        if (histogram.find(i) != histogram.end()) {
          std::cout << i << "," << histogram[i] << std::endl;
        } else {
          std::cout << i << ",0" << std::endl;
        }
      }
    }
  }

  delete oheap;
  delete random;
  delete storage;
}
