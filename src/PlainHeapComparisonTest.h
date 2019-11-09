#include "Block.h"
#include <vector>
#include <utility>

class PlainHeapComparisonTest {
 public:
  PlainHeapComparisonTest();
	void run();

 private:
  enum Operation {PUSH, POP};
  
  std::vector<int> plain_heap;
  int max_heap_size;

  static std::array<std::byte, Block::BLOCK_SIZE> sampleData(int i);
  std::pair<Operation, int> randomOp();
};
