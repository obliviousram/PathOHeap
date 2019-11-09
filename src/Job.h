#include "Block.h"

class Job {
 public:
	static void run();

 private:
  static std::array<std::byte, Block::BLOCK_SIZE> sampleData(int i);
};
