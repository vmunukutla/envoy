#include "google/pubsub/v1/pubsub.grpc.pb.h"
#include "grpc++/grpc++.h"

#include "extensions/grpc_stream_demuxer/demuxer.h"

namespace Envoy {
namespace Demuxer {

using grpc::ClientContext;
using grpc::ClientReaderWriter;
using google::pubsub::v1::Subscriber;
using google::pubsub::v1::StreamingPullRequest;
using google::pubsub::v1::StreamingPullResponse;

GRPCStreamDemuxer::GRPCStreamDemuxer(std::string subscription, std::string address, int port) 
    : subscription_(subscription), address_(address), port_(port) {}

void GRPCStreamDemuxer::start() {
    std::cout << "Address: " << address_ << std::endl;
    std::cout << "Port: " << port_ << std::endl;
    auto creds = grpc::GoogleDefaultCredentials();
    auto stub = Subscriber::NewStub(
        grpc::CreateChannel("pubsub.googleapis.com", creds));      

    // Open up the stream.
    ClientContext ctx;
    std::unique_ptr<ClientReaderWriter<
        StreamingPullRequest, StreamingPullResponse>> stream(
            stub->StreamingPull(&ctx));

    // Send initial message.
    StreamingPullRequest request;
    request.set_subscription(
        "projects/eventflow-interns-sandbox/subscriptions/vikas_sub1");
    request.set_stream_ack_deadline_seconds(10);
    stream->Write(request);

    // Receive messages.
    StreamingPullResponse response;
    while (stream->Read(&response)) {
      // Ack messages.
      StreamingPullRequest ack_request;
      for (const auto &message : response.received_messages()) {
        // Print the data from the message.
        std::cout << message.message().data() << std::endl;
        ack_request.add_ack_ids(message.ack_id());
      }
      stream->Write(ack_request);
    }
}

} // namespace Demuxer
} // namespace Envoy
