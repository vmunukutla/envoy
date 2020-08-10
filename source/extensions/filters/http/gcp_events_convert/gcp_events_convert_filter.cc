#include "extensions/filters/http/gcp_events_convert/gcp_events_convert_filter.h"

#include <string>

#include "envoy/common/exception.h"
#include "envoy/extensions/filters/http/gcp_events_convert/v3/gcp_events_convert.pb.h"
#include "envoy/http/filter.h"
#include "envoy/server/filter_config.h"

#include "common/buffer/buffer_impl.h"
#include "common/common/cleanup.h"
#include "common/common/enum_to_int.h"
#include "common/common/utility.h"
#include "common/grpc/common.h"
#include "common/http/codes.h"
#include "common/http/utility.h"
#include "common/protobuf/utility.h"

#include "google/pubsub/v1/pubsub.pb.h"

namespace Envoy {
namespace Extensions {
namespace HttpFilters {
namespace GcpEventsConvert {

GcpEventsConvertFilterConfig::GcpEventsConvertFilterConfig(
    const envoy::extensions::filters::http::gcp_events_convert::v3::GcpEventsConvert& proto_config)
    : key_(proto_config.key()), val_(proto_config.val()) {}

GcpEventsConvertFilter::GcpEventsConvertFilter(GcpEventsConvertFilterConfigSharedPtr config)
    : config_(config) {}

void GcpEventsConvertFilter::onDestroy() {}

const Http::LowerCaseString GcpEventsConvertFilter::headerKey() const {
  return Http::LowerCaseString(config_->key_);
}

const std::string GcpEventsConvertFilter::headerValue() const {
  return config_->val_;
}

Http::FilterHeadersStatus GcpEventsConvertFilter::decodeHeaders(Http::RequestHeaderMap& headers, bool) {
  // TODO(h9jiang): Implement the logic of decodeHeaders
  headers.addCopy(headerKey() , headerValue());
  return Http::FilterHeadersStatus::Continue;
}

Http::FilterDataStatus GcpEventsConvertFilter::decodeData(Buffer::Instance&, bool) {
  // TODO(h9jiang): Implement the logic of decodeData
  return Http::FilterDataStatus::Continue;
}

Http::FilterTrailersStatus GcpEventsConvertFilter::decodeTrailers(Http::RequestTrailerMap&) {
  return Http::FilterTrailersStatus::Continue;
}

void GcpEventsConvertFilter::setDecoderFilterCallbacks(Http::StreamDecoderFilterCallbacks& callbacks) {
  decoder_callbacks_ = &callbacks;
}

} // namespace GcpEventsConvert
} // namespace HttpFilters
} // namespace Extensions
} // namespace Envoy