#pragma once

#include <iostream>
#include <memory>
#include <string>

namespace Envoy {
namespace Demuxer {

class GRPCStreamDemuxer {

public:
  /**
   * Create a GRPCStreamDemuxer instance by setting private fields to paramaters
   * passed in.
   */
  GRPCStreamDemuxer(std::string subscription, std::string address, int port);

  /**
   * Create a streaming pull connection to subscription_ and process incoming
   * messages.
   */
  void start();

private:
  // Subscription name to connect to.
  std::string subscription_;
  // Address and port to forward unary grpc requests to.
  std::string address_;
  int port_;
};

using GRPCStreamDemuxerPtr = std::unique_ptr<GRPCStreamDemuxer>;

} // namespace Demuxer
} // namespace Envoy

