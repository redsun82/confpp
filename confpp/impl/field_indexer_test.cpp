#include "confpp/impl/field_indexer.h"

#include <catch2/catch_all.hpp>
#include <string>
#include <vector>

TEST_CASE("field_indexer") {
  struct S {
    int a;
    char b;
    std::string c;
    char d;
    uint16_t e;
    std::vector<char> f;
  };

  S s;
  confpp::FieldIndexer sut{&s};

  SECTION("consecutive indexing") {
    CHECK(&s.a == sut.index<int>());
    CHECK(&s.b == sut.index<char>());
    CHECK(&s.c == sut.index<std::string>());
    CHECK(&s.d == sut.index<char>());
    CHECK(&s.e == sut.index<uint16_t>());
    CHECK(&s.f == sut.index<std::vector<char>>());

    CHECK(sut.indexed(s));
  }
  SECTION("skipped indexing") {
    sut.index<int>();
    sut.index<char>();
    sut.index<char>();
    sut.index<uint16_t>();
    sut.index<std::vector<char>>();

    CHECK_FALSE(sut.indexed(s));
  }
}
