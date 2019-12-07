idf_component_register(SRCS ./seatest.cpp
                       REQUIRES nvs_flash spi
                       EMBED_FILES ../fpga/seatest.runs/impl_1/Blinky.bit)

