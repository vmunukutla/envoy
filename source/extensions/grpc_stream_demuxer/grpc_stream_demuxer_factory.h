#pragma once

#include "envoy/config/typed_config.h"
#include "envoy/extensions/grpc_stream_demuxer/v3alpha/grpc_stream_demuxer.pb.h"

#include "extensions/grpc_stream_demuxer/grpc_stream_demuxer.h"

namespace Envoy {
namespace GrpcStreamDemuxer {

class GrpcStreamDemuxerFactory : public Config::UntypedFactory {
public:
  ~GrpcStreamDemuxerFactory() override = default;

  /**
   * TODO (vmunukutla): Pass in GRPCStreamDemuxer proto as parameter after
   * fixing current build issues that come with adding GRPCStreamDemuxer proto
   * to codebase.
   */
  virtual GrpcStreamDemuxerPtr createGrpcStreamDemuxer(const envoy::extensions::grpc_stream_demuxer::v3alpha::GrpcStreamDemuxer& demuxer_object) PURE;
  std::string category() const override { return "envoy.grpc_stream_demuxer"; }
};

} // namespace GrpcStreamDemuxer
} // namespace Envoy