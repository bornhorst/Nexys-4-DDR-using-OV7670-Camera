`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
//
// Module Name: OV7670_controller
// 
//////////////////////////////////////////////////////////////////////////////////

module OV7670_controller(
    input clk,                  // 50 MHz
    input reset_n,
    input [15:0] command,       // i2c command
    output config_finished,     // output that config is complete
    output sioc,
    inout siod,
    output cam_reset,
    output pwdn,
    output xclk,
    output taken                // i2c is ready for new command
    );
   
    // address used to configure camera
    parameter camera_address = 8'h42;
    
    // used to create the XCLK for the camera
    reg sys_clk;
    
    // configuration is complete when command == 0xffff
    wire finished;
    
    // signal telling i2c to keep writing
    wire send;
    
    // configuration is complete
    assign finished = (command == 16'hffff) ? 1 : 0;
    assign config_finished = finished;
    
    // send until configuration is finished
    assign send = ~finished;
    
    // i2c sender used for writing values to camera config registers
    i2c_sender sender (
        .clk(clk),
        .reset_n(reset_n),
        .taken(taken),
        .siod(siod),
        .sioc(sioc),
        .send(send),
        .id(camera_address),        // camera address
        .register(command[15:8]),   // camera config address
        .value(command[7:0])        // value to write
    );
    
    // camera reset
    assign cam_reset = 1'b1;
    
    // camera powerdown
    assign pwdn = 1'b0;
    
    // camera xclk
    assign xclk = sys_clk;
    
    // synchronize the XCLK to the system    
    always @(posedge clk) 
        sys_clk <= ~sys_clk;
    
endmodule
