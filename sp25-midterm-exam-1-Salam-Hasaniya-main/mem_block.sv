`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 02/27/2022 07:29:58 PM
// Design Name: 
// Module Name: mem_block
// Project Name: 
// Target Devices: 
// Tool Versions: 
// Description: 
// 
// Dependencies: 
// 
// Revision:
// Revision 0.01 - File Created
// Additional Comments:
// 
//////////////////////////////////////////////////////////////////////////////////


module mem_block(
    input logic clk, 
    input logic we, //write enable
    input logic [11:0] addr,
    input logic [3:0] din,
    output logic [3:0] dout
    );
    
    // TODO: Write your code here 
    // DO NOT CHANGE THE MODULE INTERFACE
    
    logic [3:0] dout1,dout2,dout3,dout4;
 
    bram_synch_one_port # (10,4) BRAM1 ( .clk(clk), .we(we & (addr[11:10]==2'b00)), .addr_a(addr[9:0]), .din_a(din), .dout_a(dout1));
    bram_synch_one_port # (10,4) BRAM2 ( .clk(clk), .we(we & (addr[11:10]==2'b01)), .addr_a(addr[9:0]), .din_a(din), .dout_a(dout2));
    bram_synch_one_port # (10,4) BRAM3 ( .clk(clk), .we(we & (addr[11:10]==2'b10)), .addr_a(addr[9:0]), .din_a(din), .dout_a(dout3));
    bram_synch_one_port # (10,4) BRAM4 ( .clk(clk), .we(we & (addr[11:10]==2'b11)), .addr_a(addr[9:0]), .din_a(din), .dout_a(dout4));
    
    always_comb begin
        case (addr[11:10])
          2'b00: dout = dout1 ;
          2'b01: dout = dout2;
          2'b10: dout = dout3 ;
          2'b11: dout = dout4 ;
     endcase
    end
endmodule
