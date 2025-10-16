`timescale 1ns / 1ps

module param_right_shifter
    #(parameter N = 3, parameter Width = 2**N)
    (
        input  logic [Width-1:0] a,
        input  logic [N-1:0] amt,
        output logic [Width-1:0] y
    );
    
    generate
        genvar i;
        for (i = 0; i < Width; i = i + 1) begin
            assign y[i] = a[(i + amt) % (Width)];
        end
    endgenerate
    
endmodule

module param_left_shifter
    #(parameter N = 3, parameter Width = 2**N)
    (
        input  logic [Width-1:0] a,
        input  logic [N-1:0] amt,
        output logic [Width-1:0] y
    );
    
    generate
        genvar i;
        for (i = 0; i < Width; i = i + 1) begin
            assign y[i] = a[(i - amt + (Width)) % (Width)];
        end
    endgenerate

endmodule
