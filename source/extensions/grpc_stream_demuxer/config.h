#pragma once

#include "envoy/config/typed_config.h"
#include "extensions/grpc_stream_demuxer/demuxer.h"


namespace Envoy {
namespace Demuxer {

class GRPCStreamDemuxerFactory : public Config::UntypedFactory {
public:
  ~GRPCStreamDemuxerFactory() override = default;

  /**
   * Create a GRPCStreamDemuxer instance.
   */
  GRPCStreamDemuxerPtr createGPRCStreamDemuxer();
  std::string name() const override { return "grpc_stream_demuxer"; }
  std::string category() const override { return "envoy.grpc_stream_demuxer"; }
};

} // namespace Demuxer
} // namespace Envoy