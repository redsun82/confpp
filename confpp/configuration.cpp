#include "confpp/configuration.h"
#include "configuration.h"

namespace confpp {
Configuration::Configuration() : retriever{*this} {
  // advance retriever at the end of Configuration to retrieve fields in derived
  // classes
  retriever.retrieve<Configuration>();
}
} // namespace confpp
