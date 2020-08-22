#include "extensions/grpc_stream_demuxer/config.h"

namespace Envoy {
namespace Extensions {
namespace GrpcStreamDemuxer {

GrpcStreamDemuxerPtr GrpcStreamDemuxerFactoryImpl::createGrpcStreamDemuxer() {
  // TODO (vmunukutla): Remove sample parameters and replace with parameters
  // extracted from GRPCStreamDemuxer proto.
  return std::make_unique<GrpcStreamDemuxer>("subscription", "localhost", 10000);
}

REGISTER_FACTORY(GrpcStreamDemuxerFactoryImpl, GrpcStreamDemuxerFactory);

} // namespace GrpcStreamDemuxer
} // namespace Extensions
} // namespace Envoy
