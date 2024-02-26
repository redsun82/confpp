#pragma once

#include <cassert>
#include <fmt/core.h>
#include <memory>
#include <optional>
#include <span>
#include <string>
#include <string_view>
#include <type_traits>
#include <unordered_map>

#include "confpp/field_processor.h"
#include "confpp/impl/field_processor_builtin.h"
#include "confpp/impl/field_retriever.h"

namespace confpp {
class Configuration {
  FieldRetriever retriever;
  std::unordered_map<std::string, std::unique_ptr<FieldProcessor>> processors;

  template <typename T> T annotate(std::string &&name) {
    auto &field = retriever.retrieve<T>();
    auto [it, inserted] = processors.emplace(
        std::move(name), std::make_unique<DefaultFieldProcessor<T>>(field));
    assert(inserted);
    return T();
  }

  template <typename T> struct is_optional : std::false_type {};

  template <typename T>
  struct is_optional<std::optional<T>> : std::true_type {};

  struct OptionalAnnotator;

  struct Annotator {
    Configuration &conf;
    std::string name;

    template <typename T> operator T() && {
      return conf.annotate<T>(std::move(name));
    }

    OptionalAnnotator optional() &&;
  };

  struct OptionalAnnotator : Annotator {
    template <typename T>
      requires(is_optional<T>::value)
    operator T() && {
      return conf.annotate<T>(std::move(name));
    }
  };

protected:
  Configuration();
  ~Configuration() = default;
  Annotator field(std::string name) { return {*this, std::move(name)}; }

  OptionalAnnotator opt_field(std::string name) {
    return {*this, std::move(name)};
  }

public:
  bool load_from_cli(int argc, const char *const *argv) {
    std::span<const char *const> args{argv, static_cast<std::size_t>(argc)};
    return false;
    load_from_cli(args);
  }

  bool load_from_cli(std::span<const char *const> args) {
    // TODO: implement this in a separate class
    struct Ctx : LoadingContext {
      std::string_view arg;

      std::string_view current_field() const override { return arg; }
      void error(std::string_view m) override {
        fmt::println("While processing --{}: {}", arg, m);
      }
    };
    Ctx ctx;
    args = args.subspan(1);
    while (!args.empty()) {
      ctx.arg = args[0];
      args = args.subspan(1);
      if (ctx.arg == "--") {
        // TODO positional args
        return true;
      }
      if (ctx.arg.starts_with("--no-")) {
        ctx.arg.remove_prefix(5);
        auto it = processors.find(std::string{ctx.arg});
        if (it == processors.end()) {
          ctx.error("Unknown flag");
          return false;
        }
        if (!it->second->get_from_negated_cli(ctx)) {
          return false;
        }
        continue;
      }
      if (ctx.arg.starts_with("--")) {
        ctx.arg.remove_prefix(2);
        auto equal_pos = ctx.arg.find('=');
        const char *value = nullptr;
        if (equal_pos != std::string_view::npos) {
          value = ctx.arg.data() + equal_pos + 1;
          ctx.arg = ctx.arg.substr(0, equal_pos);
        }
        auto it = processors.find(std::string{ctx.arg});
        if (it == processors.end()) {
          ctx.error("Unknown flag");
          return false;
        }
        if (!value && it->second->requires_cli_arg()) {
          if (args.empty()) {
            ctx.error("Expected argument");
            return false;
          }
          value = args[0];
          args = args.subspan(1);
        }
        if (!it->second->get_from_cli(ctx, value)) {
          return false;
        }
        continue;
      }
      // TODO: short flags and positional args
      return false;
    }
    return true;
  }
};

Configuration::OptionalAnnotator Configuration::Annotator::optional() && {
  return OptionalAnnotator{conf, std::move(name)};
}
} // namespace confpp
