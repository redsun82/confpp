#pragma once

#include <trompeloeil.hpp>

#include "confpp/field_processor.h"

namespace confpp {

class LoadingContextMock : public LoadingContext {
public:
  MAKE_MOCK0(current_field, std::string_view(), const override);
  MAKE_MOCK1(error, void(std::string_view), override);
};

class FieldProcessorMock : public FieldProcessor {
public:
  MAKE_MOCK0(requires_cli_arg, bool(), const override);
  MAKE_MOCK2(get_from_cli, bool(LoadingContext &, const char *), override);
  MAKE_MOCK1(get_from_negated_cli, bool(LoadingContext &), override);
};

} // namespace confpp
