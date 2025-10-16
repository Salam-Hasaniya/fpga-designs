`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 02/10/2025 01:33:03 PM
// Design Name: 
// Module Name: early_debouncer
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


module early_debouncer(

        input logic sw,
        input logic reset,
        input logic clk, // genrally the 100MHz
        output logic db
    );

    // 10 ms m_tick generator
    // assuming clk is 100 MHz (clock period is 10 ns)
    // 10 ms / 10 ns is 10e-3 / 10e-9 which is 1_000_000
    logic m_tick;
    mod_m_counter1 #(.M(1_000_000)) ticker (
                    .clk(clk),
                    .reset(reset),  
                    .q(),
                    .max_tick(m_tick)
    );
    
    
    // There is a bug somewhere here, if you click the switch many times and fast enough the output disappears
    typedef enum {zero, wait0_1, wait0_2, wait0_3, one, wait1_1, wait1_2, wait1_3} state_type;
    
    // signal declarations
    state_type state_reg, state_next;
    
    // state register
    always_ff @(posedge clk, posedge reset)
        if(reset)
            state_reg <= zero;
        else
            state_reg <= state_next;
            
    
    // next-state logic
    always_comb
    begin
        case(state_reg)
            zero:
                    if(sw)
                        state_next = wait0_1;
                    else
                        state_next = zero;
            wait0_1:
                   
                        if(m_tick)
                            state_next = wait0_2;
                        else
                            state_next = wait0_1;
                   
            wait0_2:
                     
                        if(m_tick)
                            state_next = wait0_3;
                        else
                            state_next = wait0_2;
                                       
            wait0_3:
                     if(m_tick)
                        if(sw)
                            state_next = one;
                        else 
                            state_next = wait1_2;
                    else
                        state_next = wait0_3;    
            one:
                    if(~sw)
                        state_next = wait1_1;
                    else
                        state_next = one;
            wait1_1:
                    
                        if(m_tick)
                            state_next = wait1_2;
                        else
                            state_next = wait1_1;
                    
            wait1_2:
                    
                        if(m_tick)
                            state_next = wait1_3;
                        else 
                            state_next = wait1_2;
                                    
            wait1_3:
                     if(m_tick)
                        if(sw)
                            state_next = wait0_2;
                        else
                            state_next = zero;
                    else
                        state_next = wait1_3; 
            default: state_next = zero;
        endcase
    end
    
    // Moore output logic
    assign db = (   (state_reg == one) || 
                    (state_reg == wait0_1) || 
                    (state_reg == wait0_2) || 
                    (state_reg == wait0_3));
endmodule
