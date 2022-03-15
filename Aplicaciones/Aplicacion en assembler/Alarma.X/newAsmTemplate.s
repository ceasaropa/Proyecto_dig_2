PROCESSOR 16F84A
#include <xc.inc>
    
    CONFIG FOSC = XT
    CONFIG WDTE = OFF
    CONFIG PWRTE = OFF 
    CONFIG CP = OFF
    
    PSECT my_code, class=CODE, DELTA=2
    
my_code:
    bsf	STATUS,5
    movlw 0x1F
    movwf TRISA
    movlw 0x00
    movwf TRISB
    bcf STATUS,5
    
principal:
    movlw 00000000B
    movwf PORTB
    btfss PORTA,0
    goto principal
    
    goto enciende_led
    
enciende_led:
    movlw 00000001B
    movwf PORTB
    
    goto principal
    
end my_code
    
    


