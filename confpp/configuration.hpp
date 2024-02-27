#pragma once

#include <fmt/core.h>

#include <cassert>
#include <memory>
#include <optional>
#include <span>
#include <string>
#include <string_view>
#include <type_traits>
#include <unordered_map>

#include "confpp/field_processor.hpp"
#include "confpp/impl/field_processor_builtin.hpp"
#include "confpp/impl/field_retriever.hpp"

namespace confpp {
class Configuration {
  FieldRetriever retriever;
  std::unordered_map<std::string, std::unique_ptr<FieldProcessor>> processors;

  template <typename T>
  T &&annotate(std::string &&name, T &&default_value = T()) {
    auto &field = retriever.retrieve<T>();
    auto [it, inserted] = processors.emplace(
        std::move(name),
        std::make_unique<DefaultFieldProcessor<T>>(field, default_value));
    assert(inserted);
    return std::forward<T>(default_value);
  }

  template <bool Optional, typename U> struct DefaultAnnotator;

  template <bool Optional = false> struct Annotator {
    Configuration &conf;
    std::string name;

    template <typename T>
      requires(!Optional)
    operator T() && {
      return conf.annotate<T>(std::move(name));
    }

    template <typename T>
      requires(Optional)
    operator T() && = delete;

    template <typename T>
      requires(Optional)
    operator std::optional<T>() && {
      return conf.annotate<std::optional<T>>(std::move(name));
    }

    template <typename T>
    DefaultAnnotator<Optional, T> with_default(T &&default_value);
  };

  template <bool Optional, typename U>
  struct DefaultAnnotator : Annotator<Optional> {
    using Annotator<Optional>::conf;
    using Annotator<Optional>::name;

    U default_value;

    DefaultAnnotator(Configuration &conf, std::string name, U &&default_value)
        : Annotator<Optional>{conf, std::move(name)},
          default_value{std::move(default_value)} {}

    template <typename T> operator T() && = delete;

    template <typename T>
      requires(std::convertible_to<U, T> && !Optional)
    operator T() && {
      return conf.template annotate<T>(std::move(name),
                                       std::move(default_value));
    }

    template <typename T>
      requires(std::convertible_to<U, std::optional<T>> && Optional)
    operator std::optional<T>() && {
      return conf.template annotate<std::optional<T>>(std::move(name),
                                                      std::move(default_value));
    }
  };

protected:
  Configuration();
  ~Configuration() = default;
  Annotator<false> field(std::string name) { return {*this, std::move(name)}; }
  Annotator<true> opt_field(std::string name) {
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

  void clear() {
    for (auto &p : processors) {
      p.second->set_default();
    }
  }
};

template <bool Optional>
template <typename T>
Configuration::DefaultAnnotator<Optional, T>
Configuration::Annotator<Optional>::with_default(T &&default_value) {
  return {conf, std::move(name), std::forward<T>(default_value)};
}

} // namespace confpp
