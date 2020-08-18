#pragma once

#include <iostream>
#include <memory>
#include <string>

namespace Envoy {
namespace GrpcStreamDemuxer {

class GrpcStreamDemuxer {
public:
  GrpcStreamDemuxer(const std::string& subscription, const std::string& address, int port);

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

using GrpcStreamDemuxerPtr = std::unique_ptr<GrpcStreamDemuxer>;

} // namespace GrpcStreamDemuxer
} // namespace Envoy

