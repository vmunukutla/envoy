#include "common/config/utility.h"
#include "extensions/grpc_stream_demuxer/config.h"

#include "gtest/gtest.h"

namespace Envoy {
namespace Extensions {
namespace GrpcStreamDemuxer {

// Basic test to make sure GrpcStreamDemuxer initialization is successful.
TEST(GrpcStreamDemuxerTest, CreateGrpcStreamDemuxer) {
    auto& factory = Config::Utility::getAndCheckFactoryByName<Envoy::GrpcStreamDemuxer::GrpcStreamDemuxerFactory>("grpc_stream_demuxer");
    Envoy::GrpcStreamDemuxer::GrpcStreamDemuxerPtr demuxer = factory.createGrpcStreamDemuxer();
    EXPECT_NE(nullptr, demuxer);
}

} // namespace GrpsStreamDemuxer
} // namespace Extensions
} // namespace GrpcStreamDemuxer