`timescale 1ns / 1ps

module seven_seg_display(
    input logic slow_clk,      // Slow clock for display multiplexing
    input logic format,        // 0 = C2F, 1 = F2C
    input logic [3:0] in_hundreds, in_tens, in_ones, // Input temperature BCD
    input logic [3:0] out_hundreds, out_tens, out_ones, // Output temperature BCD
    output logic [7:0] AN,      // Anode control for 8-digit display
    output logic [6:0] seg      // 7-segment display output
);

    logic [2:0] display_index = 0;
    logic [3:0] digit; // Current digit to be displayed

    // Cycles through each digit slot every slow clock cycle
    always_ff @(posedge slow_clk) begin
       display_index <= display_index + 1;
    end

    // Selects the appropriate digit based on display index
    always_comb begin
        case (display_index)
            3'd0: begin digit = in_hundreds;        AN = 8'b01111111; end // First digit of input
            3'd1: begin digit = in_tens;            AN = 8'b10111111; end // Second digit of input
            3'd2: begin digit = in_ones;            AN = 8'b11011111; end // Third digit of input
            3'd3: begin digit = format ? 4'hF : 4'hC; AN = 8'b11101111; end // 'F' or 'C'
            3'd4: begin digit = out_hundreds;       AN = 8'b11110111; end // First digit of output
            3'd5: begin digit = out_tens;           AN = 8'b11111011; end // Second digit of output
            3'd6: begin digit = out_ones;           AN = 8'b11111101; end // Third digit of output
            3'd7: begin digit = format ? 4'hC : 4'hF; AN = 8'b11111110; end // 'C' or 'F'
            default: begin digit = 4'h0;            AN = 8'b11111111; end // All off
        endcase
    end

    // 7-Segment Decoder (Embedded Directly)
    always_comb begin
        case (digit)
            4'h0: seg = 7'b1000000; // 0
            4'h1: seg = 7'b1111001; // 1
            4'h2: seg = 7'b0100100; // 2
            4'h3: seg = 7'b0110000; // 3
            4'h4: seg = 7'b0011001; // 4
            4'h5: seg = 7'b0010010; // 5
            4'h6: seg = 7'b0000010; // 6
            4'h7: seg = 7'b1111000; // 7
            4'h8: seg = 7'b0000000; // 8
            4'h9: seg = 7'b0010000; // 9
            4'hA: seg = 7'b0001000; // A
            4'hB: seg = 7'b0000011; // b
            4'hC: seg = 7'b1000110; // c
            4'hD: seg = 7'b0100001; // d
            4'hE: seg = 7'b0000110; // E
            4'hF: seg = 7'b0001110; // F
            default: seg = 7'b1111111; // All off
        endcase
    end

endmodule
