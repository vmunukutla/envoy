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
  const std::string yaml = R"EOF(
  subscription: "test_subscription_1"
  address: 0.0.0.1
  port: 10001
  )EOF";
  envoy::extensions::grpc_stream_demuxer::v3alpha::GrpcStreamDemuxer demuxer_object;
  TestUtility::loadFromYaml(yaml, demuxer_object);
  auto& factory = Config::Utility::getAndCheckFactoryByName<Envoy::GrpcStreamDemuxer::GrpcStreamDemuxerFactory>("grpc_stream_demuxer");
  Envoy::GrpcStreamDemuxer::GrpcStreamDemuxerPtr demuxer = factory.createGrpcStreamDemuxer(demuxer_object);
  EXPECT_THAT(demuxer, testing::NotNull());
}

// Basic test to make sure GrpcStreamDemuxer initialization from an invalid factory name fails.
TEST(GrpcStreamDemuxerTest, InvalidGrpcStreamDemuxerFactory) {
  EXPECT_THROW(Config::Utility::getAndCheckFactoryByName<Envoy::GrpcStreamDemuxer::GrpcStreamDemuxerFactory>("invalid_factory_name"), 
    EnvoyException);
}

} // namespace GrpcStreamDemuxer
} // namespace Extensions
} // namespace Envoy
