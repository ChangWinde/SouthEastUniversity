;*******************************************************************************
; 
;   5xx_ACTIVE_test.asm
;  
;   Redistribution and use in source and binary forms, with or without 
;   modification, are permitted provided that the following conditions 
;   are met:
; 
;     Redistributions of source code must retain the above copyright 
;     notice, this list of conditions and the following disclaimer.
; 
;     Redistributions in binary form must reproduce the above copyright
;     notice, this list of conditions and the following disclaimer in the 
;     documentation and/or other materials provided with the   
;     distribution.
; 
;     Neither the name of Texas Instruments Incorporated nor the names of
;     its contributors may be used to endorse or promote products derived
;     from this software without specific prior written permission.
; 
;   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 
;   "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT 
;   LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
;   A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT 
;   OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, 
;   SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT 
;   LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
;   DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
;   THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
;   (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
;   OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
; 
;*******************************************************************************

;*******************************************************************************
; Note that : 
; -It mixes with types of instructions: type I,II, JMP, Emulations 
; -It mixes with address modes
; -Here are some numbers:
;   (A) Memory Access Point of View
;       # of fetch from Flash: 11+4*8 = 43        -64%
;       # of Read from RAM:    12                 -18%
;       # of Write to RAM:     12                 -18%
;   (B) Type of Instruction Point of View
;       # of instructions:     6+8*3 = 30
;         ALU inst:            2+2*8 = 18         -60%
;         Data Read inst:      4+1*8 = 12         -40%
;         Data Write inst:                        -40%
;         Control/Jump :       8                  -26%
; -The loop in the example contains successive read from memory, then data
;  process, and after that write back to a single address. 
;*******************************************************************************

						.global	ACTIVE_MODE_TEST                        

						.bss	TestRAM,0x22          	 ; Reserve 0x22 bytes for active mode test
                        
						.text     
                                                         ;  Format | Fetch  | ramR   |ramW   |           
ACTIVE_MODE_TEST        NOP
                        MOV     #TestRAM, R12            ;      1  | 2      | 0      | 0     |       
                        MOV     #0x4,0(R12)              ;      1  | 2      | 0      | 1     | 
                        MOV     &TestRAM,&TestRAM+0x02   ;      1  | 3      | 1      | 1     | 
                        ADD     @R12,2(R12)              ;      1  | 2      | 1      | 1     |
                        SWPB    @R12+                    ;      2  | 1      | 1      | 1     |
                        MOV     @R12,R13                 ;      1  | 1      | 1      | 0     |          
IDD_AM_L1               ; run 8 times
                        XOR     @R12+,&TestRAM+0x20      ;      1  | 2      | 1      | 1     |
                        DEC     R13                      ;     E1  | 1      | 0      | 0     | 
                        JNZ     IDD_AM_L1                ;     JMP | 1      | 0      | 0     |

						RETA
						.end
