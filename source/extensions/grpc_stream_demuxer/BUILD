load(
    "//bazel:envoy_build_system.bzl",
    "envoy_cc_extension",
    "envoy_cc_library",
    "envoy_package",
)

licenses(["notice"])  # Apache 2

envoy_package()

envoy_cc_library(
    name = "grpc_stream_demuxer",
    srcs = ["grpc_stream_demuxer.cc"],
    hdrs = ["grpc_stream_demuxer.h"],
    deps = [
        "//source/common/common:minimal_logger_lib",
    ],
)

envoy_cc_library(
    name = "grpc_stream_demuxer_factory_lib",
    hdrs = ["grpc_stream_demuxer_factory.h"],
    deps = [
        ":grpc_stream_demuxer",
        "//include/envoy/config:typed_config_interface",
    ],
)

envoy_cc_extension(
    name = "config",
    srcs = ["config.cc"],
    hdrs = ["config.h"],
    deps = [      
        ":grpc_stream_demuxer_factory_lib",      
        "//include/envoy/registry",
    ],
    security_posture = "unknown",
    status = "wip",
)
