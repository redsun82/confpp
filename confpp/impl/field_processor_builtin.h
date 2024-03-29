#pragma once

#include "confpp/field_processor.h"

#include <concepts>
#include <optional>
#include <type_traits>

namespace confpp {

template <typename T> class DefaultFieldProcessorBase : public FieldProcessor {
protected:
  T &value;
  T default_value;

public:
  DefaultFieldProcessorBase(T &value, T default_value = {})
      : value{value}, default_value{default_value} {}

  void set_default() const override { value = default_value; }
};

template <>
class DefaultFieldProcessor<bool> : public DefaultFieldProcessorBase<bool> {

public:
  using DefaultFieldProcessorBase<bool>::DefaultFieldProcessorBase;
  using DefaultFieldProcessorBase<bool>::value;

  bool requires_cli_arg() const override { return false; }

  bool get_from_cli(LoadingContext &ctx, const char *arg) override {
    using namespace std::string_view_literals;
    if (!arg || arg == ""sv || arg == "true"sv) {
      value = true;
      return true;
    }
    if (arg == "false"sv) {
      value = false;
      return true;
    }
    ctx.error("Expected 'true' or 'false'");
    return false;
  }

  bool get_from_negated_cli(LoadingContext &ctx) override {
    value = false;
    return true;
  };
};

// TODO: move to a type traits helper library
template <typename T> struct is_optional : std::false_type {};
template <typename T> struct is_optional<std::optional<T>> : std::true_type {};

template <typename T>
  requires std::convertible_to<const char *, T>
class DefaultFieldProcessor<T> : public DefaultFieldProcessorBase<T> {
public:
  using DefaultFieldProcessorBase<T>::DefaultFieldProcessorBase;
  using DefaultFieldProcessorBase<T>::value;

  bool get_from_cli(LoadingContext &ctx, const char *arg) override {
    value = arg;
    return true;
  }

  bool get_from_negated_cli(LoadingContext &ctx) override {
    if constexpr (is_optional<T>::value) {
      value = std::nullopt;
      return true;
    } else {
      ctx.error("Negated flag not supported");
      return false;
    }
  };
};

} // namespace confpp
