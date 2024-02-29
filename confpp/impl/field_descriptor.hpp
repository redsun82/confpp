#pragma once
#include <concepts>
#include <string>

#include "confpp/field_processor.hpp"

namespace confpp {

struct DefaultSource {
  template <typename T> explicit operator T() { return T{}; }
};

inline constexpr DefaultSource default_source{};

template <typename D> struct FieldDescriptor {
  std::string name;
  D default_value;
  FieldProcessingFlags flags;

  template <typename T> auto with_default(T &&d) && {
    return FieldDescriptor<T>{std::move(name), std::forward<T>(d), flags};
  }

  auto required() && {
    flags.required = true;
    return std::move(*this);
  }

  auto with_separator(char s) && {
    flags.cli_separator = s;
    return std::move(*this);
  }
};

auto field(std::string name) {
  return FieldDescriptor<DefaultSource>{std::move(name), default_source};
}

template <typename T, typename D>
  requires std::constructible_from<T, D>
auto make_processor_item(T &dest, FieldDescriptor<D> &&descriptor) {
  FieldProcessor::ptr_t p = std::make_unique<DefaultFieldProcessor<T>>(
      dest, T{std::move(descriptor.default_value)}, descriptor.flags);
  return std::pair<std::string, FieldProcessor::ptr_t>{
      std::move(descriptor.name), std::move(p)};
};
} // namespace confpp
