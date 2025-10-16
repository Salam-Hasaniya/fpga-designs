module bar_demo 
   (
    input  logic [10:0] x, y,     // treated as x-/y-axis
    input  logic [11:0] rgb_value,
    input  logic [1:0]  sq_control,
    output logic [11:0] bar_rgb 
   );

   // extract square color components
   logic [3:0] r_sq, g_sq, b_sq;
   assign r_sq = rgb_value[11:8];
   assign g_sq = rgb_value[7:4];
   assign b_sq = rgb_value[3:0];

   // complement of square color
   logic [3:0] cr, cg, cb;
   assign cr = 4'b1111 - r_sq;
   assign cg = 4'b1111 - g_sq;
   assign cb = 4'b1111 - b_sq;

   // determine square size based on control
   logic [7:0] square_size;
   always_comb begin
      case (sq_control)
         2'b00: square_size = 8'd16;
         2'b01: square_size = 8'd32;
         2'b10: square_size = 8'd64;
         2'b11: square_size = 8'd128;
      endcase
   end

   // determine if pixel is in square region
   logic [10:0] x_center = 320;
   logic [10:0] y_center = 240;
   logic [10:0] half_size;
   logic        in_square;

   assign half_size = square_size >> 1;
   assign in_square = (x >= (x_center - half_size)) && (x < (x_center + half_size)) &&
                      (y >= (y_center - half_size)) && (y < (y_center + half_size));

   // final output
   always_comb begin
      if (in_square)
         bar_rgb = rgb_value;         // square color from switches
      else
         bar_rgb = {cr, cg, cb};      // background = complement
   end

endmodule
