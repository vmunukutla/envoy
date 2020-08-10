#include "extensions/grpc_stream_demuxer/config.h"

namespace Envoy {
namespace Demuxer {

GRPCStreamDemuxerPtr GRPCStreamDemuxerFactory::createGPRCStreamDemuxer() {
    return std::make_unique<GRPCStreamDemuxer>("subscription", "localhost", 10000);
}

} // namespace Demuxer
} // namespace Envoy