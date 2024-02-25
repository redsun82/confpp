#include "confpp/impl/field_indexer.h"

namespace confpp {
void *FieldIndexer::index(size_t align, size_t size) {
  size_t space = size * align; // upper limit for where the pointer could land
  auto res = std::align(align, size, current, space);
  assert(res);
  // set current just past this object
  current = reinterpret_cast<char *>(current) + size;
  return res;
}

bool FieldIndexer::indexed(void *address, size_t size) const {
  return current == reinterpret_cast<char *>(address) + size;
}
} // namespace confpp
