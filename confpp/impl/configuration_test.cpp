#include <catch2/catch_all.hpp>

#include "confpp/configuration.h"

#include <optional>

using namespace std::string_view_literals;

TEST_CASE("basic") {

  struct Conf : confpp::Configuration {
    std::string a1 = field("a1");
    const char *a2 = field("a2");
    bool b1 = field("b1");
    bool b2 = field("b2");
    bool b3 = field("b3");
    bool b4 = field("b4");
    std::optional<std::string> c1 = field("c1").optional();
    std::optional<std::string> c2 = field("c2").optional();
  };

  Conf conf;
  conf.b2 = true;
  conf.b4 = false;
  conf.c2 = "garbage";

  SECTION("get") {
    std::vector args{"./test",     "--a1",    "hello",     "--a2=world",
                     "--b1",       "--no-b2", "--b3=true", "--b4=false",
                     "--c1=hello", "--no-c2"};
    conf.load_from_cli(args);
    REQUIRE(conf.a1 == "hello");
    REQUIRE(conf.a2 == "world"sv);
    REQUIRE(conf.b1 == true);
    REQUIRE(conf.b2 == false);
    REQUIRE(conf.b3 == true);
    REQUIRE(conf.b4 == false);
    REQUIRE(conf.c1 == "hello");
    REQUIRE(conf.c2 == std::nullopt);
  }
}
