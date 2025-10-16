`timescale 1ns / 1ps

module F2C_ROM(
    input logic clk,
    input logic [7:0] addr,
    output logic [7:0] data
);

    // signal declaration
    (*rom_style = "block" *)logic [7:0] F2C_ROM [0:255];
    
    initial
      $readmemb("F2C.mem", F2C_ROM);
        
    always_ff @(posedge clk)
        data <= F2C_ROM[addr];
endmodule
