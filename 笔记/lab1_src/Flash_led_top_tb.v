`timescale 1ns / 1ns


module flash_led_top_tb;
 	 	reg sysclk_p,sysclk_n,rst,sw0;
 	 	wire [19:0] led;
 	  	initial begin
 	 		sysclk_p = 1'b0;
 	 		sysclk_n = 1'b1;
 	 		rst = 1'b1;
 	 		sw0 = 1'b0;
 	 		#10 rst = 1'b0;
 	 		#10 rst = 1'b1;
 	 		#1000000				 //3s后改变位移方向
 	 		#1000000
 	 		#1000000
 	 		sw0 = 1'b1;
 	 	end
 	 	always #2.5 sysclk_p <= ~sysclk_p;        //200MHz时钟
 	 	always #2.5 sysclk_n <= ~sysclk_n; 
 	 	
 	 	flash_led_top flash_led_top(
 	 		.sysclk_p( sysclk_p ),
 	 		.sysclk_n( sysclk_n ),
 	 		.rst_n( rst ),
 	 		.sw0( sw0 ),
 	 		.led( led )
 	 	);
endmodule
