`timescale 1ns / 1ps

module multi_barrel_shifter_mux 
#(parameter N = 3, parameter Width = 2**N)
    (
        input logic [Width-1:0] a,
        input logic [N-1:0] amt,
        input logic select,
        output logic [Width-1:0] y
    );

    eire [Width-1:0] y_right;
    wire [Width-1:0] y_left;

    param_right_shifter # (N,Width) right_shift (.a(a),.amt(amt),.y(y_right));
    param_left_shifter # (N,Width) left_shift (.a(a),.amt(amt),.y(y_left));

    assign y = select ? y_right : y_left;
    
endmodule
