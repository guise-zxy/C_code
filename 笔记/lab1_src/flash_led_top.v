`timescale 10ns / 1ns


module flash_led_top(
        input sysclk_p,
        input sysclk_n,
 	 	input rst_n,
 	 	input sw0,
 	 	output [19:0]led
 	 	);

 	 	wire rst;
 	 	assign rst = ~rst_n;
 	 	
 	 	
              wire clk;
clk_wiz_0 instance_name
                 (
                  // Clock out ports
                  .clk_out1(clk),     // output clk_out1
                  // Status and control signals
                  .reset(rst), // input reset
                  .locked(locked),       // output locked
                 // Clock in ports
                  .clk_in1_p(sysclk_p),    // input clk_in1_p
                  .clk_in1_n(sysclk_n));    // input clk_in1_n

 	 	wire clk_bps; 	 	
 	 	counter counter(
 	 		.clk( clk ),
 	 		.rst( rst ),
 	 		.clk_bps( clk_bps )
 	 	);
 	 	flash_led_ctl flash_led_ctl(
 	 		.clk( clk ),
 	 		.rst( rst ),
 	 		.dir( sw0 ),
 	 		.clk_bps( clk_bps ),
 	 		.led( led )
 	 	);
endmodule
