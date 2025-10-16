`timescale 1ns / 1ps

module fifo_tb;
    
    // signal declarations
    localparam DATA_WIDTH = 4; // Adjusted to match new FIFO
    localparam ADDR_WIDTH = 3;
    localparam T = 10; //clock period
    
    logic clk, reset;
    logic wr, rd;
    logic [2*DATA_WIDTH -1: 0] w_data; // Updated for 8-bit writes
    logic [DATA_WIDTH -1: 0] r_data;
    logic full, empty;
    
    // instantiate module under test
    fifo #(.DATA_WIDTH(DATA_WIDTH), .ADDR_WIDTH(ADDR_WIDTH)) uut (.*);
    
    // 10 ns clock running forever
    always
    begin
        clk = 1'b1;
        #(T / 2);
        clk = 1'b0;
        #(T / 2);
    end
    
    // reset for the first half cycle
    initial
    begin
        reset = 1'b1;
        rd = 1'b0;
        wr = 1'b0;
        @(negedge clk);
        reset = 1'b0;
    end
    
    //test vectors
    initial
    begin
        // ----------------EMPTY-----------------------
        // write random numbers
        @(negedge clk);
        w_data = 8'b10011011; 
        wr = 1'b1;     
        @(negedge clk);
        wr = 1'b0;
        
        repeat(1) @(negedge clk);
        w_data = 8'b01010100; 
        wr = 1'b1;
        @(negedge clk);
        wr = 1'b0;
        
        repeat(1) @(negedge clk);
        w_data = 8'b00101101; 
        wr = 1'b1;
        @(negedge clk);
        wr = 1'b0;        
        
        // Read one value
        repeat(1) @(negedge clk);
        rd = 1'b1;
        @(negedge clk);
        rd = 1'b0;        
        
        // Write again after reading
        repeat(1) @(negedge clk);
        w_data = 8'b11000011; 
        wr = 1'b1;
        @(negedge clk);
        wr = 1'b0;        
        
        repeat(1) @(negedge clk);
        w_data = 8'b01111010; 
        wr = 1'b1;
        @(negedge clk);
        wr = 1'b0;
        
        // Read second value
        repeat(1) @(negedge clk);
        rd = 1'b1;
        @(negedge clk);
        rd = 1'b0;
        
        // Write after second read
        repeat(1) @(negedge clk);
        w_data = 8'b10100101;
        wr = 1'b1;
        @(negedge clk);
        wr = 1'b0;
        
        // Read until empty
        repeat(1) @(negedge clk);
        rd = 1'b1;
        @(negedge clk);
        rd = 1'b0;
        
        repeat(1) @(negedge clk);
        rd = 1'b1;
        @(negedge clk);
        rd = 1'b0;
        
        repeat(1) @(negedge clk);
        rd = 1'b1;
        @(negedge clk);
        rd = 1'b0;
        
        repeat(1) @(negedge clk);
        rd = 1'b1;
        @(negedge clk);
        rd = 1'b0;
        
        repeat(1) @(negedge clk);
        rd = 1'b1;
        @(negedge clk);
        rd = 1'b0;
        
        repeat(1) @(negedge clk);
        rd = 1'b1;
        @(negedge clk);
        rd = 1'b0;
        
        repeat(1) @(negedge clk);
        rd = 1'b1;
        @(negedge clk);
        rd = 1'b0;
        
        repeat(1) @(negedge clk);
        rd = 1'b1;
        @(negedge clk);
        rd = 1'b0;
        
        repeat(1) @(negedge clk);
        rd = 1'b1;
        @(negedge clk);
        rd = 1'b0;
          
        // read & write at the same time
        repeat(1) @(negedge clk);
        w_data = 28;
        wr = 1'b1;
        rd = 1'b1;
        @(negedge clk)
        wr = 1'b0;
        rd = 1'b0;
        
         // Read until empty
        repeat(1) @(negedge clk);
        rd = 1'b1;
        @(negedge clk);
        rd = 1'b0;
        
        repeat(1) @(negedge clk);
        rd = 1'b1;
        @(negedge clk);
        rd = 1'b0;
        
        repeat(3) @(negedge clk);
        $stop;
    end
    
endmodule
