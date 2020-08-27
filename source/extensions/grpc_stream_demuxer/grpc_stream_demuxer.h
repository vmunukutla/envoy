#pragma once

#include "envoy/event/dispatcher.h"

#include "common/common/logger.h"

#include "google/pubsub/v1/pubsub.grpc.pb.h"
#include "grpc++/grpc++.h"

using grpc::ClientContext;
using grpc::ClientReaderWriter;
using google::pubsub::v1::Subscriber;
using google::pubsub::v1::StreamingPullRequest;
using google::pubsub::v1::StreamingPullResponse;

namespace Envoy {
namespace Extensions {
namespace GrpcStreamDemuxer {

/**
 * GrpcStreamDemuxer initiates a streaming pull connection to the subscription
 * to pull pubsub messages and sends each pubsub message in a grpc request to
 * the address and port.
 */
class GrpcStreamDemuxer : Logger::Loggable<Logger::Id::grpc_stream_demuxer> {
public:
  GrpcStreamDemuxer(const std::string& subscription, const std::string& address, int port, Event::Dispatcher& dispatcher);

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
  Event::TimerPtr interval_timer_;
  // ClientReaderWriter<StreamingPullRequest, StreamingPullResponse>* stream_;
};

using GrpcStreamDemuxerPtr = std::unique_ptr<GrpcStreamDemuxer>;

} // namespace GrpcStreamDemuxer
} // namespace Extensions
} // namespace Envoy
