module t_flipflop(
    input  logic clk,
    input  logic reset,
    input  logic t,
    output logic q
);
    always_ff @(posedge clk) begin
        if (reset)
            q <= 0;
        else if (t)
            q <= ~q;
    end
endmodule
