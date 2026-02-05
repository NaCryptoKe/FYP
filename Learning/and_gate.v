module and_gate (
    input wire a,    // Input pin 1
    input wire b,    // Input pin 2
    output wire y    // Output pin
);

    // This describes the continuous connection of the wires
    assign y = a & b;

endmodule

// wire indicates the physical connection or the copper wire
// assign continously updates within milliseconds to change y