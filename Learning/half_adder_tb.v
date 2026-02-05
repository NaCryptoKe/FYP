// 1. Declare the testbench (no inputs or outputs)
module half_adder_tb;

    // 2. Create "virtual wires" to connect to our adder
    reg a;    // 'reg' because we will manually change these values
    reg b;
    wire sum; // 'wire' because we are just observing these
    wire carry;

    // 3. Instantiate the "Unit Under Test" (UUT)
    // This is like plugging the chip into our breadboard
    half_adder uut (
        .a(a),
        .b(b),
        .sum(sum),
        .carry(carry)
    );

    // 4. The "Script" of our test
    initial begin
        $dumpfile("wave.vcd");
        $dumpvars(0, half_adder_tb);

        // At 0 nanoseconds
        a = 0; b = 0;
        #10; // Wait 10 time units

        // At 10 nanoseconds
        a = 0; b = 1;
        #10;

        // At 20 nanoseconds
        a = 1; b = 0;
        #10;

        // At 30 nanoseconds
        a = 1; b = 1;
        #10;

        $finish; // Stop the simulation
    end

endmodule