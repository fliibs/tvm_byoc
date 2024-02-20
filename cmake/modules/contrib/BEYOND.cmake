if(USE_BEYOND)
    tvm_file_glob(GLOB CSOURCE_RELAY_CONTRIB_SRC src/relay/backend/contrib/beyond/**.cc)
    list(APPEND COMPILER_SRCS ${CSOURCE_RELAY_CONTRIB_SRC})

    tvm_file_glob(GLOB BYOC_FUNC_CONTRIB_SRC src/runtime/contrib/byoc_func/**.cc)
    list(APPEND RUNTIME_SRCS ${BYOC_FUNC_CONTRIB_SRC})

endif()