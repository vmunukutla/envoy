#pragma once

#include <string>

#include "envoy/server/filter_config.h"

#include "envoy/extensions/filters/http/gcp_events_convert/v3/gcp_events_convert.pb.h"

namespace Envoy {
namespace Extensions {
namespace HttpFilters {
namespace GcpEventsConvert {

class GcpEventsConvertFilterConfig {
public:
  GcpEventsConvertFilterConfig(const envoy::extensions::filters::http::gcp_events_convert::v3::Decoder& proto_config);

  const std::string& key() const { return key_; }
  const std::string& val() const { return val_; }

private:
  const std::string key_;
  const std::string val_;
};

using GcpEventsConvertFilterConfigSharedPtr = std::shared_ptr<GcpEventsConvertFilterConfig>;

class GcpEventsConvertFilter : public Http::StreamDecoderFilter {
public:
  GcpEventsConvertFilter(GcpEventsConvertFilterConfigSharedPtr);
  ~GcpEventsConvertFilter();

  // Http::StreamFilterBase
  void onDestroy() override;

  // Http::StreamDecoderFilter
  Http::FilterHeadersStatus decodeHeaders(Http::RequestHeaderMap&, bool) override;
  Http::FilterDataStatus decodeData(Buffer::Instance&, bool) override;
  Http::FilterTrailersStatus decodeTrailers(Http::RequestTrailerMap&) override;
  void setDecoderFilterCallbacks(Http::StreamDecoderFilterCallbacks&) override;

private:
  
  void buildBody(const Buffer::Instance*, const Buffer::Instance& , std::string&);
  const Http::LowerCaseString headerKey() const;
  const std::string headerValue() const;
  bool isCloudEvent(Http::RequestHeaderMap&);
  // modify the data of HTTP request
  // 1. drain buffered data
  // 2. write cloud event data
  bool updateBody();
  // modify the header of HTTP request
  // 1. replace header's content type with ce-datacontenttype
  // 2. add cloud event information, ce-version, ce-type...... (except ce's data)
  // 3. [TBD] add Ack ID into header 
  bool updateHeader();
  
  Http::RequestHeaderMap* request_headers_{};
  bool skip_{};
  const GcpEventsConvertFilterConfigSharedPtr config_;
  Http::StreamDecoderFilterCallbacks* decoder_callbacks_;
};

} // namespace GcpEventsConvert
} // namespace HttpFilters
} // namespace Extensions
} // namespace Envoy