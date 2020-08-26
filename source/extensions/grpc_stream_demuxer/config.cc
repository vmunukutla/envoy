#include "extensions/grpc_stream_demuxer/config.h"

namespace Envoy {
namespace Extensions {
namespace GrpcStreamDemuxer {

GrpcStreamDemuxerPtr GrpcStreamDemuxerFactoryImpl::createGrpcStreamDemuxer(const envoy::extensions::grpc_stream_demuxer::v3alpha::GrpcStreamDemuxer& demuxer_object, Event::Dispatcher& dispatcher) {
  return std::make_unique<GrpcStreamDemuxer>(demuxer_object.subscription(), demuxer_object.address(), demuxer_object.port(), dispatcher);
}

REGISTER_FACTORY(GrpcStreamDemuxerFactoryImpl, GrpcStreamDemuxerFactory);

} // namespace GrpcStreamDemuxer
} // namespace Extensions
} // namespace Envoy
