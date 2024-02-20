if(USE_BYOC)
    tvm_file_glob(GLOB CSOURCE_RELAY_CONTRIB_SRC src/relay/backend/contrib/byoc_c/**.cc)
    list(APPEND COMPILER_SRCS ${CSOURCE_RELAY_CONTRIB_SRC})

    tvm_file_glob(GLOB BYOC_C_CONTRIB_SRC src/runtime/contrib/byoc_c/byoc_c.cc)
    list(APPEND RUNTIME_SRCS ${BYOC_C_CONTRIB_SRC})

endif()