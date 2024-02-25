#pragma once

#include <cassert>
#include <string>
#include <unordered_map>

#include "confpp/impl/field_indexer.h"

namespace confpp {
class Configuration {
  FieldIndexer indexer;
  std::unordered_map<std::string, void *> addresses;

  template <typename T> T annotate(std::string &&name) {
    auto address = indexer.index<T>();
    auto [it, inserted] = addresses.emplace(std::move(name), address);
    assert(inserted);
  }

  struct Annotator {
    Configuration &conf;
    std::string name;

    template <typename T> operator T() && { conf.annotate<T>(std::move(name)); }
  };

protected:
}
} // namespace confpp
