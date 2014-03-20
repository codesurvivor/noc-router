module crossbar(out, in, s);

parameter width = 2; // number of bits for each port
localparam n_in_ports = 5;
localparam n_out_ports = 5;

// port declaration
output [width*n_out_ports-1:0] out;
input [width*n_in_ports-1:0] in;
input [5:0] s;

reg[width*n_out_ports-1:0] out;


always @(in or s)
begin

case (s)
  0: out[1*width-1:0] = in[1*width-1:0];
  1: out[1*width-1:0] = in[2*width-1:1*width];
  2: out[1*width-1:0] = in[3*width-1:2*width];
  3: out[1*width-1:0] = in[4*width-1:3*width];
  4: out[1*width-1:0] = in[5*width-1:4*width];
  
  5: out[2*width-1:1*width] = in[1*width-1:0];
  6: out[2*width-1:1*width] = in[2*width-1:1*width];
  7: out[2*width-1:1*width] = in[3*width-1:2*width];
  8: out[2*width-1:1*width] = in[4*width-1:3*width];
  9: out[2*width-1:1*width] = in[5*width-1:4*width];
  
  10: out[3*width-1:2*width] = in[1*width-0];
  11: out[3*width-1:2*width] = in[2*width-1:1*width];
  12: out[3*width-1:2*width] = in[3*width-1:2*width];
  13: out[3*width-1:2*width] = in[4*width-1:3*width];
  14: out[3*width-1:2*width] = in[5*width-1:4*width];
  
  15: out[4*width-1:3*width] = in[1*width-1:0];
  16: out[4*width-1:3*width] = in[2*width-1:1*width];
  17: out[4*width-1:3*width] = in[3*width-1:2*width];
  18: out[4*width-1:3*width] = in[4*width-1:3*width];
  19: out[4*width-1:3*width] = in[5*width-1:4*width];
  
  20: out[5*width-1:4*width] = in[1*width-1:0];
  21: out[5*width-1:4*width] = in[2*width-1:1*width];
  22: out[5*width-1:4*width] = in[3*width-1:2*width];
  23: out[5*width-1:4*width] = in[4*width-1:3*width];
  24: out[5*width-1:4*width] = in[5*width-1:4*width];
  default: out = {width{1'bx}};
    endcase
    end
    
endmodule
