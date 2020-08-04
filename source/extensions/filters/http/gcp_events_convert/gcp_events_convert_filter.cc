#include <string>

#include "extensions/filters/http/gcp_events_convert/gcp_events_convert_filter.h"

#include "envoy/extensions/filters/http/gcp_events_convert/v3/gcp_events_convert.pb.h"
#include "envoy/server/filter_config.h"

#include "common/buffer/buffer_impl.h"
#include "common/common/cleanup.h"
#include "common/common/enum_to_int.h"
#include "common/grpc/common.h"
#include "common/http/codes.h"
#include "common/http/utility.h"
#include "common/protobuf/utility.h"

// #include "google/pubsub/v1/pubsub_proto.pb.h"

namespace Envoy {
namespace Extensions {
namespace HttpFilters {
namespace GcpEventsConvert {

GcpEventsConvertFilterConfig::GcpEventsConvertFilterConfig(
    const envoy::extensions::filters::http::gcp_events_convert::v3::Decoder& proto_config)
    : key_(proto_config.key()), val_(proto_config.val()) {}

GcpEventsConvertFilter::GcpEventsConvertFilter(GcpEventsConvertFilterConfigSharedPtr config)
    : valid_cloud_event_(false), config_(config) {}

GcpEventsConvertFilter::~GcpEventsConvertFilter() {}

void GcpEventsConvertFilter::onDestroy() {}

const Http::LowerCaseString GcpEventsConvertFilter::headerKey() const {
  return Http::LowerCaseString(config_->key());
}

const std::string GcpEventsConvertFilter::headerValue() const {
  return config_->val();
}

Http::FilterHeadersStatus GcpEventsConvertFilter::decodeHeaders(Http::RequestHeaderMap& headers, bool end_stream) {
  if (end_stream || !isCloudEvent(headers)) {
    // if this is a header-only request or it's not a request containing cloud event
    // we don't need to do any buffering
    return Http::FilterHeadersStatus::Continue;
  }

  // store the current header for future usage
  request_headers_ = &headers;
  return Http::FilterHeadersStatus::StopIteration;
}

Http::FilterDataStatus GcpEventsConvertFilter::decodeData(Buffer::Instance&, bool end_stream) {
  // for any requst body that is not related to cloud event. Pass through
  if (!valid_cloud_event_) return Http::FilterDataStatus::Continue;

  // wait for all the body has arrived.
  if (end_stream) {
    const Buffer::Instance* buffered = decoder_callbacks_->decodingBuffer();
    // nothing got buffered, Continue
    if (buffered == nullptr) return Http::FilterDataStatus::Continue;

    std::unique_ptr<Buffer::Instance> new_buffer = std::make_unique<Buffer::OwnedImpl>();
    buffered->copyOut(0 , buffered->length() , new_buffer.get());
    // ...
    // TODO
    // ...
    return Http::FilterDataStatus::Continue;
  }

  else return Http::FilterDataStatus::StopIterationAndBuffer;
}

Http::FilterTrailersStatus GcpEventsConvertFilter::decodeTrailers(Http::RequestTrailerMap&) {
  return Http::FilterTrailersStatus::Continue;
}

void GcpEventsConvertFilter::setDecoderFilterCallbacks(Http::StreamDecoderFilterCallbacks& callbacks) {
  decoder_callbacks_ = &callbacks;
}

void GcpEventsConvertFilter::buildBody(const Buffer::Instance* buffered,
                                        const Buffer::Instance& last,
                                        std::string& body) {
  body.reserve((buffered ? buffered->length() : 0) + last.length());
  if (buffered) {
    for (const Buffer::RawSlice& slice : buffered->getRawSlices()) {
      body.append(static_cast<const char*>(slice.mem_), slice.len_);
    }
  }

  for (const Buffer::RawSlice& slice : last.getRawSlices()) {
    body.append(static_cast<const char*>(slice.mem_), slice.len_);
  }
}

// use the content type string value to determine whether the filter should be triggered or not. 
bool GcpEventsConvertFilter::isCloudEvent(Http::RequestHeaderMap& headers) {
  absl::string_view  content_type = headers.getContentTypeValue();
  valid_cloud_event_ = content_type.compare("application/grcp+json+cloudevent") == 0;
  return valid_cloud_event_;
}


} // namespace GcpEventsConvert
} // namespace HttpFilters
} // namespace Extensions
} // namespace Envoy