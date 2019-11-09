#ifndef POHEAP_UNTRUSTEDSTORAGEINTERFACE_H
#define POHEAP_UNTRUSTEDSTORAGEINTERFACE_H

#include "Bucket.h"

class UntrustedStorageInterface {
 public:
  virtual void setCapacity(int total_num_of_buckets) = 0;
  virtual Bucket readBucket(int position) = 0;
  virtual void writeBucket(int position, const Bucket& bucket_to_write) = 0;
};

#endif
