#include "extensions/grpc_stream_demuxer/config.h"

namespace Envoy {
namespace Demuxer {

GRPCStreamDemuxerPtr GRPCStreamDemuxerFactory::createGPRCStreamDemuxer() {
    // TODO (vmunukutla): Remove sample parameters and replace with parameters
    // extracted from GRPCStreamDemuxer proto.
    return std::make_unique<GRPCStreamDemuxer>("subscription", "localhost", 10000);
}

} // namespace Demuxer
} // namespace Envoy