#include "extensions/grpc_stream_demuxer/grpc_stream_demuxer.h"

namespace Envoy {
namespace Extensions {
namespace GrpcStreamDemuxer {

GrpcStreamDemuxer::GrpcStreamDemuxer(const std::string& subscription, const std::string& address, int port, Event::Dispatcher& dispatcher) 
  : subscription_(subscription), address_(address), port_(port) {   	
  auto creds = grpc::GoogleDefaultCredentials();
  auto stub = Subscriber::NewStub(
    grpc::CreateChannel("pubsub.googleapis.com", creds));      

  // Open up the stream.
  // CompletionQueue cq;
  ClientContext ctx;
  unsigned int client_connection_timeout = 2;
  std::chrono::system_clock::time_point deadline = 
	  std::chrono::system_clock::now() + std::chrono::seconds(client_connection_timeout);
  ctx.set_deadline(deadline);
  // stream_ = std::move(stub->StreamingPull(&ctx));
  
  // Send initial message.
  // StreamingPullRequest request;
  // request.set_subscription(subscription_);
  // request.set_stream_ack_deadline_seconds(10);
  // stream_->Write(request);
  interval_timer_ = dispatcher.createTimer([this]() -> void { startTimer(); });
  interval_timer_->enableTimer(std::chrono::milliseconds(5000));
}

void GrpcStreamDemuxer::startTimer() {
  ENVOY_LOG(debug, "Firing Timer!");
  interval_timer_->enableTimer(std::chrono::milliseconds(5000));
}

void GrpcStreamDemuxer::start() {
  ENVOY_LOG(debug, "Firing timer!");
  // Receive messages.
  StreamingPullResponse response;
  ENVOY_LOG(debug, "one");
  // stream_->Read(&response);
  ENVOY_LOG(debug, "two");
  // while (true) {
    // Ack messages.
    StreamingPullRequest ack_request;
    for (const auto &message : response.received_messages()) {
      // Print the data from the message.
      ENVOY_LOG(info, "Pubsub message data: {}", message.message().data());
      ack_request.add_ack_ids(message.ack_id());
    }
    // stream_->Write(ack_request);
  // }  
  ENVOY_LOG(debug, "Address: {}", address_);
  ENVOY_LOG(debug, "Port: {}", port_);
}

} // namespace GrpcStreamDemuxer
} // namespace Extensions
} // namespace Envoy
