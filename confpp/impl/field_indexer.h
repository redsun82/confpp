#pragma once

#include <cassert>
#include <cstddef>
#include <memory>

namespace confpp {

class FieldIndexer {
  void *current;

  void *index(size_t align, size_t size);
  bool check(void *address, size_t size) const;

public:
  FieldIndexer(void *start) : current{start} {}

  template <typename T> void *index() { return index(alignof(T), sizeof(T)); }

  template <typename T> bool check(T &obj) const {
    return check(&obj, sizeof(T));
  }
};
} // namespace confpp
