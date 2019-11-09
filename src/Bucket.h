#ifndef POHEAP_BUCKET_H
#define POHEAP_BUCKET_H

#include "Block.h"
#include <vector>
#include <optional>

class Bucket {
 public:
  // copy; holding a reference or pointer causes trouble when this->blocks is reallocated
  Block subtree_min;

  Bucket();

  /**
   * Caller is responsible for making sure new_blk is allowed in this bucket!
   * Caller is responsible for updating this bucket's subtree_min afterward!
   * Copies the given block into this bucket.
   */
  void addBlock(const Block& new_blk);

  /**
   * Caller is responsible for updating this bucket's subtree_min afterward!
   * Returns the removed block if a timestamp match is found.
   */
  std::optional<Block> removeBlock(long t);

  std::vector<Block>& getBlocks();

  static void setMaxSize(int maximum_size);
  static void resetState();

 private:
  static bool is_init;
  static int max_size;
  std::vector<Block> blocks;
};

#endif
