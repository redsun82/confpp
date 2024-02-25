#pragma once

#include <span>

namespace confpp {
class FieldProcessor {
public:
  FieldProcessor() = default;
  virtual ~FieldProcessor() = default;

  FieldProcessor(const FieldProcessor &) = delete;
  FieldProcessor &operator=(const FieldProcessor &) = delete;

  virtual bool is_boolean() const { return true; }
  virtual bool get_from_cli(std::span<const char *> &args) = 0;
};
} // namespace confpp
