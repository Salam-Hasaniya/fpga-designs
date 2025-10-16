`timescale 1ns / 1ps

module C2F_ROM(
    input logic clk,
    input logic [7:0] addr,
    output logic [7:0] data
);

    // signal declaration
    (*rom_style = "block" *)logic [7:0] C2F_ROM [0:255];
    
    initial
        $readmemb("C2F.mem", C2F_ROM);
        
    always_ff @(posedge clk)
        data <= C2F_ROM[addr];
endmodule
