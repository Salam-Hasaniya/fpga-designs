`timescale 1ns / 1ps

module ROM_temp_conversion(
    input clk,  
    input [7:0] temp_input,    
    input format,          
    output logic [6:0] seg,     
    output logic [7:0] AN 
    );
    
    //wires
    logic [7:0] C2F_temp, F2C_temp;
    logic slow_clk;
    logic [2:0] display_index = 0;
    logic [3:0] in_hundreds, in_tens, in_ones;
    logic [3:0] out_hundreds, out_tens, out_ones;
    
    //clock
    clock_divider #(50_000) (.clk_in(clk), .clk_out(slow_clk));
    
    
    C2F_ROM rom_inst (.clk(clk), .addr(temp_input), .data(C2F_temp));
    F2C_ROM rom2_inst (.clk(clk), .addr(temp_input), .data(F2C_temp));
    
    bin_to_bcd bcd_orig (.binary(temp_input), .ones(in_ones), .tens(in_tens), .hundreds(in_hundreds));
    bin_to_bcd bcd_conv (.binary(format? F2C_temp:C2F_temp ), .ones(out_ones), .tens(out_tens), .hundreds(out_hundreds));
    
    seven_seg_display display_inst (.slow_clk(slow_clk), .format(format), .in_hundreds(in_hundreds), .in_tens(in_tens), .in_ones(in_ones),
    .out_hundreds(out_hundreds), .out_tens(out_tens), .out_ones(out_ones), .AN(AN), .seg(seg));

    
endmodule

    
   

