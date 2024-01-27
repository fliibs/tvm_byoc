tvm_file_glob(GLOB CSOURCE_RELAY_CONTRIB_SRC    src/relay/backend/contrib/byoc_json/codegen_json.h
                                                src/relay/backend/contrib/byoc_json/codegen_json.cc
                                                src/runtime/contrib/byoc_json/byoc_json_runtime.cc)
list(APPEND COMPILER_SRCS ${CSOURCE_RELAY_CONTRIB_SRC})