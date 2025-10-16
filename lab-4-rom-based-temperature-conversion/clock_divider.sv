`timescale 1ns / 1ps

module clock_divider #(parameter DIV_FACTOR = 100_000)(
    input clk_in,
    output reg clk_out
);

    reg [31:0] counter = 0;

    always @(posedge clk_in) begin
        counter <= counter + 1;
        if (counter >= (DIV_FACTOR - 1)) begin
            counter <= 0;
            clk_out <= ~clk_out;
        end
    end
endmodule
