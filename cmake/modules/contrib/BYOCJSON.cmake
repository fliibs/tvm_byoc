
tvm_file_glob(GLOB CSOURCE_RELAY_CONTRIB_SRC src/relay/backend/contrib/byoc_json/**)
list(APPEND COMPILER_SRCS ${CSOURCE_RELAY_CONTRIB_SRC})