add_library(wml SHARED
    config.cpp
    parser.cpp
    tokenizer.cpp
)
set_property(TARGET wml PROPERTY COMPILE_FLAGS -std=c++14)
