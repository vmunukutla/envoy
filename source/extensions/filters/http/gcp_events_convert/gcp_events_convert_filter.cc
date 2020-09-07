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
#include "external/com_github_cloudevents_sdk/v1/protocol_binding/pubsub_binder.h"
#include "external/com_github_cloudevents_sdk/v1/protocol_binding/http_binder.h"

namespace Envoy {
namespace Extensions {
namespace HttpFilters {
namespace GcpEventsConvert {

using google::pubsub::v1::PubsubMessage;
using google::pubsub::v1::ReceivedMessage;
using cloudevents::binding::Binder;
using io::cloudevents::v1::CloudEvent;

GcpEventsConvertFilterConfig::GcpEventsConvertFilterConfig(
    const envoy::extensions::filters::http::gcp_events_convert::v3::GcpEventsConvert& proto_config)
    : content_type_(proto_config.content_type()) {}

GcpEventsConvertFilter::GcpEventsConvertFilter(GcpEventsConvertFilterConfigSharedPtr config)
    : config_(config) {}

GcpEventsConvertFilter::GcpEventsConvertFilter(GcpEventsConvertFilterConfigSharedPtr config,
                                               bool has_cloud_event,
                                               Http::RequestHeaderMap* headers)
    : request_headers_(headers), has_cloud_event_(has_cloud_event), config_(config) {}

void GcpEventsConvertFilter::onDestroy() {}

Http::FilterHeadersStatus GcpEventsConvertFilter::decodeHeaders(Http::RequestHeaderMap& headers,
                                                                bool end_stream) {
  std::cout << "========== convert filter header ARRIVE ==========" << std::endl;
  std::cout << headers << std::endl;

  if (end_stream || !isCloudEvent(headers)) {
    // if this is a header-only request or it's not a request containing cloud event
    // we don't need to do any buffering
    return Http::FilterHeadersStatus::Continue;
  }

  has_cloud_event_ = true;
  // store the current header for future usage
  request_headers_ = &headers;
  return Http::FilterHeadersStatus::StopIteration;
}

Http::FilterDataStatus GcpEventsConvertFilter::decodeData(Buffer::Instance& buffer, bool end_stream) {
  std::cout << "========== convert filter body ARRIVE   ==========" << std::endl;
  std::cout << "@buffer : " << std::endl << buffer.toString() << std::endl;
  std::cout << "@buffer size: " << buffer.length() << std::endl;
  std::cout << "@end stream : " << ( (end_stream) ? "true" : "false")  << std::endl;

  // for any requst body that is not related to cloud event. Pass through
  if (!has_cloud_event_)
    return Http::FilterDataStatus::Continue;

  // For any request body that is not the end of HTTP request and not empty
  // Buffer the current HTTP request's body
  if (!end_stream)
    return Http::FilterDataStatus::StopIterationAndBuffer;

  if (decoder_callbacks_ == nullptr) {
    ENVOY_LOG(warn, "Gcp Events Convert Filter log: decoder callbacks pointer = nullptr");
    return Http::FilterDataStatus::Continue;
  }

  const Buffer::Instance* buffered = decoder_callbacks_->decodingBuffer();

  ReceivedMessage received_message;
  bool status = received_message.ParseFromString(buildBody(buffered, buffer));

  if (!status) {
    // buffered data didn't successfully converted to proto. Continue
    ENVOY_LOG(warn, "Gcp Events Convert Filter log: fail to convert from body to proto object");
    return Http::FilterDataStatus::Continue;
  }

  const PubsubMessage& pubsub_message = received_message.message();
  cloudevents::binding::PubsubBinder pubsub_binder;

  cloudevents_absl::StatusOr<CloudEvent> ce = pubsub_binder.Unbind(pubsub_message);
  if (!ce.ok()) {
    ENVOY_LOG(warn, "Gcp Events Convert Filter log: SDK pubsub unbind error {}", ce.status());
    return Http::FilterDataStatus::Continue;
  }

  cloudevents::binding::HttpReqBinder http_binder;
  cloudevents_absl::StatusOr<HttpRequest> req = http_binder.Bind(*ce);
  if (!req.ok()) {
    ENVOY_LOG(warn, "Gcp Events Convert Filter log: SDK Http bind error {}", req.status());
    return Http::FilterDataStatus::Continue;
  }

  absl::Status update_status = updateHeader(*req);
  if (!update_status.ok()) {
    ENVOY_LOG(warn, "Gcp Events Convert Filter log: update header {}", update_status.ToString());
    return Http::FilterDataStatus::Continue;
  }

  update_status = updateBody(*req, buffered, buffer);
  if (!update_status.ok()) {
    ENVOY_LOG(warn, "Gcp Events Convert Filter log: update body {}", update_status.ToString());
    return Http::FilterDataStatus::Continue;
  }
  std::cout << "========== convert filter header/body LEAVE ==========" << std::endl;
  std::cout << "@header : " << std::endl;
  std::cout << *request_headers_ << std::endl;
  std::cout << "@buffer : " << buildBody(buffered, buffer) << std::endl;
  std::cout << "@buffer size : " << buffer.length() << std::endl; 
  return Http::FilterDataStatus::Continue;
}

Http::FilterTrailersStatus GcpEventsConvertFilter::decodeTrailers(Http::RequestTrailerMap&) {
  return Http::FilterTrailersStatus::Continue;
}

void GcpEventsConvertFilter::setDecoderFilterCallbacks(
    Http::StreamDecoderFilterCallbacks& callbacks) {
  decoder_callbacks_ = &callbacks;
}

std::string GcpEventsConvertFilter::buildBody(const Buffer::Instance* buffered,
                                              const Buffer::Instance& last) {
  std::string body;
  body.reserve((buffered ? buffered->length() : 0) + last.length());
  if (buffered) {
    for (const Buffer::RawSlice& slice : buffered->getRawSlices()) {
      body.append(static_cast<const char*>(slice.mem_), slice.len_);
    }
  }

  for (const Buffer::RawSlice& slice : last.getRawSlices()) {
    body.append(static_cast<const char*>(slice.mem_), slice.len_);
  }

  return body;
}

bool GcpEventsConvertFilter::isCloudEvent(const Http::RequestHeaderMap& headers) const {
  return headers.getContentTypeValue() == config_->content_type_;
}

absl::Status GcpEventsConvertFilter::updateHeader(const HttpRequest& http_req) {
  for (const auto& header : http_req.base()) {
    Http::LowerCaseString header_key(header.name_string().to_string());
    // avoid deep copy from boost string_view to absl string_view
    // only copy the @pointer data() , @length size()
    absl::string_view header_val(header.value().data(), header.value().size());
    if (header_key == Http::LowerCaseString("content-type")) {
      request_headers_->setContentType(header_val);
    } else {
      request_headers_->addCopy(header_key, header_val);
    }
  }
  return absl::OkStatus();
}

absl::Status GcpEventsConvertFilter::updateBody(const HttpRequest& http_req,
                                                const Buffer::Instance* bufferedPtr,
                                                Buffer::Instance& buffer) {
  buffer.drain(buffer.length());
  if (bufferedPtr) {
    decoder_callbacks_->modifyDecodingBuffer([&http_req](Buffer::Instance& buffered) {
      // drain the current buffered instance
      buffered.drain(buffered.length());
      // replace the current buffered instance with the new body
      buffered.add(http_req.body());
    }); 
  } else {
    buffer.add(http_req.body());
  }
  return absl::OkStatus();
}

} // namespace GcpEventsConvert
} // namespace HttpFilters
} // namespace Extensions
} // namespace Envoy
