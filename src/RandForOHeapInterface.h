#ifndef POHEAP_RANDFOROHEAPINTERFACE_H
#define POHEAP_RANDFOROHEAPINTERFACE_H

class RandForOHeapInterface {
 public:
  virtual int getRandomLeaf() = 0;
  virtual void setBound(int num_leaves) = 0;
};

#endif 
