module tetris_rom (
	input logic clock,
	input logic [18:0] address,
	output logic [3:0] q
);

logic [3:0] memory [0:287999] /* synthesis ram_init_file = "./tetris/tetris.mif" */;

always_ff @ (posedge clock) begin
	q <= memory[address];
end

endmodule
