#pragma once

#include <cassert>
#include <cstddef>
#include <memory>

namespace confpp {

class FieldRetriever {
  void *current;

  void *address(size_t align, size_t size);

public:
  template <typename T> explicit FieldRetriever(T &obj) : current{&obj} {}

  template <typename T> T &retrieve() {
    return *reinterpret_cast<T *>(address(alignof(T), sizeof(T)));
  }
};
} // namespace confpp
