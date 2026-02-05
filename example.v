module example(
    input clk,
    input a,
    input b, 
    output c
);

    wire w;     // wire declaration
    // Wire represents continous value, the default value of the net is Z which is high
    reg r;      // reg declaration
    // reg represents a value stored over time, used in procedural blocks. the default value of the net is X which is low
    assign w = a & b;   // wire continous assignment

    always @ (posedge clk) begin
        r = a;  // reg procedure assignment
    end

    assign c = r ^ w;
end module

/*
Operators in verilog

Logical Operators
-----------------
&   bitwise AND
|   bitwise OR
^   bitwise XOR
~   bitwise NOT
^~  bitwise XNOR

&&  boolean AND
||  boolean OR
!   boolean NOT

Arithmetic Operation
--------------------
+   addition
-   subtraction

Reduction Operators
-------------------
&   reduce via AND
~&  reduce via NAND
|   reduce via OR
~|  reduce via NOR
^   reduce via XOR
^~  reduce via XNOR

Shift Operators
---------------
>>  shift right
<<  shift left
>>> arithmetic shift right

Relational Operators
--------------------
==  equal
!=  not equal
>   greather than
>=  greater than equals to
<   less than
<=  less than or equals to

Other Operators
---------------
{}      concatenate
{N{}}   replicate N times
*/

/*
A simple adder 

module adder (input [3:0] A, input [3:0]B, output [3:0] Sum, output Cout);
assign {Cout, Sum} = A + B;
endmodule;
*/

/*
Assignment in verilog
There are two types:
    continous and procedural assignment
Continouts is: outside procedural blocks, drives values on to nets, automatically active at time zero, and automatically assigns value when the right hand changes.
Procedural are: inside procedural blocks, updates register and memory data types, evaluated when the statement is encountered, assignment in procedural blocks using initial or always.
*/