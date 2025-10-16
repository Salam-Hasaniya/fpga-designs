`timescale 1ns / 1ps

module reg_file
    #(parameter ADDR_WIDTH = 3, DATA_WIDTH = 4)
    (
        input logic clk,
        input logic w_en,
        input logic [ADDR_WIDTH - 1: 0] r_addr, // reading address
        input logic [ADDR_WIDTH - 1: 0] w_addr, // writing address
        input logic [2*DATA_WIDTH - 1: 0] w_data, // doubled
        output logic [DATA_WIDTH - 1: 0] r_data
    );
    
    // signal declaration
    logic [DATA_WIDTH - 1: 0] memory [0: 2 ** ADDR_WIDTH - 1];
    
    // write operation
    always_ff @(posedge clk) begin
        if (w_en) begin
            memory[w_addr]     <= w_data[DATA_WIDTH-1:0];            // lower half of write
            memory[w_addr + 1] <= w_data[2*DATA_WIDTH-1:DATA_WIDTH]; // upper half of write
        end
    end
    
    // Read operation
    assign r_data = memory[r_addr];
    
endmodule