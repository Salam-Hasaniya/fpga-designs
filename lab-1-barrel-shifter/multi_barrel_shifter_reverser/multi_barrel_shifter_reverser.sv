`timescale 1ns / 1ps

module multi_barrel_shifter_reverser
#(parameter N = 3, parameter Width = 2**N)
(
    input logic [Width-1:0] a,
    input logic [N-1:0] amt,
    input logic select,
    output logic [Width-1:0] y
);

    logic [Width-1:0] pre_reversed, rotated, post_reversed;

    generate
        genvar i;
        for (i = 0; i < Width; i = i + 1) begin
            assign pre_reversed[i] = select ? a[Width-1-i] : a[i];
        end
    endgenerate

    param_right_shifter # (N, Width) right_shift (.a(pre_reversed), .amt(amt), .y(rotated) );

    generate
        for (i = 0; i < Width; i = i + 1) begin
            assign post_reversed[i] = select ? rotated[Width-1-i] : rotated[i];
        end
    endgenerate

    assign y = post_reversed;
endmodule
