#pragma once

#include "envoy/event/dispatcher.h"
#include "envoy/service/pubsub/v3alpha/received_message.grpc.pb.h"

#include "common/common/logger.h"

#include "google/pubsub/v1/pubsub.grpc.pb.h"
#include "grpc++/grpc++.h"

using grpc::Channel;
using grpc::ClientContext;
using grpc::Status;
using envoy::service::pubsub::v3alpha::ReceivedMessageService;
using google::pubsub::v1::ReceivedMessage;

namespace Envoy {
namespace Extensions {
namespace GrpcStreamDemuxer {

class ReceivedMessageServiceClient {
  public:
    ReceivedMessageServiceClient(std::shared_ptr<Channel> channel)
      : stub_(ReceivedMessageService::NewStub(channel)) {}
    
    // Assembles the client's payload, sends it and presents the response back
    // from the server.
    std::string SendReceivedMessage(const ReceivedMessage &request) {

      // Container for the data we expect from the server.
      google::protobuf::Empty reply;

      // Context for the client. It could be used to convey extra information to
      // the server and/or tweak certain RPC behaviors.
      ClientContext context;

      // The actual RPC.
      Status status = stub_->SendReceivedMessage(&context, request, &reply);

      // Act upon its status.
      if (status.ok()) {
        return "RPC worked";
      } else {
        std::cout << status.error_code() << ": " << status.error_message()
                  << std::endl;
        return "RPC failed";
      }
    }

  private:
    std::unique_ptr<ReceivedMessageService::Stub> stub_;
};

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
};

using GrpcStreamDemuxerPtr = std::unique_ptr<GrpcStreamDemuxer>;

} // namespace GrpcStreamDemuxer
} // namespace Extensions
} // namespace Envoy
