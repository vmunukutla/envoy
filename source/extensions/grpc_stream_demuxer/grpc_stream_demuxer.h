#pragma once

#include "common/common/logger.h"

#include <iostream>
#include <memory>
#include <string>

namespace Envoy {
namespace GrpcStreamDemuxer {

class GrpcStreamDemuxer : Logger::Loggable<Logger::Id::config> {
public:
  /**
   * Create a GrpcStreamDemuxer object from the subscription, address, and port, which
   * are configured by the user in their yaml config. GrpcStreamDemuxer creates a 
   * streaming pull connection to the subscription to pull pubsub messages and sends
   * each pubsub message in a grpc request to the address and port.
   */
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

