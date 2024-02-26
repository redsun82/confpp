#pragma once

#include <string_view>

namespace confpp {

class LoadingContext {
public:
  LoadingContext() = default;
  virtual ~LoadingContext() = default;

  LoadingContext(const LoadingContext &) = delete;
  LoadingContext &operator=(const LoadingContext &) = delete;

  virtual std::string_view current_field() const = 0;
  virtual void error(std::string_view message) = 0;
};

class FieldProcessor {
public:
  FieldProcessor() = default;
  virtual ~FieldProcessor() = default;

  FieldProcessor(const FieldProcessor &) = delete;
  FieldProcessor &operator=(const FieldProcessor &) = delete;

  virtual void set_default() const = 0;
  virtual bool requires_cli_arg() const { return true; }
  virtual bool get_from_cli(LoadingContext &ctx, const char *arg) = 0;
  virtual bool get_from_negated_cli(LoadingContext &ctx) {
    ctx.error("Negated flag not supported");
    return false;
  };
};

template <typename T> class DefaultFieldProcessor;

} // namespace confpp
