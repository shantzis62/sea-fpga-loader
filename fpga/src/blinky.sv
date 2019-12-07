`timescale 1ns / 1ps
module Blinky(input i_clk,
              output reg o_led,
              output reg o_out0,
              output reg o_out1,
              input wire i_passthru0,
              output wire o_passthru0,
              input wire i_passthru1,
              output wire o_passthru1);
  localparam COUNT = 100000000;
  reg [$clog2(COUNT)-1:0] counter;
  always @ (posedge i_clk) begin
    if (counter < COUNT) begin
      counter <= counter + 1;
    end
    else begin
      counter <= 0;
      o_led <= ~o_led;
      o_out0 <= ~o_out0;
      o_out1 <= ~o_out1;
    end
  end
  assign o_passthru0 = i_passthru0;
  assign o_passthru1 = i_passthru1;
endmodule
