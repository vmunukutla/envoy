#include "common/config/utility.h"
#include "extensions/grpc_stream_demuxer/config.h"

#include "gmock/gmock.h"
#include "gtest/gtest.h"

namespace Envoy {
namespace Extensions {
namespace GrpcStreamDemuxer {

// Basic test to make sure GrpcStreamDemuxer initialization is successful.
TEST(GrpcStreamDemuxerTest, CreateGrpcStreamDemuxer) {
  auto& factory = Config::Utility::getAndCheckFactoryByName<GrpcStreamDemuxerFactory>("grpc_stream_demuxer");
  GrpcStreamDemuxerPtr demuxer = factory.createGrpcStreamDemuxer();
  EXPECT_THAT(demuxer, testing::NotNull());
}

} // namespace GrpcStreamDemuxer
} // namespace Extensions
} // namespace Envoy
