module mod_counter(
    input logic clk,
    input logic enable,
    input logic [3:0] x,
    output logic done
);
    logic [7:0] count;
    logic [7:0] max_count;
    
    assign max_count = x * 10;

    always_ff @(posedge clk) 
        if (!enable)
            count <= 0;
        else if (count < max_count - 1)  
            count <= count + 1;
        else
            count <= 0;
    
    
//  always_ff @(posedge clk) 
//        if (!enable)
//            done <= 0;
//        else if (count == max_count)
//            done <= 1;
//        else
//            done <= 0;

    assign done = count == (max_count - 1);
    
endmodule
