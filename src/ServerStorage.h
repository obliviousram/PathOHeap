#ifndef POHEAP_SERVERSTORAGE_H
#define POHEAP_SERVERSTORAGE_H

#include "UntrustedStorageInterface.h"

class ServerStorage : public UntrustedStorageInterface {
 public:
  ServerStorage();

  /** Must be called before reading or writing any buckets. Cannot be changed after being set. */
  void setCapacity(int total_num_of_buckets);

  /** Returns a copy of the read bucket. The original bucket cannot be modified directly. */
  Bucket readBucket(int position);

  /** Copies the given bucket into this storage. */
  void writeBucket(int position, const Bucket& bucket_to_write);

private: 
  static bool is_initialized;
  static bool is_capacity_set;
  int capacity;
  std::vector<Bucket> buckets;

  void checkRWValid(int position);
};

#endif
