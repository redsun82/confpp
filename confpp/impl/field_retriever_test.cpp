#include "confpp/impl/field_retriever.h"

#include <catch2/catch_all.hpp>
#include <cstdint>
#include <string>
#include <vector>

TEST_CASE("field_indexer") {
  SECTION("without base") {
    struct S {
      int a;
      uint8_t b;
      std::string c;
      uint8_t d;
      uint16_t e;
      std::vector<uint8_t> f;
    };

    S s;
    confpp::FieldRetriever sut{s};

    SECTION("good retrieves") {
      CHECK(&s.a == &sut.retrieve<int>());
      CHECK(&s.b == &sut.retrieve<uint8_t>());
      CHECK(&s.c == &sut.retrieve<std::string>());
      CHECK(&s.d == &sut.retrieve<uint8_t>());
      CHECK(&s.e == &sut.retrieve<uint16_t>());
      CHECK(&s.f == &sut.retrieve<std::vector<uint8_t>>());
    }
    SECTION("skipped one") {
      sut.retrieve<int>();
      sut.retrieve<uint8_t>();
      sut.retrieve<uint8_t>();
      sut.retrieve<uint16_t>();
      sut.retrieve<std::vector<uint8_t>>();
    }
  }
  SECTION("with base") {
    struct Base {
    protected:
      ~Base() = default;

    public:
      char c;
      std::string s;
      confpp::FieldRetriever sut{*this};
    };
    struct Derived : Base {
      uint8_t x;
      std::string y;
      uint16_t z;
    };
    Derived d;

    d.sut.retrieve<Base>();

    CHECK(&d.x == &d.sut.retrieve<uint8_t>());
    CHECK(&d.y == &d.sut.retrieve<std::string>());
    CHECK(&d.z == &d.sut.retrieve<uint16_t>());
  }
}
