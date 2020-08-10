#pragma once

#include "envoy/registry/registry.h"

#include "extensions/grpc_stream_demuxer/grpc_stream_demuxer_factory.h"


namespace Envoy {
namespace Demuxer {

class GRPCStreamDemuxerFactoryImpl : public GRPCStreamDemuxerFactory {
public:
  /**
   * Create a GRPCStreamDemuxer instance.
   * TODO (vmunukutla): Pass in GRPCStreamDemuxer proto as parameter after
   * fixing current build issues that come with adding GRPCStreamDemuxer proto
   * to codebase.
   */
  GRPCStreamDemuxerPtr createGPRCStreamDemuxer() override;
  std::string name() const override { return "grpc_stream_demuxer"; }
};

DECLARE_FACTORY(GRPCStreamDemuxerFactoryImpl);

} // namespace Demuxer
} // namespace Envoy