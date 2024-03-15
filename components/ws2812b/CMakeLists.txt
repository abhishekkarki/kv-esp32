idf_component_register(
        SRCS "ws2812b.c"
        # could also do INCLUDE_DIRS "." but having public includes separate is a good practice
        # you can still include the header as usual with #include "ws2812b.h" and the compiler finds it
        # you could then add a private include directory with PRIV_INCLUDE_DIRS (e.g. ".")
        INCLUDE_DIRS "include"
        REQUIRES "driver" # TODO 2: add the ws2812b driver component as a requirement
)
