#include <catch2/catch_all.hpp>

#include "confpp/configuration.h"

#include <optional>

using namespace std::string_view_literals;

TEST_CASE("integration test") {

  struct Conf : confpp::Configuration {
    std::string a1 = field("a1");
    const char *a2 = field("a2");
    const char *a3 = field("a3").with_default("");
    bool b1 = field("b1");
    bool b2 = field("b2").with_default(true);
    bool b3 = field("b3");
    bool b4 = field("b4").with_default(true);
    std::optional<std::string> c1 = opt_field("c1");
    std::optional<std::string> c2 = opt_field("c2").with_default("garbage");
    std::string d = field("d").with_default("default");
  };

  Conf conf;

  SECTION("default constructor") {
    REQUIRE(conf.a1 == "");
    REQUIRE(conf.a2 == nullptr);
    REQUIRE(conf.a3 == ""sv);
    REQUIRE(conf.b1 == false);
    REQUIRE(conf.b2 == true);
    REQUIRE(conf.b3 == false);
    REQUIRE(conf.b4 == true);
    REQUIRE(conf.c1 == std::nullopt);
    REQUIRE(conf.c2 == "garbage");
    REQUIRE(conf.d == "default");
  }

  SECTION("load_from_cli") {
    std::vector args{"./test",     "--a1",       "hello",   "--a2=world",
                     "--a3=!",     "--b1",       "--no-b2", "--b3=true",
                     "--b4=false", "--c1=hello", "--no-c2"};
    conf.load_from_cli(args);
    REQUIRE(conf.a1 == "hello");
    REQUIRE(conf.a2 == "world"sv);
    REQUIRE(conf.a3 == "!"sv);
    REQUIRE(conf.b1 == true);
    REQUIRE(conf.b2 == false);
    REQUIRE(conf.b3 == true);
    REQUIRE(conf.b4 == false);
    REQUIRE(conf.c1 == "hello");
    REQUIRE(conf.c2 == std::nullopt);
    REQUIRE(conf.d == "default");

    SECTION("clear") {
      conf.clear();
      REQUIRE(conf.a1 == "");
      REQUIRE(conf.a2 == nullptr);
      REQUIRE(conf.a3 == ""sv);
      REQUIRE(conf.b1 == false);
      REQUIRE(conf.b2 == true);
      REQUIRE(conf.b3 == false);
      REQUIRE(conf.b4 == true);
      REQUIRE(conf.c1 == std::nullopt);
      REQUIRE(conf.c2 == "garbage");
      REQUIRE(conf.d == "default");
    }
  }
}
