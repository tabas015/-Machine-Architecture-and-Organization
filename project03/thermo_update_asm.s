

.text
.global  set_temp_from_ports
        
### args are
        ## %dx = THERMO_SENSOR_PORT
        ## %rsi = temp_t *temp 
        ## %cl = THERMO_STATUS_PORT
        ## %r8d = THERMO_DISPLAY_PORT

set_temp_from_ports:
    movw  THERMO_SENSOR_PORT(%rip), %dx   # copy global var to reg dx  (16-bit word)
    movb  THERMO_STATUS_PORT(%rip), %cl   # copy global var to reg cl  (8-bit byte)
    
.START:    
    cmpw $64000, %dx                     ## comparing  64000 to the tempt val of global variable
    ja .ret_1                            #return 1 
.MAIN: 
    movw  %dx,%ax                       # %ax has the %dx=THERMO_SENSOR_PORT value
    shrw $6, %ax                        #shifting right by 6 which is dividing the THERMO_SENSOR_PORT by 64
    andw $0b111111,%dx                  # bitmaasking
    subw $500, %ax                      # subtracting 500 from ax 
    cmpw $31, %dx                       # checking the remainder if the remainder,dx >= 32
    ja .ROUND                           #jumping to round for rounding the remainder
                        			
.CONTINUE:
    andb $0x1,%cl                       #logical anding the THERMO_STATUS_PORT and $0x1
    cmpb $1, %cl                        #comparing the THERMO_STATUS_PORT and $1
    jne .CEL_FER                        #if not equal then jump to .CEL_FER which will convert Farenheit to 0 and celsius to ax
    
    movb $1,2(%rdi)                     #converting Farenheit to 1 
    imull $9, %eax                      
    cwtl						        # so first, sign-extend %ax to %eax
    cltq						        # then sign-extend %eax to %rax
    cqto                                # then sign-extend %rax to %rdx
    movl $5, %r9d
    idivl %r9d
    addl $320, %eax 
    movw %ax, 0(%rdi) 
    jmp  .ret_0                     
.CEL_FER:
    movb $0, 2(%rdi)                    #converting Farenheit to 0 
    movw %ax,0(%rdi)
    jmp  .ret_0    
                                                      
.ret_1:
    movl $1, %eax                        # return 1
    ret
    jmp .MAIN
.ROUND:                                 # rouding the remainder 
    addl $1,%eax 
    movl %eax,0(%rdi) 
    jmp .CONTINUE 
.ret_0:                                 # return 0
    movl $0,%eax
    ret   



### Data area associated with the next function
.data
	
my_int:                         # declare location an single int
        .int 0                  # value 1234

digit_masks:  
        .int 0b1111110             # array[0]
        .int 0b0001100             # array[1] 
        .int 0b0110111             # array[2]                    
        .int 0b0011111             # array[3]  
        .int 0b1001101             # array[4] 
        .int 0b1011011             # array[5] 
        .int 0b1111011             # array[6] 
        .int 0b0001110             # array[7] 
        .int 0b1111111             # array[8] 
        .int 0b1011111             # array[9] 
far_mask:
        .int 0b100000000000000000000000000000 
cel_mask:
        .int 0b10000000000000000000000000000
.text
.global  set_display_from_temp

## ENTRY POINT FOR REQUIRED FUNCTION
set_display_from_temp:  
//rdi = 1 temp_t rsi = 2   display 
.MAIN_01:
    movl  %edi,%r9d              ## %r9d is temp.tenths_degrees   
    movl %edi, %r8d              ## %r8d is fahrenheit 
    sarl  $16,%r8d               ## moving farenheit to low order bits
    
    cmpb $0, %r8b                ## checking %r8b or fahrenheit == 0
    je .CELSIUS_CHECK_01         ##jump to celcius check 
    cmpb $1,%r8b                 ## checking %r8b orfahrenheit == 1
    je .CELSIUS_CHECK_02         ##jump to celcius check
    jmp .ERROR                   ##jump to

.CELSIUS_CHECK_01:
    cmpw $-500,%r9w             ## checking if temp.tenths_degrees is in bound of -500 to +500
    jl .ERROR                   ##jump to error 
    cmpw $500, %r9w             ## checking if temp.tenths_degrees is in bound of -500 to +500   
    jg .ERROR                   ##jump to error
    jmp .CONTINUE_01
.CELSIUS_CHECK_02:
    cmpw $-580,%r9w              ##checking if temp.tenths_degrees is in bound of -580 to +1220
    jl .ERROR                    ##jump to error
    cmpw $1220, %r9w             ##checking if temp.tenths_degrees is in bound of -580 to +1220
    jg .ERROR                    ##jump to error
    jmp .CONTINUE_01
.ERROR:
    movl $1,%eax                 ##return 1 as error 
    ret 

.CONTINUE_01:
    movl $0,%r10d               ## int disp is %r10d register 
    movl $0,%r11d               ## 0 ing out %r11d as will use it at to mov %r9w which is temp.tenths_degrees
    movw %r9w,%r11w             ##%r9w is temp.tenths_degrees. and %r11w is the register to save the temp.tenths_degrees.
    cmpw $0, %r11w              ## comparing %r11w  to 0
    jl .DISP                    ## if less than then jumping to .DISP 
    jmp .JUMP                   ## otherwise will continue 
.DISP:
    orl $0b0000001,%r10d        ## 0b0000001 = negative
    shll $7,%r10d               ## shit display by 7 
    negw %r11w                  ## negated temp.tenths_degrees

.JUMP:
    pushq %rbx                  ## pushing callee register as I have ran out of registers
    movq %rsi, %rbx             ## moving *display to the callee register and saving it there for future use
    movw %r11w,%ax              ##moving %r11w  to %ax in an aim of division

    
    cwtl                       ## so first, sign-extend %ax to %eax
    cltq                       ## then sign-extend %eax to %rax
    cqto                       # then sign-extend %rax to %rdx
    movl $10, %esi             ## %esi used for division 
    idivl %esi                 ## division
    movl %edx, %r9d            ## %r9d is temp_tenths int temp_tenths = c % 10;
    
    cqto                       ## sign-extend %rax to %rdx
    idivl %esi                 ## division
    movl %edx, %edi            ## %edi is temp_ones int temp_ones = c % 10; 
   
    cqto                       ## sign-extend %rax to %rdx
    idivl %esi                 ## division
    movl %edx, %ecx            ## %ecx is temp_tens int temp_tens = c % 10;  
    
    leaq digit_masks(%rip), %rsi  
    cmpl $0,%eax               #if(temp_hundreds != 0)
    je .DISP_02

.DISP_01:       
    orl (%rsi, %rax, 4), %r10d     ##disp |= arr[temp_hundreds]      
    shll $7, %r10d                 ##disp = disp << 7;
.DISP_02:   
    cmpl $0,%ecx                    ## temp_tens !=0
    je .disp_03 
   
    orl (%rsi, %rcx, 4), %r10d     ## disp |=arr[temp_tens]
    shll $7, %r10d                 ##disp = disp << 7;
.disp_03:     
    orl (%rsi, %rdi, 4), %r10d      ## disp |=arr[temp_ones]
    shll $7, %r10d                  ##disp = disp << 7;     
    orl (%rsi, %r9, 4), %r10d       ## disp |= arr[temp_tenths]
   
    cmpb $1, %r8b                  ##checking if it's (temp.is_fahrenheit)
    je .Farenheit
    movl cel_mask(%rip), %esi       ## if not using celcius mask 
    
    orl %esi,%r10d
    movl %r10d, (%rbx)          ##*display = disp;
    jmp .Ret_0
    
.Farenheit:
    
    movl far_mask(%rip), %esi      ## if not using fahrenheit mask 
    orl %esi,%r10d    
    movl %r10d, (%rbx)             ##*display = disp;


.Ret_0:
    movl $0, %eax
    popq %rbx                    ##popping the callee register
    ret 



.text
.global thermo_update
        
# ENTRY POINT FOR REQUIRED FUNCTION
thermo_update:
    pushq $0
    movq %rsp,%rdi                         # %rsp will move the struct to $rdi
    call set_temp_from_ports               # calling the first function 
    cmpl $0, %eax                          # checking it's returing value is 0 or not
    jne .ret_001    
    
    movq (%rsp),%rdi                       # %rsp will move the struct address to $rdi
    movq %rsp, %rsi                        # %rsp will move the struct to $rdi
    call set_display_from_temp             # calling the second function 
    cmpl $0, %eax                          # checking it's returing value is 0 or not
    jne .ret_001 
    popq %rdx                             # popping out of stack 
    movl %edx,THERMO_DISPLAY_PORT(%rip)   ## THERMO_DISPLAY_PORT = disp
    movq $0,%rax
    ret


.ret_001:
    movq $1,%rax
    popq %rdx                    # popping out of stack 
    ret 
 

