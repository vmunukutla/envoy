#pragma once

#include <string>

#include "envoy/extensions/filters/http/gcp_events_convert/v3/gcp_events_convert.pb.h"
#include "envoy/extensions/filters/http/gcp_events_convert/v3/gcp_events_convert.pb.validate.h"

#include "extensions/filters/http/common/factory_base.h"
#include "extensions/filters/http/well_known_names.h"

namespace Envoy {
namespace Extensions {
namespace HttpFilters {
namespace GcpEventsConvert {

/**
 * Config registration for the Gcp Events Convert filter. @see NamedHttpFilterConfigFactory.
 */
class GcpEventsConvertFilterFactory
    : public Common::FactoryBase<
          envoy::extensions::filters::http::gcp_events_convert::v3::GcpEventsConvert> {
public:
  GcpEventsConvertFilterFactory() : FactoryBase(HttpFilterNames::get().GcpEventsConvert) {}

  Http::FilterFactoryCb createFilterFactoryFromProtoTyped(
      const envoy::extensions::filters::http::gcp_events_convert::v3::GcpEventsConvert&
          proto_config,
      const std::string& stats_prefix, Server::Configuration::FactoryContext& context) override;
};

} // namespace GcpEventsConvert
} // namespace HttpFilters
} // namespace Extensions
} // namespace Envoy
