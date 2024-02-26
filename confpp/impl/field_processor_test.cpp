#include <catch2/catch_all.hpp>
#include <catch2/trompeloeil.hpp>

#include "confpp/impl/field_processor_builtin.h"
#include "confpp/impl/field_processor_mocks.h"
#include "field_processor_mocks.h"

using namespace confpp;
using namespace trompeloeil;
using namespace std::string_view_literals;

TEST_CASE("bool field processor") {
  LoadingContextMock ctx;
  bool value = false;
  DefaultFieldProcessor<bool> processor{value};

  SECTION("requires_cli_arg") {
    REQUIRE(processor.requires_cli_arg() == false);
  }

  SECTION("get_from_cli") {
    SECTION("get_from_cli true") {
      REQUIRE(processor.get_from_cli(ctx, "true"));
      REQUIRE(value);
    }
    SECTION("get_from_cli empty") {
      REQUIRE(processor.get_from_cli(ctx, ""));
      REQUIRE(value);
    }
    SECTION("get_from_cli nullptr") {
      REQUIRE(processor.get_from_cli(ctx, nullptr));
      REQUIRE(value);
    }
    SECTION("get_from_cli false") {
      value = true;
      REQUIRE(processor.get_from_cli(ctx, "false"));
      REQUIRE_FALSE(value);
    }
    SECTION("get_from_cli other") {
      auto initial_value = GENERATE(false, true);
      value = initial_value;
      REQUIRE_CALL(ctx, error("Expected 'true' or 'false'"));
      REQUIRE_FALSE(processor.get_from_cli(ctx, "other"));
      REQUIRE(value == initial_value);
    }
  }

  SECTION("get_from_negated_cli") {
    value = true;
    REQUIRE(processor.get_from_negated_cli(ctx));
    REQUIRE_FALSE(value);
  }
}

TEMPLATE_TEST_CASE("string field processor", "", std::string, std::string_view,
                   const char *, std::optional<std::string>) {
  LoadingContextMock ctx;
  TestType value = "garbage";
  DefaultFieldProcessor<TestType> processor{value};

  SECTION("requires_cli_arg") { REQUIRE(processor.requires_cli_arg()); }

  SECTION("get_from_cli") {
    SECTION("get_from_cli") {
      REQUIRE(processor.get_from_cli(ctx, "value"));
      REQUIRE(value == "value"sv);
    }
  }

  SECTION("get_from_negated_cli") {
    if constexpr (std::is_same_v<TestType, std::optional<std::string>>) {
      REQUIRE(processor.get_from_negated_cli(ctx));
      REQUIRE_FALSE(value);
    } else {
      REQUIRE_CALL(ctx, error("Negated flag not supported"));
      REQUIRE_FALSE(processor.get_from_negated_cli(ctx));
      REQUIRE(value == "garbage"sv);
    }
  }
}
