`timescale 1ns / 1ps

// n4fpga.v - Top level module 
//
// Created By:	Ryan Bornhorst
//
// Description:
// ------------
// This module provides the top level for the hardware.
// The module is designed to connect to a OV7670 Camera
// and stream video through the VGA port using RGB565.
//////////////////////////////////////////////////////////////////////
module n4fpga(
    input				clk,			               // 100Mhz clock input
	input				btnCpuReset,	               // CPU reset pushbutton
    /* OV7670 Camera Ports */
    input               OV7670_VSYNC,              
    input               OV7670_HREF,
    input               OV7670_PCLK,
    output              OV7670_SIOC,
    inout               OV7670_SIOD,
    output              OV7670_XCLK,
    output              OV7670_RESET,
    output              OV7670_PWDN,
    input   [7:0]       OV7670_DATA,
    /* buttons, sitches, leds */  
    input               btnU,btnD,btnR,btnL,btnC,      // buttons
    input  [15:0]       sw,                            // switches
    output  [15:0]      led,                           // LEDs
    /* VGA ports */
    output  [3:0]       vga_red, vga_green, vga_blue,   
    output              vga_hsync, vga_vsync,
    /* UART signals */
    input               uart_rtl_rxd,
    output              uart_rtl_txd
);

// internal variables
// Clock and Reset 
wire				sysclk;             // 
wire				sysreset_n;

// make the connections
// system-wide signals
assign sysclk = clk;
assign sysreset_n = btnCpuReset;      

// embedded system for the block design
embsys embsys
        /* Camera Ports */
       (.OV7670_DATA_0(OV7670_DATA),
        .OV7670_HREF_0(OV7670_HREF),
        .OV7670_PCLK_0(OV7670_PCLK),
        .OV7670_PWDN_0(OV7670_PWDN),
        .OV7670_RESET_0(OV7670_RESET),
        .OV7670_SIOC_0(OV7670_SIOC),
        .OV7670_SIOD_0(OV7670_SIOD),
        .OV7670_VSYNC_0(OV7670_VSYNC),
        .OV7670_XCLK_0(OV7670_XCLK),
        /* Board Buttons */
        .btnC_0(btnC),
        .btnD_0(btnD),
        .btnL_0(btnL),
        .btnR_0(btnR),
        .btnU_0(btnU),
        /* RAM Port A Clock */
        .clka_0(OV7670_PCLK),
        /* LEDs and Switches */
        .led_0(),                       // not currently used
        .led_1(led[0]),                 // led[0] is used to show that configuration completed
        .sw_0(sw),
        /* System Clock and Reset */
        .sysclk(sysclk),
        .sysreset_n(sysreset_n),
        /* UART through USB Port */
        .uart_rtl_0_rxd(uart_rtl_rxd),
        .uart_rtl_0_txd(uart_rtl_txd),
        /* VGA Port Signals */
        .vga_blue_0(vga_blue),
        .vga_green_0(vga_green),
        .vga_hsync_0(vga_hsync),
        .vga_red_0(vga_red),
        .vga_vsync_0(vga_vsync));

endmodule

