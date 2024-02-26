#include "confpp/impl/field_retriever.h"

namespace confpp {
void *FieldRetriever::address(size_t align, size_t size) {
  size_t space = size * align; // upper limit for where the pointer could land
  auto res = std::align(align, size, current, space);
  assert(res);
  // set current just past this object
  current = reinterpret_cast<char *>(current) + size;
  return res;
}
} // namespace confpp
