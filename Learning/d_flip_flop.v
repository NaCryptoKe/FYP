module d_flip_flop (
    input wire clk,
    input wire d,
    output reg q      // 'reg' means this signal can hold a state
);

    // This block triggers ONLY when the clock goes from 0 to 1
    always @(posedge clk) begin
        q <= d;       // Non-blocking assignment: "Transfer d to q"
    end

endmodule

//In Verilog, <= is used inside always blocks to represent hardware shifting data simultaneously.