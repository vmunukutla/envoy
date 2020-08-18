#pragma once

#include "envoy/registry/registry.h"

#include "extensions/grpc_stream_demuxer/grpc_stream_demuxer_factory.h"

namespace Envoy {
namespace GrpcStreamDemuxer {

class GrpcStreamDemuxerFactoryImpl : public GrpcStreamDemuxerFactory {
public:
  /**
   * TODO (vmunukutla): Pass in GrpcStreamDemuxer proto as parameter after
   * fixing current build issues that come with adding GrpcStreamDemuxer proto
   * to codebase.
   */
  GrpcStreamDemuxerPtr createGrpcStreamDemuxer() override;
  std::string name() const override { return "grpc_stream_demuxer"; }
};

DECLARE_FACTORY(GrpcStreamDemuxerFactoryImpl);

} // namespace GrpcStreamDemuxer
} // namespace Envoy
