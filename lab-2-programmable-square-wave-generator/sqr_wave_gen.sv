//Top Module
module sqr_wave_gen(
    input logic clk,
    input logic reset,
    input logic [3:0] m,
    input logic [3:0] n,
    output logic q
);
    logic done_m, done_n;

    mod_counter high_count (.clk(clk), .enable(q), .x(m), .done(done_m));
    mod_counter low_count (.clk(clk), .enable(~q), .x(n), .done(done_n));
    t_flipflop tff (.clk(clk), .reset(reset), .t(done_m | done_n), .q(q));
endmodule
