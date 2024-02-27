#pragma once

#include <memory>
#include <string_view>

namespace confpp {

class ConfigurationContext {
public:
  using ptr_t = std::unique_ptr<ConfigurationContext>;

  ConfigurationContext() = default;
  virtual ~ConfigurationContext() = default;

  ConfigurationContext(const ConfigurationContext &) = delete;
  ConfigurationContext &operator=(const ConfigurationContext &) = delete;

  virtual void set_current_field(std::string_view field) = 0;
  virtual std::string_view current_field() const = 0;
  virtual void error(std::string_view message) = 0;
  virtual void check_errors() const = 0;
};

class FieldProcessor {
public:
  using ptr_t = std::unique_ptr<FieldProcessor>;

  FieldProcessor() = default;
  virtual ~FieldProcessor() = default;

  FieldProcessor(const FieldProcessor &) = delete;
  FieldProcessor &operator=(const FieldProcessor &) = delete;

  virtual void clear() = 0;
  virtual bool is_required() const { return false; };
  virtual bool requires_cli_arg() const { return true; }
  virtual bool get_from_cli(ConfigurationContext &ctx, const char *arg) = 0;
  virtual bool get_from_negated_cli(ConfigurationContext &ctx) {
    ctx.error("Negated flag not supported");
    return false;
  };
};

template <typename T> class DefaultFieldProcessor;

struct FieldProcessingFlags {
  bool required{false};
  char cli_separator{'\0'};
};

template <typename T> class DefaultFieldProcessorBase : public FieldProcessor {
protected:
  T &value;
  T default_value;
  FieldProcessingFlags flags;

public:
  DefaultFieldProcessorBase(T &value, T default_value,
                            FieldProcessingFlags flags = {})
      : value{value}, default_value{default_value}, flags{flags} {}

  void clear() override { value = default_value; }
  bool is_required() const override { return flags.required; }
};

} // namespace confpp
