`timescale 1ns / 1ps

module bin_to_bcd (
    input [7:0] binary,      // 8-bit binary input
    output reg [3:0] ones,   // Ones place
    output reg [3:0] tens,   // Tens place
    output reg [3:0] hundreds // Hundreds place
);

    integer i;
    
    always @(*) begin
        // Reset BCD outputs
        ones = 4'd0;
        tens = 4'd0;
        hundreds = 4'd0;
        
        // Shift and Add-3 Algorithm
        for (i = 7; i >= 0; i = i - 1) begin
            if (hundreds >= 5) hundreds = hundreds + 3;
            if (tens >= 5) tens = tens + 3;
            if (ones >= 5) ones = ones + 3;
            
            // Shift left (binary << 1) and append new bit
            hundreds = {hundreds[2:0], tens[3]};
            tens = {tens[2:0], ones[3]};
            ones = {ones[2:0], binary[i]};
        end
    end
endmodule
