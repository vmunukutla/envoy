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

namespace Envoy {
namespace Extensions {
namespace HttpFilters {
namespace GcpEventsConvert {

using testing::InSequence;
using testing::NiceMock;
using testing::Return;

TEST(GcpEventsConvertFilterUnitTest, DecodeHeaderTestWtihLowerCases) {
  envoy::extensions::filters::http::gcp_events_convert::v3::GcpEventsConvert config;
  config.set_key("some random key");
  config.set_val("some random value");
  GcpEventsConvertFilter filter(std::make_shared<GcpEventsConvertFilterConfig>(config));
  Http::TestRequestHeaderMapImpl headers;

  EXPECT_EQ(Http::FilterHeadersStatus::Continue, filter.decodeHeaders(headers, true));
  const Http::HeaderEntry* entry = headers.get(Http::LowerCaseString("some random key"));
  ASSERT_THAT(entry, testing::NotNull());
  EXPECT_EQ(entry->value() , "some random value");
}

TEST(GcpEventsConvertFilterUnitTest, DecodeHeaderTestWithUpperCaseKey) {
  envoy::extensions::filters::http::gcp_events_convert::v3::GcpEventsConvert config;
  config.set_key("SOME RANDOM KEY");
  config.set_val("some random value");
  GcpEventsConvertFilter filter(std::make_shared<GcpEventsConvertFilterConfig>(config));
  Http::TestRequestHeaderMapImpl headers;

  EXPECT_EQ(Http::FilterHeadersStatus::Continue, filter.decodeHeaders(headers, true));
  const Http::HeaderEntry* entry = headers.get(Http::LowerCaseString("some random key"));
  ASSERT_THAT(entry, testing::NotNull());
  EXPECT_EQ(entry->value() , "some random value");
}

TEST(GcpEventsConvertFilterUnitTest, DecodeHeaderTestWithMixedCaseValue) {
  envoy::extensions::filters::http::gcp_events_convert::v3::GcpEventsConvert config;
  config.set_key("some random key");
  config.set_val("some random MIX value");
  GcpEventsConvertFilter filter(std::make_shared<GcpEventsConvertFilterConfig>(config));
  Http::TestRequestHeaderMapImpl headers;

  EXPECT_EQ(Http::FilterHeadersStatus::Continue, filter.decodeHeaders(headers, true));
  const Http::HeaderEntry* entry = headers.get(Http::LowerCaseString("some random key"));
  ASSERT_THAT(entry, testing::NotNull());
  EXPECT_EQ(entry->value() , "some random MIX value");
}

TEST(GcpEventsConvertFilterUnitTest, DecodeDataTestWithOneBody) {
  envoy::extensions::filters::http::gcp_events_convert::v3::GcpEventsConvert config;
  config.set_key("some random key");
  config.set_val("some random value");
  GcpEventsConvertFilter filter(std::make_shared<GcpEventsConvertFilterConfig>(config));

  Buffer::OwnedImpl data1("hello");
  EXPECT_EQ(Http::FilterDataStatus::Continue, filter.decodeData(data1, false));

  Buffer::OwnedImpl data2;
  EXPECT_EQ(Http::FilterDataStatus::Continue, filter.decodeData(data1, true));
}

TEST(GcpEventsConvertFilterUnitTest, DecodeDataTestWithMultipleBody) {
  envoy::extensions::filters::http::gcp_events_convert::v3::GcpEventsConvert config;
  config.set_key("some random key");
  config.set_val("some random value");
  GcpEventsConvertFilter filter(std::make_shared<GcpEventsConvertFilterConfig>(config));

  Buffer::OwnedImpl data1("hello");
  EXPECT_EQ(Http::FilterDataStatus::Continue, filter.decodeData(data1, false));

  Buffer::OwnedImpl data2(" world");
  EXPECT_EQ(Http::FilterDataStatus::Continue, filter.decodeData(data2, false));

  Buffer::OwnedImpl data3("! ");
  EXPECT_EQ(Http::FilterDataStatus::Continue, filter.decodeData(data3, false));

  Buffer::OwnedImpl data4;
  EXPECT_EQ(Http::FilterDataStatus::Continue, filter.decodeData(data4, true));
}

TEST(GcpEventsConvertFilterUnitTest, DecodeDataTestWithoutBody) {
  envoy::extensions::filters::http::gcp_events_convert::v3::GcpEventsConvert config;
  config.set_key("some random key");
  config.set_val("some random value");
  GcpEventsConvertFilter filter(std::make_shared<GcpEventsConvertFilterConfig>(config));

  Buffer::OwnedImpl data;
  EXPECT_EQ(Http::FilterDataStatus::Continue, filter.decodeData(data, true));
}

TEST(GcpEventsConvertFilterUnitTest, DecodeTrailerTest) {
  envoy::extensions::filters::http::gcp_events_convert::v3::GcpEventsConvert config;
  config.set_key("some random key");
  config.set_val("some random value");
  GcpEventsConvertFilter filter(std::make_shared<GcpEventsConvertFilterConfig>(config));

  Http::TestRequestTrailerMapImpl trailer;
  EXPECT_EQ(Http::FilterTrailersStatus::Continue, filter.decodeTrailers(trailer));
}

} // namespace GcpEventsConvert
} // namespace HttpFilters
} // namespace Extensions
} // namespace Envoy
