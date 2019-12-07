`timescale 1ns / 1ps
module BlinkyTb();
  reg clk = 1'b0;
  reg led = 1'b0;

  Blinky blinky(.i_clk(clk), .o_led(led));

  initial begin
    forever begin
      # 5
      clk = ~clk;
    end
  end

endmodule
