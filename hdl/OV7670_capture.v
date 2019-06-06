`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
//
// Module Name: OV7670_capture
// 
//////////////////////////////////////////////////////////////////////////////////


module OV7670_capture(
    input pclk,                 // pixel output clock
    input reset_n,              // reset
    input vsync,                // vertical sync
    input href,                 // horizontal ref
    input [7:0] d,              // camera data
    output [18:0] addr,         // capture address
    output reg [11:0] dout,     // capture data
    output reg we               // write enable
    );
    
    reg [15:0] d_latch;         // data latch
    reg [18:0] address;         // capture address
    reg [18:0] address_next;    // next capture address
    reg [1:0]  wr_hold;         // signal for incrementing address or holding current address
    
    assign addr = address;
    
    /* Data transfer using HREF */
    // href      write enable        data               address      address next
    //              
    // 1              x             xxxxxxxxxxxx        xxxxxxxx        address
    // 0              x             xxxxxxxxxxxx        address         address
    // x              1             rrrrggggbbbb        address         address + 1
    
    always @(posedge pclk or negedge reset_n) begin
        if(~reset_n) begin
            d_latch <= 0;
            address <= 0;
            address_next <= 0;
            wr_hold <= 0;
        end else if(vsync == 1'b1) begin    // reset address after vsync
            address <= 0;
            address_next <= 0;
            wr_hold <= 0;
        end else begin                      // send data to RAM while vsync is low
            dout <= {d_latch[15:12], d_latch[10:7], d_latch[4:1]};
            address <= address_next;
            we <= wr_hold[1];
            wr_hold <= {wr_hold[0],(href & ~wr_hold[0])};
            d_latch <= {d_latch[7:0],d};
        
            // increment the address
            if(wr_hold[1] == 1'b1)
                address_next <= address_next + 1'b1;
        end
    end
        
    
endmodule
