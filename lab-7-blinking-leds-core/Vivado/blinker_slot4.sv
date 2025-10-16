`timescale 1ns / 1ps

module blinker_slot4 (
    input  logic clk,
    input  logic reset,
    // slot interface
    input  logic cs,
    input  logic read,
    input  logic write,
    input  logic [4:0] addr,
    input  logic [31:0] wr_data,
    output logic [31:0] rd_data,
    // external port    
    output logic [3:0] leds
);

    // declaration
    logic [31:0] blink_reg0, blink_reg1, blink_reg2, blink_reg3;
    logic wr_en;

    // blink counters
    logic [31:0] counter0, counter1, counter2, counter3;
    
    // LED states
    logic led0, led1, led2, led3;

    // write logic
    always_ff @(posedge clk or posedge reset) begin
        if (reset) begin
            blink_reg0 <= 16'd1000 * 100_000;  // default blink rates
            blink_reg1 <= 16'd500  * 100_000;
            blink_reg2 <= 16'd250  * 100_000;
            blink_reg3 <= 16'd125  * 100_000;
        end else if (wr_en) begin
            case (addr[3:2]) // addr bits 3:2 to select register
                2'b00: blink_reg0 <= wr_data[15:0] * 100_000; // LED 0
                2'b01: blink_reg1 <= wr_data[15:0] * 100_000; // LED 1
                2'b10: blink_reg2 <= wr_data[15:0] * 100_000; // LED 2
                2'b11: blink_reg3 <= wr_data[15:0] * 100_000; // LED 3
            endcase
        end
    end

    // decoding logic 
    assign wr_en = cs && write;

    // slot read interface
    assign rd_data = 0;

    // LED blinking Logic
    always_ff @(posedge clk or posedge reset) begin
        if (reset) begin
            counter0 <= 0; counter1 <= 0; counter2 <= 0; counter3 <= 0;
            led0 <= 0; led1 <= 0; led2 <= 0; led3 <= 0;
        end else begin
            if (counter0 >= blink_reg0) begin
                counter0 <= 0;
                led0 <= ~led0;
            end else counter0 <= counter0 + 1;

            if (counter1 >= blink_reg1) begin
                counter1 <= 0;
                led1 <= ~led1;
            end else counter1 <= counter1 + 1;

            if (counter2 >= blink_reg2) begin
                counter2 <= 0;
                led2 <= ~led2;
            end else counter2 <= counter2 + 1;

            if (counter3 >= blink_reg3) begin
                counter3 <= 0;
                led3 <= ~led3;
            end else counter3 <= counter3 + 1;
        end
    end

    // external output 
    assign leds = {led3, led2, led1, led0};

endmodule
