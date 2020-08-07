#include <chrono>
#include <memory>

#include "envoy/event/dispatcher.h"
#include "envoy/extensions/filters/http/gcp_events_convert/v3/gcp_events_convert.pb.h"

#include "common/http/header_map_impl.h"
#include "common/runtime/runtime_impl.h"

#include "extensions/filters/http/gcp_events_convert/gcp_events_convert_filter.h"
#include "extensions/filters/http/well_known_names.h"

#include "test/mocks/buffer/mocks.h"
#include "test/mocks/http/mocks.h"
#include "test/test_common/printers.h"
#include "test/test_common/test_runtime.h"

#include "gmock/gmock.h"
#include "gtest/gtest.h"

using testing::InSequence;
using testing::NiceMock;
using testing::Return;

namespace Envoy {
namespace Extensions {
namespace HttpFilters {
namespace GcpEventsConvert {

class GcpEventsConvertFilterTest : public testing::Test {
public:
  GcpEventsConvertFilterConfigSharedPtr setupConfig() {
    envoy::extensions::filters::http::gcp_events_convert::v3::GcpEventsConvert proto_config;
    proto_config.set_key("some random key");
    proto_config.set_val("some random value");
    return std::make_shared<GcpEventsConvertFilterConfig>(proto_config);
  }

  GcpEventsConvertFilterTest() : config_(setupConfig()), filter_(config_) {
    filter_.setDecoderFilterCallbacks(callbacks_);
  }

  void routeLocalConfig(const Router::RouteSpecificFilterConfig* route_settings,
                        const Router::RouteSpecificFilterConfig* vhost_settings) {
    ON_CALL(callbacks_.route_->route_entry_, perFilterConfig(HttpFilterNames::get().GcpEventsConvert))
        .WillByDefault(Return(route_settings));
    ON_CALL(callbacks_.route_->route_entry_.virtual_host_,
            perFilterConfig(HttpFilterNames::get().GcpEventsConvert))
        .WillByDefault(Return(vhost_settings));
  }

  NiceMock<Http::MockStreamDecoderFilterCallbacks> callbacks_;
  GcpEventsConvertFilterConfigSharedPtr config_;
  GcpEventsConvertFilter filter_;
  // Create a runtime loader, so that tests can manually manipulate runtime guarded features.
  TestScopedRuntime scoped_runtime;
};

TEST_F(GcpEventsConvertFilterTest, HeaderOnlyRequest) {
  Http::TestRequestHeaderMapImpl headers{
    {":method", "GET"}, {":path", "/"}, {":authority", "host"}};
  EXPECT_EQ(Http::FilterHeadersStatus::Continue, filter_.decodeHeaders(headers, true));

  const Http::HeaderEntry* entry = headers.get(Http::LowerCaseString("some random key"));
  EXPECT_TRUE(entry != nullptr);
  EXPECT_EQ(entry->value() , "some random value");
}

TEST_F(GcpEventsConvertFilterTest, TestMetadata) {
  Http::MetadataMap metadata_map{{"metadata", "metadata"}};
  EXPECT_EQ(Http::FilterMetadataStatus::Continue, filter_.decodeMetadata(metadata_map));
}

TEST_F(GcpEventsConvertFilterTest, RequestWithDataAndTrailer) {
  InSequence s;

  Http::TestRequestHeaderMapImpl headers;
  EXPECT_EQ(Http::FilterHeadersStatus::Continue, filter_.decodeHeaders(headers, false));

  Buffer::OwnedImpl data1("hello");
  EXPECT_EQ(Http::FilterDataStatus::Continue, filter_.decodeData(data1, false));

  Buffer::OwnedImpl data2(" world");
  EXPECT_EQ(Http::FilterDataStatus::Continue, filter_.decodeData(data2, true));

  Http::TestRequestTrailerMapImpl trailers;
  EXPECT_EQ(Http::FilterTrailersStatus::Continue, filter_.decodeTrailers(trailers));
}

} // namespace GcpEventsConvert
} // namespace HttpFilters
} // namespace Extensions
} // namespace Envoy
