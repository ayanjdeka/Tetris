/************************************************************************
Avalon-MM Interface VGA Text mode display

Register Map:
0x000-0x0257 : VRAM, 80x30 (2400 byte, 600 word) raster order (first column then row)
0x258        : control register

VRAM Format:
X->
[ 31  30-24][ 23  22-16][ 15  14-8 ][ 7    6-0 ]
[IV3][CODE3][IV2][CODE2][IV1][CODE1][IV0][CODE0]

IVn = Draw inverse glyph
CODEn = Glyph code from IBM codepage 437

Control Register Format:
[[31-25][24-21][20-17][16-13][ 12-9][ 8-5 ][ 4-1 ][   0    ] 
[[RSVD ][FGD_R][FGD_G][FGD_B][BKG_R][BKG_G][BKG_B][RESERVED]

VSYNC signal = bit which flips on every Vsync (time for new frame), used to synchronize software
BKG_R/G/B = Background color, flipped with foreground when IVn bit is set
FGD_R/G/B = Foreground color, flipped with background when Inv bit is set

************************************************************************/

`define NUM_REGS 601 //80*30 characters / 4 characters per register
`define CTRL_REG 600 //index of control register

module vga_text_avl_interface (
	// Avalon Clock Input, note this clock is also used for VGA, so this must be 50Mhz
	// We can put a clock divider here in the future to make this IP more generalizable
	input logic CLK,
	
	// Avalon Reset Input
	input logic RESET,
	
	// Avalon-MM Slave Signals
	input  logic AVL_READ,					// Avalon-MM Read
	input  logic AVL_WRITE,					// Avalon-MM Write
	input  logic AVL_CS,					// Avalon-MM Chip Select
	input  logic [3:0] AVL_BYTE_EN,			// Avalon-MM Byte Enable
	input  logic [11:0] AVL_ADDR,			// Avalon-MM Address
	input  logic [31:0] AVL_WRITEDATA,		// Avalon-MM Write Data
	output logic [31:0] AVL_READDATA,		// Avalon-MM Read Data
	
	// Exported Conduit (mapped to VGA port - make sure you export in Platform Designer)
	output logic [3:0]  red, green, blue,	// VGA color channels (mapped to output pins in top-level)
	output logic hs, vs						// VGA HS/VS
);

logic [31:0] LOCAL_REG       [`NUM_REGS]; // Registers
//put other local variables here
logic pix_clk, blank, sync, pixel, inv;
logic [7:0] data, fontCode, fontColor;
logic [9:0] DrawX, DrawY, del_offset, del_xoffset, del_yoffset, vramIndex;
logic [9:0] xChar; 
logic [9:0] yChar;
logic [10:0] fontAddr;
logic [2:0] xOffset;
logic [3:0] yOffset;
logic [11:0] charIndex;
logic vramOffset;
logic [3:0] frontR, frontG, frontB, backR, backG, backB;
logic [31:0] Palette [8];
logic [11:0] vram_addr;
logic [31:0] readout_buff, buff_readout;
logic memWrite, memRead;
assign memWrite = AVL_WRITE & ~AVL_ADDR[11] & AVL_CS;
assign memRead = AVL_READ & ~AVL_ADDR[11] & AVL_CS;

OCM onChipMemory(.address_a(vram_addr), 
					.address_b(AVL_ADDR), 
					.byteena_b(AVL_BYTE_EN), 
					.clock(CLK), 
					.data_a(), 
					.data_b(AVL_WRITEDATA),  
					.wren_a(0), 
					.wren_b(memWrite), 
					.q_a(readout_buff), 
					.q_b());

always_ff @ (posedge CLK) begin
	if(RESET)
		Palette <= '{default:32'b0};
	else if(AVL_CS)
		begin
			if(AVL_READ & AVL_ADDR[11])
				buff_readout <= Palette[AVL_ADDR[2:0]];
			else if(AVL_WRITE & AVL_ADDR[11])
				Palette[AVL_ADDR[2:0]] <= AVL_WRITEDATA;
		end
end

always_comb begin
	if(memRead)
		AVL_READDATA = buff_readout;
	else
		AVL_READDATA = 32'hX;
end

//Declare submodules..e.g. VGA controller, ROMS, etc
vga_controller controllerOfVGA(.Clk(CLK), .Reset(RESET), .hs(hs), .vs(vs), .pixel_clk(pix_clk), .blank(blank), .sync(sync), .DrawX(DrawX), .DrawY(DrawY));
font_rom fonts(.addr(fontAddr), .data(data));

/*
always_ff @ (posedge CLK) begin
	if (RESET)
		LOCAL_REG <= '{default:32'b0};

	else if (AVL_CS) begin
		if (AVL_READ) AVL_READDATA <= LOCAL_REG[AVL_ADDR];
		
		else if (AVL_WRITE) begin
			unique case (AVL_BYTE_EN)
				4'b1111	:	LOCAL_REG[AVL_ADDR] <= AVL_WRITEDATA;
				4'b1100	:	LOCAL_REG[AVL_ADDR][31:16] <= AVL_WRITEDATA[31:16];
				4'b0011	:	LOCAL_REG[AVL_ADDR][15:0] <= AVL_WRITEDATA[15:0];
				4'b1000	:	LOCAL_REG[AVL_ADDR][31:24] <= AVL_WRITEDATA[31:24];
				4'b0100	:	LOCAL_REG[AVL_ADDR][23:16] <= AVL_WRITEDATA[23:16];
				4'b0010	:	LOCAL_REG[AVL_ADDR][15:8] <= AVL_WRITEDATA[15:8];
				4'b0001	:	LOCAL_REG[AVL_ADDR][7:0] <= AVL_WRITEDATA[7:0];
				default	: ;
			endcase 
		end
	end
end


//handle drawing (may either be combinational or sequential - or both).
always_comb begin
	xChar = DrawX >> 3;
	yChar = DrawY >> 4;
	charIndex = yChar * 80 + xChar;
	vramIndex = charIndex >> 2;
	unique case (charIndex[1:0])
		2'b00:
			fontCode = readout_buff[7:0];//LOCAL_REG[vramIndex][7:0];
		2'b01:
			fontCode = readout_buff[15:8];//LOCAL_REG[vramIndex][15:8];
		2'b10:
			fontCode = readout_buff[23:16];//LOCAL_REG[vramIndex][23:16];
		2'b11:
			fontCode = readout_buff[31:24];//LOCAL_REG[vramIndex][31:24];
	endcase
	fontAddr = fontCode[6:0] * 16 + DrawY[3:0];
	pixel = data[3'b111 - DrawX[2:0]];
	inv = fontCode[7];
end

always_ff @ (posedge pix_clk) begin
	if(~blank)
		begin
			blue <= 4'h0;
			green <= 4'h0;
			red <= 4'h0;
		end
	else
	begin
		if(pixel ^ inv)
			begin
				blue <= 4'hf;//LOCAL_REG[600][16:13];
				green <= 4'h0;//LOCAL_REG[600][20:17];
				red <=4'h0;//LOCAL_REG[600][24:21];
			end
		else
			begin
				blue <= 4'h0;//LOCAL_REG[600][4:1];
				green <= 4'h0;//LOCAL_REG[600][8:5];
				red <= 4'hf;//LOCAL_REG[600][12:9];
			end
	end
end
*/



//always_ff @ (posedge pix_clk) begin
//	vram_addr <= charIndex >> 1;
//	del_offset <= charIndex[0];
//	vramOffset <= del_offset;
//	del_xoffset <= DrawX[2:0];
//	del_yoffset <= DrawY[3:0];
//	xOffset <= del_xoffset;
//	yOffset <= del_yoffset;
//end

always_comb begin
	xChar = DrawX >> 3;
	yChar = DrawY >> 4;
	charIndex = yChar * 80 + xChar;
	vram_addr = charIndex/2;
	vramOffset = charIndex[0];
	xOffset = DrawX[2:0];
	yOffset = DrawY[3:0];
end

always_comb begin
	if(~vramOffset)
		begin
			fontColor = readout_buff[7:0];
			fontCode = readout_buff[15:8];
		end
	else
		begin
			fontColor = readout_buff[23:16];
			fontCode = readout_buff[31:24];
		end
	fontAddr = fontCode[6:0] * 16 + yOffset;
	pixel = data[3'b111 - xOffset];
	inv = fontCode[7];
	if(fontColor[4])
		begin
			frontR = Palette[fontColor[7:5]][24:21];
			frontG = Palette[fontColor[7:5]][20:17];
			frontB = Palette[fontColor[7:5]][16:13];
		end
	else
		begin
			frontR = Palette[fontColor[7:5]][12:9];
			frontG = Palette[fontColor[7:5]][8:5];
			frontB = Palette[fontColor[7:5]][4:1];
		end
	if(fontColor[0])
		begin
			backR = Palette[fontColor[3:1]][24:21];
			backG = Palette[fontColor[3:1]][20:17];
			backB = Palette[fontColor[3:1]][16:13];
		end
	else
		begin
			backR = Palette[fontColor[3:1]][12:9];
			backG = Palette[fontColor[3:1]][8:5];
			backB = Palette[fontColor[3:1]][4:1];
		end
end

always_ff @ (posedge pix_clk) begin
	if(~blank)
		begin
			blue <= 4'h0;
			green <= 4'h0;
			red <= 4'h0;
		end
	else if(DrawX <= 1)
		begin
			blue = 4'h0;
			green = 4'h0;
			red = 4'h0;
		end
	else
		begin
			if(pixel ^ inv)
				begin
					blue <= frontB;
					green <= frontG;
					red <= frontR;
				end
			else
				begin
					blue <= backB;
					green <= backG;
					red <= backR;
				end
		end
end

endmodule
