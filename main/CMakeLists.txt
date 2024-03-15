idf_component_register(
        SRCS "main.c"
        INCLUDE_DIRS "."
        PRIV_REQUIRES "driver" # TODO 2.1: add the ws2812b component as a dependency
)
