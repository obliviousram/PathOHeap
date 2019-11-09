#include "Block.h"
#include <climits>

Block::Block(int leaf_id, long t, int k, std::array<std::byte, BLOCK_SIZE> v)
  : leaf_id(leaf_id), t(t), k(k), v(v) {}

Block::Block() : leaf_id(-1), t(-1), k(INT_MAX) {
  this->v.fill(std::byte{0});
}

bool operator<(const Block& b1, const Block& b2) {
  return (b1.k < b2.k) || (b1.k == b2.k && b1.t < b2.t);
}

bool operator==(const Block& b1, const Block& b2) {
  return b1.leaf_id == b2.leaf_id && b1.t == b2.t && b1.k == b2.k && b1.v == b2.v;
}

bool operator!=(const Block& b1, const Block& b2) {
  return !(b1 == b2);
}

std::ostream& operator<<(std::ostream& out, const Block& b) {
  if (b.leaf_id != -1) {
    out << "Block (leaf, t, k, v) = " << b.leaf_id << " " << b.t << " " << b.k << " [ ";
    for (std::byte byte : b.v) {
      out << std::to_integer<int>(byte) << " ";
    }
    out << "]";
  } else {
    out << "Dummy block";
  }
  return out;
}
