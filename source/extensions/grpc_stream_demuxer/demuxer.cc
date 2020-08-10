#include "extensions/grpc_stream_demuxer/demuxer.h"

namespace Envoy {
namespace Demuxer {

GRPCStreamDemuxer::GRPCStreamDemuxer(std::string subscription, std::string address, int port) 
    : subscription_(subscription), address_(address), port_(port) {}

void GRPCStreamDemuxer::start() {
    // TODO (vmunukutla): Implement so that it forms a streaming pull connection
    // to subscription_, waits for messages to be published to subscription_, and
    // sends unary grpc requests to address:port when messages are published to
    // subscription_.
    std::cout << "Subscription: " << subscription_ << std::endl;
    std::cout << "Address: " << address_ << std::endl;
    std::cout << "Port: " << port_ << std::endl;
}

} // namespace Demuxer
} // namespace Envoy
