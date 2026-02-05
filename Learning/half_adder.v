module half_adder (
    input wire a,
    input wire b,
    output wire sum,
    output wire carry
);

    assign sum = a ^ b;    // XOR gate (Addition)
    assign carry = a & b;  // AND gate (The overflow)

endmodule