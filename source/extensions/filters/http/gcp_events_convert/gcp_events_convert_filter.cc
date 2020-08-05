#include "extensions/filters/http/gcp_events_convert/gcp_events_convert_filter.h"

#include <string>

#include "envoy/extensions/filters/http/gcp_events_convert/v3/gcp_events_convert.pb.h"
#include "envoy/server/filter_config.h"

#include "common/buffer/buffer_impl.h"
#include "common/common/cleanup.h"
#include "common/common/enum_to_int.h"
#include "common/grpc/common.h"
#include "common/http/codes.h"
#include "common/http/utility.h"
#include "common/protobuf/utility.h"

#include "google/pubsub/v1/pubsub.pb.h"

using google::pubsub::v1::PubsubMessage;
using google::pubsub::v1::ReceivedMessage;

namespace Envoy {
namespace Extensions {
namespace HttpFilters {
namespace GcpEventsConvert {

GcpEventsConvertFilterConfig::GcpEventsConvertFilterConfig(
    const envoy::extensions::filters::http::gcp_events_convert::v3::Decoder& proto_config)
    : key_(proto_config.key()), val_(proto_config.val()) {}

GcpEventsConvertFilter::GcpEventsConvertFilter(GcpEventsConvertFilterConfigSharedPtr config)
    : skip_(false), config_(config) {}

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
    skip_ = true;
    return Http::FilterHeadersStatus::Continue;
  }

  // store the current header for future usage
  request_headers_ = &headers;
  return Http::FilterHeadersStatus::StopIteration;
}

Http::FilterDataStatus GcpEventsConvertFilter::decodeData(Buffer::Instance&, bool end_stream) {
  std::cout << "decode data" << std::endl;
  // for any requst body that is not related to cloud event. Pass through
  if (skip_) return Http::FilterDataStatus::Continue;

  // wait for all the body has arrived.
  if (end_stream) {
    const Buffer::Instance* buffered = decoder_callbacks_->decodingBuffer();
    
    if (buffered == nullptr) {
      // nothing got buffered, Continue
      return Http::FilterDataStatus::Continue;
    }

    ReceivedMessage received_message;
    Envoy::ProtobufUtil::JsonParseOptions parse_option;
    auto status = Envoy::ProtobufUtil::JsonStringToMessage(buffered->toString(), &received_message, parse_option);
    
    if (status != google::protobuf::util::Status::OK) {
      // buffered data didn't successfully converted to proto. Continue
      return Http::FilterDataStatus::Continue;
    }

    PubsubMessage& pubsub_message = *received_message.mutable_message();

    // TODO 
    // Use Cloud Event SDK to convert Pubsub Message to HTTP Binding
    
    if (!updateHeader() || !updateBody()){
      // something wrong while update HTTP request. Continue
      return Http::FilterDataStatus::Continue;
    }

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

bool GcpEventsConvertFilter::isCloudEvent(Http::RequestHeaderMap& headers) {
  absl::string_view  content_type = headers.getContentTypeValue();
  return content_type.compare("application/grcp+json+cloudevent") == 0;
}

bool GcpEventsConvertFilter::updateHeader() {
  request_headers_->addCopy(headerKey(), headerValue());
  return true;
}

bool GcpEventsConvertFilter::updateBody() {
    decoder_callbacks_->modifyDecodingBuffer([](Buffer::Instance& buffered) {
      Buffer::OwnedImpl new_buffer;
      new_buffer.add("This is a example body");
      buffered.drain(buffered.length());
      buffered.move(new_buffer);
    });
    return true;
}


} // namespace GcpEventsConvert
} // namespace HttpFilters
} // namespace Extensions
} // namespace Envoy