#pragma once

#include <iostream>
#include <memory>
#include <string>

namespace Envoy {
namespace Demuxer {

class GRPCStreamDemuxer {

public:
  GRPCStreamDemuxer(std::string subscription, std::string address, int port);

  void start();

private:
  std::string subscription_;
  std::string address_;
  int port_;
};

using GRPCStreamDemuxerPtr = std::unique_ptr<GRPCStreamDemuxer>;

} // namespace Demuxer
} // namespace Envoy

