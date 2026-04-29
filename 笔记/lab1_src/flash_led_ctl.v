`timescale 10ns / 1ns

module flash_led_ctl(
 	 	input clk,
 	 	input rst,
 	 	input dir,
 	 	input clk_bps,
 	 	output reg[19:0]led
 	 	);
 	 	always @( posedge clk or posedge rst )
 	 		if( rst )
 	 			led <= 20'h00001;
 	 		else
 	 			case( dir )
 	 				1'b0:  			 //从左向右
 	 					if( clk_bps )
 	 				 		if( led != 20'd1 )
 	 							led <= led >> 1'b1;
 	 						else
 	 							led <= 20'h80000;
 	 				1'b1:  			 //从右向左
 	 			 		if( clk_bps )
 	 						if( led != 20'h80000 )
 	 							led <= led << 1'b1;
 	 						else
 	 							led <= 20'd1;
 	 			endcase
endmodule

