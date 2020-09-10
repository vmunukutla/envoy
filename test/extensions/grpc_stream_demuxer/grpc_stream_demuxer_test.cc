#include "envoy/extensions/grpc_stream_demuxer/v3alpha/grpc_stream_demuxer.pb.h"

#include "common/config/utility.h"
#include "extensions/grpc_stream_demuxer/config.h"
#include "test/test_common/utility.h"

#include "gmock/gmock.h"
#include "gtest/gtest.h"

namespace Envoy {
namespace Extensions {
namespace GrpcStreamDemuxer {

// Basic test to make sure GrpcStreamDemuxer proto initialization from yaml config is successful.
TEST(GrpcStreamDemuxerTest, CreateGrpcStreamDemuxerProto) {
  const std::string yaml = R"EOF(
  subscription: "test_subscription_0"
  address: 0.0.0.0
  port: 10000
  )EOF";
  envoy::extensions::grpc_stream_demuxer::v3alpha::GrpcStreamDemuxer demuxer_object;
  TestUtility::loadFromYaml(yaml, demuxer_object);
  EXPECT_EQ(demuxer_object.subscription(), "test_subscription_0");
  EXPECT_EQ(demuxer_object.address(), "0.0.0.0");
  EXPECT_EQ(demuxer_object.port(), 10000);
}

// Basic test to make sure GrpcStreamDemuxer proto initialization from invalid yaml config fails.
TEST(GrpcStreamDemuxerTest, InvalidGrpcStreamDemuxerProto) {
  const std::string yaml = R"EOF(
  invalid_field: "test_subscription_0"
  address: 0.0.0.0
  port: 10000
  )EOF";
  envoy::extensions::grpc_stream_demuxer::v3alpha::GrpcStreamDemuxer demuxer_object;
  EXPECT_THROW(TestUtility::loadFromYaml(yaml, demuxer_object), EnvoyException);
}

// Basic test to make sure GrpcStreamDemuxer initialization from yaml config is successful.
TEST(GrpcStreamDemuxerTest, CreateGrpcStreamDemuxer) {
  auto& factory = Config::Utility::getAndCheckFactoryByName<GrpcStreamDemuxerFactory>("grpc_stream_demuxer");
  GrpcStreamDemuxerPtr demuxer = factory.createGrpcStreamDemuxer();
  EXPECT_THAT(demuxer, testing::NotNull());
}

} // namespace GrpcStreamDemuxer
} // namespace Extensions
} // namespace Envoy
