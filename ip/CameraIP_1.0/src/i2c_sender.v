`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
//
// Module Name: i2c_sender
// 
//////////////////////////////////////////////////////////////////////////////////

module i2c_sender(
    input clk,              // 50 MHz
    input reset_n,
    inout siod,
    output reg sioc,
    output reg taken,       // thank you sir, may I have another
    input send,             // yes you may
    input [7:0] id,         // camera address
    input [7:0] register,   // config register address
    input [7:0] value       // config value to write
    );
    
    reg [7:0] divider;      // transaction counter
    reg [31:0] busy_sr;     // busy signal
    reg [31:0] data_sr;     // transaction data
   
    // data IO signal
    // inout port for camera that signals when camera is ready for data 
    assign siod = ((busy_sr[11:10]==2'b10)||(busy_sr[20:19]==2'b10)||(busy_sr[29:28]==2'b10)) ? 1'bz : data_sr[31];
    
    // i2c setup for writing values to the camera registers
    always @(posedge clk or negedge reset_n) begin
        if(~reset_n) begin
            divider <= 8'b00000001;                 // 254 cycle pause 
            busy_sr <= 0;
            data_sr <= 32'hffffffff;
        end else begin
            taken <= 0;
            if(busy_sr[31] == 1'b0) begin           // ready to start transaction when siod goes low
                sioc <= 1'b1;
                if(send == 1'b1) begin
                    if(divider == 8'h00) begin      // setup data for transfer
                        data_sr <= {3'b100,id,1'b0,register,1'b0,value,1'b0,2'b01};
                        busy_sr <= {32'b11111111_11111111_11111111_11111111};
                        taken <= 1'b1;
                    end else
                        divider <= divider + 1'b1;
                end
            end else begin
                // serial clock controls data transactions
                // transactions start when sioc goes from high to low after siod goes low
                // transactions stop when sioc and siod go high
                case({busy_sr[31:29], busy_sr[2:0]})
                    6'b111111:                      // sequence 1 -begin (sioc high)
                        case(divider[7:6])
                            2'b00: sioc <= 1'b1;
                            2'b01: sioc <= 1'b1;
                            2'b10: sioc <= 1'b1;
                            default: sioc <= 1'b1;
                        endcase
                    6'b111110:                      // sequence 2 -begin (sioc high)
                        case(divider[7:6])
                            2'b00: sioc <= 1'b1;
                            2'b01: sioc <= 1'b1;
                            2'b10: sioc <= 1'b1;
                            default: sioc <= 1'b1;
                        endcase
                    6'b111100:                      // sequence 3 -begin (sioc low)
                        case(divider[7:6])
                            2'b00: sioc <= 1'b0;
                            2'b01: sioc <= 1'b0;
                            2'b10: sioc <= 1'b0;
                            default: sioc <= 1'b0;
                        endcase
                    6'b110000:                      // sequence 1 -end (sioc low->high)
                        case(divider[7:6])
                            2'b00: sioc <= 1'b0;
                            2'b01: sioc <= 1'b1;
                            2'b10: sioc <= 1'b1;
                            default: sioc <= 1'b1;
                        endcase
                    6'b100000:                      // sequence 2 -end (sioc high)
                        case(divider[7:6])
                            2'b00: sioc <= 1'b1;
                            2'b01: sioc <= 1'b1;
                            2'b10: sioc <= 1'b1;
                            default: sioc <= 1'b1;
                        endcase
                    6'b000000:                      // idle state (sioc high)
                        case(divider[7:6])
                            2'b00: sioc <= 1'b1;
                            2'b01: sioc <= 1'b1;
                            2'b10: sioc <= 1'b1;
                            default: sioc <= 1'b1;
                        endcase
                    default:                     
                        case(divider[7:6])
                            2'b00: sioc <= 1'b0;
                            2'b01: sioc <= 1'b1;
                            2'b10: sioc <= 1'b1;
                            default: sioc <= 1'b0;
                        endcase
                endcase
                
                // system is busy
                if(divider == 8'hff) begin
                    busy_sr <= {busy_sr[30:0],1'b0};
                    data_sr <= {data_sr[30:0],1'b1};
                    divider <= 0;
                end else
                    divider <= divider + 1'b1;
            end
        end
    end
    
endmodule
