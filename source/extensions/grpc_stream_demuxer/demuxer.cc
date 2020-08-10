#include "extensions/grpc_stream_demuxer/demuxer.h"

namespace Envoy {
namespace Demuxer {

GRPCStreamDemuxer::GRPCStreamDemuxer(std::string subscription, std::string address, int port) 
    : subscription_(subscription), address_(address), port_(port) {}

void GRPCStreamDemuxer::start() {
    std::cout << "Subscription: " << subscription_ << std::endl;
    std::cout << "Address: " << address_ << std::endl;
    std::cout << "Port: " << port_ << std::endl;
}

} // namespace Demuxer
} // namespace Envoy
