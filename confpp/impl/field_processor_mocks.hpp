#pragma once

#include <trompeloeil.hpp>

#include "confpp/field_processor.hpp"

namespace confpp {

class ConfigurationContextMock : public ConfigurationContext {
public:
  MAKE_MOCK1(set_current_field, void(std::string_view), override);
  MAKE_MOCK0(current_field, std::string_view(), const override);
  MAKE_MOCK1(error, void(std::string_view), override);
  MAKE_MOCK0(check_errors, void(), const override);
};

class FieldProcessorMock : public FieldProcessor {
public:
  MAKE_MOCK0(clear, void(), override);
  MAKE_MOCK0(is_required, bool(), const override);
  MAKE_MOCK0(requires_cli_arg, bool(), const override);
  MAKE_MOCK2(get_from_cli, bool(ConfigurationContext &, const char *),
             override);
  MAKE_MOCK1(get_from_negated_cli, bool(ConfigurationContext &), override);
};

} // namespace confpp
