#include "confpp/impl/field_descriptor.hpp"

#include <catch2/catch_all.hpp>

template <>
class confpp::DefaultFieldProcessor<long>
    : confpp::DefaultFieldProcessorBase<long> {
public:
  using DefaultFieldProcessorBase<long>::DefaultFieldProcessorBase;

  bool get_from_cli(ConfigurationContext &ctx, const char *arg) override {
    value = 42;
    return true;
  }
};

TEST_CASE("descriptors") {
  auto d = confpp::field("a");
  long l = 2;
  auto [name, p] = confpp::make_processor_item(l, std::move(d));
  CHECK(name == "a");
  REQUIRE(p);
  CHECK_FALSE(p->is_required());
  p->clear();
  CHECK(l == 0);
  confpp::ConfigurationContext *null = nullptr;
  CHECK(p->get_from_cli(*null, ""));
  CHECK(l == 42);
}
