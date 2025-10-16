`timescale 1ns / 1ps

module fifo_ctrl
    #(parameter ADDR_WIDTH = 3)
    (
        input logic clk, reset,
        input logic wr, rd,
        output logic full, empty,
        output logic [ADDR_WIDTH - 1: 0] w_addr,
        output logic [ADDR_WIDTH - 1: 0] r_addr
    );
    
    logic [ADDR_WIDTH - 1: 0] wr_ptr, wr_ptr_next;
    logic [ADDR_WIDTH - 1: 0] rd_ptr, rd_ptr_next;
    
    logic full_next;
    logic empty_next;
    
    // registers for status and read/write pointers
    always_ff @(posedge clk, posedge reset)
    begin
        if(reset)
        begin
            wr_ptr <= 0;
            rd_ptr <= 0;
            full <= 1'b0;
            empty <= 1'b1;
        end
        else
        begin
            wr_ptr <= wr_ptr_next;
            rd_ptr <= rd_ptr_next;
            full <= full_next;
            empty <= empty_next;
        end
    end
    
    always_comb
    begin
        // default is to keep old (current) values
        wr_ptr_next = wr_ptr;
        rd_ptr_next = rd_ptr;
        full_next = full;
        empty_next = empty;
        unique case({wr, rd})
            2'b01: // read
            begin
                if (~empty)
                begin
                    rd_ptr_next = rd_ptr + 1;
                    full_next = 1'b0;
                      if ((wr_ptr_next == rd_ptr)) // prevents writing if either half of word is read
                        full_next = 1'b1;
                
                    if (rd_ptr_next == wr_ptr) //checks for half word
                        empty_next = 1'b1;
                end                
            end
            
            2'b10: // write
            begin
                if (~full)
                begin
                    wr_ptr_next = wr_ptr + 2; // increment by 2
                    empty_next = 1'b0;
                    if ((wr_ptr_next == rd_ptr)|| (wr_ptr_next + 1 == rd_ptr)) // prevents writing if either half of word is read
                        full_next = 1'b1;
                end
            end
            
            2'b11: // read and write simultaneously 
            begin
                if (empty)
                begin
                    wr_ptr_next = wr_ptr + 2;
                    rd_ptr_next = rd_ptr;
                    empty_next = 1'b0;
                end 
                else
                begin
                    wr_ptr_next = wr_ptr + 2;
                    rd_ptr_next = rd_ptr + 1;
                end
            end
            
            default: ; // case 2'b00; null statements; no op
        endcase       
    end
    
    // outputs
    assign w_addr = wr_ptr;
    assign r_addr = rd_ptr;
    
endmodule