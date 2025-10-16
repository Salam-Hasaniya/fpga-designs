`timescale 1ns / 1ps

module testbench;
    parameter N = 3; 
    parameter Width = 2**N;

    logic [Width-1:0] a;
    logic [N-1:0] amt;
    logic [Width-1:0] y_right;
    logic [Width-1:0] y_left;

    param_right_shifter #(N) right_shifter (
        .a(a),
        .amt(amt),
        .y(y_right)
    );

    param_left_shifter #(N) left_shifter (
        .a(a),
        .amt(amt),
        .y(y_left)
    );

    initial begin
        $monitor("Time=%0t | Input=%b | Shift=%d | Right=%b | Left=%b", 
                  $time, a, amt, y_right, y_left);

        a = 8'b10110011; amt = 1; #10;
        a = 8'b10110011; amt = 2; #10;
        a = 8'b10110011; amt = 3; #10;
        a = 8'b10110011; amt = 4; #10;
        a = 8'b11001100; amt = 5; #10;

        $finish;
    end
endmodule
