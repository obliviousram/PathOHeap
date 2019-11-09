#include "Bucket.h"
#include <stdexcept>

bool Bucket::is_init = false;
int Bucket::max_size = -1;

Bucket::Bucket() : blocks(), subtree_min(Block()) {
  if (!Bucket::is_init) {
    throw std::runtime_error("Please set bucket size before creating a bucket");
  }
}

void Bucket::addBlock(const Block& new_blk) {
  if (this->blocks.size() < Bucket::max_size) {
    this->blocks.push_back(new_blk);
  }
}

std::optional<Block> Bucket::removeBlock(long t) {
  int removed_index = -1;
  for (int i = 0; i < this->blocks.size(); i++) {
    if (this->blocks.at(i).t == t) {
      removed_index = i;
      break;
    }
  }

  if (removed_index == -1) {
    return std::nullopt;
  } else {
    Block removed_copy = Block(this->blocks.at(removed_index));
    this->blocks.erase(this->blocks.begin() + removed_index);
    return std::optional<Block>{removed_copy};
  }
}

std::vector<Block>& Bucket::getBlocks() {
  return this->blocks;
}

void Bucket::setMaxSize(int maximum_size) {
  if (Bucket::is_init) {
    throw std::runtime_error("Max Bucket Size was already set");
  }
  Bucket::max_size = maximum_size;
  Bucket::is_init = true;
}

void Bucket::resetState() {
  Bucket::is_init = false;
}
