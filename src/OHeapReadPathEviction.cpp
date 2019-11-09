#include "OHeapReadPathEviction.h"
#include <cmath>
#include <algorithm>
#include <climits>

OHeapReadPathEviction::OHeapReadPathEviction(UntrustedStorageInterface* storage,
    RandForOHeapInterface* rand_gen, int bucket_size, int num_blocks, bool type_hiding_security)
    : storage(storage), rand_gen(rand_gen), bucket_size(bucket_size), num_blocks(num_blocks),
      type_hiding_security(type_hiding_security) {
  this->num_levels = ceil(log2(num_blocks)) + 1;
  this->num_buckets = pow(2, num_levels) - 1;
  if (this->num_buckets*this->bucket_size < this->num_blocks) {  // potential precision loss
    throw std::runtime_error("Not enough space for the acutal number of blocks.");
  }
  this->num_leaves = pow(2, num_levels - 1);
  this->t = 0;
  
  std::cout << "Initializing OHeap with " << this->num_levels << " levels, " <<
      this->num_blocks << " blocks, " << this->num_buckets << " buckets" << std::endl;

  Bucket::resetState();
  Bucket::setMaxSize(bucket_size);
  this->rand_gen->setBound(num_leaves);
  this->storage->setCapacity(num_buckets);
  
  this->stash = std::vector<Block>();

  Bucket init_bkt = Bucket();
  for (int i = 0; i < bucket_size; i++) {
    init_bkt.addBlock(Block());
  }

  for (int i = 0; i < num_buckets; i++) {
    this->storage->writeBucket(i, Bucket(init_bkt));
  }
}

Block OHeapReadPathEviction::findMin() {
  Block ret = this->onlyFindMin();

  if (this->type_hiding_security) {
    this->onlyDeleteBlock(-1, -1);
    this->insertDummyBlock();
  }

  return ret;
}

Block OHeapReadPathEviction::deleteBlock(int leaf, long t) {
  if (this->type_hiding_security) {
    this->onlyFindMin();
  }
    
  Block ret = this->onlyDeleteBlock(leaf, t);
    
  if (this->type_hiding_security) {
    this->insertDummyBlock();
  }

  this->t++;
  return ret;
}

std::pair<int, int> OHeapReadPathEviction::insertBlock(int k, std::array<std::byte, Block::BLOCK_SIZE> v) {
  if (this->type_hiding_security) {
    this->onlyFindMin();
    this->onlyDeleteBlock(-1, -1);
  }

  auto ret = std::pair<int, int>(-1, -1);

  if (k != INT_MAX) {
    ret = this->onlyInsertBlock(k, v);
  } else {
    this->insertDummyBlock();
  }

  this->t++;
  return ret;
}

Block OHeapReadPathEviction::extractMin() {
  Block min_block = this->onlyFindMin();
  Block ret = this->onlyDeleteBlock(min_block.leaf_id, min_block.t);

  if (this->type_hiding_security) {
    this->insertDummyBlock();
  }

  this->t++;
  return ret;
}

std::pair<int, int> OHeapReadPathEviction::decreaseKey(int leaf, long t) {
  if (this->type_hiding_security) {
    this->onlyFindMin();
  }

  Block old_block = this->onlyDeleteBlock(leaf, t);
  auto ret = this->onlyInsertBlock(old_block.k - 1, old_block.v);

  this->t++;
  return ret;
}

std::pair<int, int> OHeapReadPathEviction::increaseKey(int leaf, long t) {
  if (this->type_hiding_security) {
    this->onlyFindMin();
  }

  Block old_block = this->onlyDeleteBlock(leaf, t);
  auto ret = this->onlyInsertBlock(old_block.k + 1, old_block.v);

  this->t++;
  return ret;
}

Block OHeapReadPathEviction::onlyFindMin() {
  Block min = this->storage->readBucket(P(0,0)).subtree_min;
  if (this->stash.size() > 0) {
    const Block& stash_min = *std::min_element(this->stash.begin(), this->stash.end());
    if (stash_min < min) {
      min = stash_min;
    }
  }
  return min;
}

std::pair<int, int> OHeapReadPathEviction::onlyInsertBlock(int k, std::array<std::byte, Block::BLOCK_SIZE> v) {
  auto ret = std::pair<int, int>();

  int leaf = this->rand_gen->getRandomLeaf();
  stash.push_back(Block(leaf, this->t, k, v));
  
  ret.first = leaf;
  ret.second = this->t;

  this->evictAfterInsert();

  return ret;
}

/** pedagogy */
void OHeapReadPathEviction::insertDummyBlock() {
  this->evictAfterInsert();
}

/** convenience function */
void OHeapReadPathEviction::evictAfterInsert() {
  // two random paths non-overlapping except at the root
  // = random leaf_id in the first half and random leaf_id in the second half (over all leaf_ids)
  int random_leaf_one = this->rand_gen->getRandomLeaf()/2;
  int random_leaf_two = this->rand_gen->getRandomLeaf()/2 + this->num_leaves/2;

  this->evictAndUpdateMin(random_leaf_one);
  this->evictAndUpdateMin(random_leaf_two);
}

Block OHeapReadPathEviction::onlyDeleteBlock(int leaf, long t) {
  // try to delete from stash
  int removed_index = -1;
  for (int i = 0; i < stash.size(); i++) {
    Block& stash_block_ref = stash.at(i);
    if (stash_block_ref.leaf_id == leaf && stash_block_ref.t == t) {
      removed_index = i;
      break;
    }
  }

  Block stash_block = Block();
  if (removed_index != -1) {
    stash_block = Block(stash.at(removed_index));
    stash.erase(stash.begin() + removed_index);
  }

  // try to delete from storage (dummy ops if block was found in stash)
  Block storage_block = this->readAndRemove(leaf, t);
  this->evictAndUpdateMin(leaf);

  if (stash_block.t != -1) {
    return stash_block;
  } else {
    return storage_block;
  }
}

void OHeapReadPathEviction::evictAndUpdateMin(int leaf) {
  if (leaf != -1) {
    // pull all blocks on path into stash
    for (int l = 0; l < num_levels; l++) {
      std::vector<Block> blocks = this->storage->readBucket(this->P(leaf, l)).getBlocks();
      for (const Block& b : blocks) {
        if (b.t != -1) {
          this->stash.push_back(b);
        }
      }
    }

    // push as many stash blocks as possible into the leaf bucket
    // then push as many stash blocks as possible into the leaf's parent
    // etc.
    for (int l = num_levels-1; l >= 0; l--) {
      auto timestamps_evicted = std::vector<int>();
      Bucket bucket;
      int Pxl = P(leaf, l);
      int counter = 0;

      for (const Block& be_evicted : this->stash) {
        if (counter >= this->bucket_size) {
          break;
        }
        if (Pxl == this->P(be_evicted.leaf_id, l)) {
          bucket.addBlock(be_evicted);
          timestamps_evicted.push_back(be_evicted.t);
          counter++;
        }
      }

      // removing from the stash; careful not to modify the stash while iterating over it
      for (int i = 0; i < timestamps_evicted.size(); i++) {
        for (int j = 0; j < this->stash.size(); j++) {
          if (this->stash.at(j).t == timestamps_evicted.at(i)) {
            this->stash.erase(this->stash.begin() + j);
            break;
          }
        }
      }

      // pad out the bucket with dummy blocks
      while (counter < bucket_size) {
        bucket.addBlock(Block());
        counter++;
      }

      this->storage->writeBucket(Pxl, bucket);
    }
  } else {
    // dummy ops
    leaf = this->rand_gen->getRandomLeaf();
    auto buckets = std::vector<Bucket>();

    for (int l = 0; l < num_levels; l++) {
      buckets.push_back(this->storage->readBucket(P(leaf, l)));
    }

    for (int l = num_levels-1; l >= 0; l--) {
      this->storage->writeBucket(this->P(leaf, l), buckets.at(l));
    }
  }

  this->updateMin(leaf);
}

Block OHeapReadPathEviction::readAndRemove(int leaf, long t) {
  if (leaf != -1 && t != -1) {
    Block ret;    
    for (int l = 0; l < this->num_levels; l++) {
      int p = this->P(leaf, l);
      Bucket bucket_on_path = this->storage->readBucket(p);
      std::optional<Block> possible_ret = bucket_on_path.removeBlock(t);
      if (possible_ret.has_value()) {
        ret = possible_ret.value();
        bucket_on_path.addBlock(Block());  // for obliviousness
      }
      this->storage->writeBucket(p, bucket_on_path);
    }
    return ret;
  } else {
    // dummy ops
    leaf = this->rand_gen->getRandomLeaf();
    for (int l = 0; l < num_levels; l++) {
      int p = this->P(leaf, l);
      Bucket b = this->storage->readBucket(p);
      this->storage->writeBucket(p, b);
    }
    return Block();
  }
}

void OHeapReadPathEviction::updateMin(int leaf) {
  if (leaf != -1) {
    for (int l = num_levels-1; l >= 0; l--) {
      int p = this->P(leaf, l);
      auto potential_mins = std::vector<const Block*>();

      Bucket bucket_on_path = this->storage->readBucket(p);
      const std::vector<Block>& blocks_on_path = bucket_on_path.getBlocks();
      if (blocks_on_path.size() > 0) {
        const Block& min_block_on_path =
            *std::min_element(blocks_on_path.begin(), blocks_on_path.end());
        potential_mins.push_back(&min_block_on_path);
      }
      if (l != num_levels-1) {
        Bucket child1_bucket = this->storage->readBucket(2*p+1);
        Bucket child2_bucket = this->storage->readBucket(2*p+2);
        potential_mins.push_back(&child1_bucket.subtree_min);
        potential_mins.push_back(&child2_bucket.subtree_min);
      }

      // given a vector of pointers, get the min of what those pointers reference
      if (potential_mins.size() > 0) {
        int min_index = 0;
        for (int i = 1; i < potential_mins.size(); i++) {
          if (*potential_mins.at(i) < *potential_mins.at(min_index)) {
            min_index = i;
          }
        }
        bucket_on_path.subtree_min = *potential_mins.at(min_index);
      } else {
        bucket_on_path.subtree_min = Block();
      }

      this->storage->writeBucket(p, bucket_on_path);
    }
  } else {
    // dummy ops
    leaf = this->rand_gen->getRandomLeaf();
    for (int l = num_levels-1; l >= 0; l--) {
      int p = this->P(leaf, l);
      Bucket b = this->storage->readBucket(p);
      if (l != num_levels-1) {
        this->storage->readBucket(2*p+1);
        this->storage->readBucket(2*p+2);
      }
      this->storage->writeBucket(p, b);
    }
  }
}

int OHeapReadPathEviction::P(int leaf, int level) {
  if (level == 0) return 0;
    
  return pow(2,level)-1 + (leaf/pow(2,num_levels-1-level));
}

std::vector<Block>& OHeapReadPathEviction::getStash() {
  return this->stash;
}
    
int OHeapReadPathEviction::getStashSize() {
  return this->stash.size();
}
    
int OHeapReadPathEviction::getNumLeaves() {
  return this->num_leaves;
}

int OHeapReadPathEviction::getNumLevels() {
  return this->num_levels;
}

int OHeapReadPathEviction::getNumBlocks() {
  return this->num_blocks;
}

int OHeapReadPathEviction::getNumBuckets() {
  return this->num_buckets;
}

std::ostream& operator<<(std::ostream& out, const OHeapReadPathEviction& oheap) {
  out << "==========" << std::endl;
  for (int i = 0; i < oheap.num_buckets; i++) {
    std::vector<Block> blocks = oheap.storage->readBucket(i).getBlocks();
    bool all_dummy = true;
    for (const Block& bl : blocks) {
      if (bl.leaf_id != -1) {
        all_dummy = false;
        break;
      }
    }
    if (!all_dummy) {
      out << "Bucket " << i << " contains:" << std::endl;
    
      for (int j = 0; j < oheap.bucket_size; j++) {
        const Block& bl = blocks.at(j);
        if (bl.leaf_id != -1) {
          out << bl << std::endl;
        }
      }

      out << std::endl;
    }
  }
  out << "==========" << std::endl;
  return out;
}
