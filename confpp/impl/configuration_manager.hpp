#pragma once

#include <fmt/format.h>

#include <cassert>
#include <exception>
#include <list>fmt/
#include <string>
#include <string_view>
#include <unordered_map>

#include "confpp/field_processor.hpp"

namespace confpp {

class ConfigurationError : public std::exception {
  const char *what() override { return "Encountered a configuration error"; }
}

class DefaultConfigurationContext : public ConfigurationContext {
  std::string_view current;
  bool has_errors{false};

public:
  void set_current_field(std::string_view field) override { current = field; }
  std::string_view current_field() const override { return current; }
  void error(std::string_view message) override {
    has_errors = true;
    fmt::println(stderr, "While processing {}: {}", current, message);
  }
  void check_errors() const override {
    if (has_errors) {
      throw ConfigurationError();
    }
  }
};

class ConfigurationManager {
  ConfigurationContext::ptr_t ctx;
  std::list<std::string> string_pool;
  std::unordered_map<std::string_view, FieldProcessor::ptr_t> processors;

  void register_processor(std::string name, FieldProcessor::ptr_t processor) {
    string_pool.emplace_back(std::move(name));
    auto [it, inserted] =
        processors.emplace(string_pool.back(), std::move(processor));
    assert(inserted && "already registered");
  }

public:
  explicit ConfigurationManager(
      ConfigurationContext::ptr_t ctx =
          std::make_unique<DefaultConfigurationContext>())
      : ctx{std::move(ctx)} {}
}
} // namespace confpp
