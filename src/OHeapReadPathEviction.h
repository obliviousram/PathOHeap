#ifndef POHEAP_OHEAPREADPATHEVICTION_H
#define POHEAP_OHEAPREADPATHEVICTION_H

#include "OHeapInterface.h"
#include "RandForOHeapInterface.h"
#include "UntrustedStorageInterface.h"

class OHeapReadPathEviction : public OHeapInterface {
 public:
  OHeapReadPathEviction(UntrustedStorageInterface* storage,
      RandForOHeapInterface* rand_gen, int bucket_size, int num_blocks, bool type_hiding_security);

  Block findMin();
  
  /** Returns the leaf_id (pos) and t of the inserted block. */
  std::pair<int, int> insertBlock(int k, std::array<std::byte, Block::BLOCK_SIZE> v);
  
  /** Returns the deleted block if one matching the parameters exists, otherwise a dummy block. */
  Block deleteBlock(int leaf, long t);
  
  Block extractMin();

  /** Ill-defined if no block with matching leaf and t exists. */
  std::pair<int, int> decreaseKey(int leaf, long t);
  std::pair<int, int> increaseKey(int leaf, long t);

  std::vector<Block>& getStash();
  int getStashSize();
  int getNumLeaves();
  int getNumLevels();
  int getNumBlocks();
  int getNumBuckets();
  friend std::ostream& operator<<(std::ostream& out, const OHeapReadPathEviction& oheap);

 private:
  int bucket_size;
  int num_levels;
  int num_leaves;
  int num_blocks;
  int num_buckets;
  long t;  // timestamp or number of ops performed so far
  bool type_hiding_security;

  UntrustedStorageInterface* storage;
  RandForOHeapInterface* rand_gen;
  std::vector<Block> stash;

  Block onlyFindMin();
  Block onlyDeleteBlock(int leaf, long t);
  std::pair<int, int> onlyInsertBlock(int k, std::array<std::byte, Block::BLOCK_SIZE> v);
  void insertDummyBlock();
  void evictAfterInsert();

  /**
   * Evicts the path corresponding to the given leaf, then runs updateMin.
   * (It never makes sense to evict without updating min.)
   */
  void evictAndUpdateMin(int leaf);

  /**
   * Each leaf corresponds to a specific path made up of one bucket per level; in reality buckets
   * are placed linearly in storage. Returns the correct storage index so that bucket can be
   * retrived.
   */
  int P(int leaf, int level);

  /**
   * Operation P.ReadNRm from the OHeap paper where leaf identifies the path.
   * Returns the removed Block if one matching the parameters exists, otherwise a dummy block.
   */
  Block readAndRemove(int leaf, long t);

  /** Operation P.updateMin from the OHeap paper where leaf identifies the path. */
  void updateMin(int leaf);
};

#endif 
