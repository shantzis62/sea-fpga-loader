create_clock -period 10.000 -name sys_clk -waveform {0.000 5.000} -add [get_ports i_clk]

set_property IOSTANDARD LVCMOS33 [get_ports i_clk]
set_property PACKAGE_PIN H4 [get_ports i_clk]

set_property IOSTANDARD LVCMOS33 [get_ports o_led]
set_property PACKAGE_PIN J1 [get_ports o_led]

# Arduino GPIO2
set_property IOSTANDARD LVCMOS33 [get_ports o_out0]
set_property PACKAGE_PIN A10 [get_ports o_out0]

# FPGA_IO0
set_property IOSTANDARD LVCMOS33 [get_ports o_out1]
set_property PACKAGE_PIN N14 [get_ports o_out1]

# UART (via FPGA)
# RX - passthru0: FPGA_IO2 (C4) to FPGA_QSPI_HD (D13, ESP GPIO21)
set_property IOSTANDARD LVCMOS33 [get_ports i_passthru0]
set_property PACKAGE_PIN C4 [get_ports i_passthru0]
set_property IOSTANDARD LVCMOS33 [get_ports o_passthru0]
set_property PACKAGE_PIN D13 [get_ports o_passthru0]
# TX - passthru1: FPGA_IO3 (B13) to FPGA_QSPI_WP (J13, ESP GPIO22)
set_property IOSTANDARD LVCMOS33 [get_ports o_passthru1]
set_property PACKAGE_PIN B13 [get_ports o_passthru1]
set_property IOSTANDARD LVCMOS33 [get_ports i_passthru1]
set_property PACKAGE_PIN J13 [get_ports i_passthru1]

# Voltage defs for loading config.
set_property CONFIG_VOLTAGE 3.3 [current_design]
set_property CFGBVS VCCO [current_design]
