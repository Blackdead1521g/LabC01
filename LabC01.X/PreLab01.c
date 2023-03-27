/* 
 * File:   newmain.c
 * Author: Kevin Alarcón
 *
 * Created on 23 de marzo de 2023, 04:57 PM
 */

// CONFIG1
#pragma config FOSC = INTRC_NOCLKOUT// Oscillator Selection bits (INTOSCIO oscillator: I/O function on RA6/OSC2/CLKOUT pin, I/O function on RA7/OSC1/CLKIN)
#pragma config WDTE = OFF       // Watchdog Timer Enable bit (WDT disabled and can be enabled by SWDTEN bit of the WDTCON register)
#pragma config PWRTE = OFF      // Power-up Timer Enable bit (PWRT disabled)
#pragma config MCLRE = OFF      // RE3/MCLR pin function select bit (RE3/MCLR pin function is digital input, MCLR internally tied to VDD)
#pragma config CP = OFF         // Code Protection bit (Program memory code protection is disabled)
#pragma config CPD = OFF        // Data Code Protection bit (Data memory code protection is disabled)
#pragma config BOREN = OFF      // Brown Out Reset Selection bits (BOR disabled)
#pragma config IESO = OFF       // Internal External Switchover bit (Internal/External Switchover mode is disabled)
#pragma config FCMEN = OFF      // Fail-Safe Clock Monitor Enabled bit (Fail-Safe Clock Monitor is disabled)
#pragma config LVP = OFF        // Low Voltage Programming Enable bit (RB3 pin has digital I/O, HV on MCLR must be used for programming)

// CONFIG2
#pragma config BOR4V = BOR40V   // Brown-out Reset Selection bit (Brown-out Reset set to 4.0V)
#pragma config WRT = OFF        // Flash Program Memory Self Write Enable bits (Write protection off)

#include <xc.h>
#define _tmr0_value 200 // valor de tmr0 para la interupcion 
#define _XTAL_FREQ 8000000 
#define LED_PIN PORTEbits.RE0 /// usar e0 como salida del led

// Rutina de  interrupciones
void __interrupt() isr(void) {
    if (INTCONbits.T0IF) { // Si se produjo una interrupción por desbordamiento de TMR0
        LED_PIN = ~LED_PIN;//alterna entre encendido y a apagado
        INTCONbits.T0IF = 0; // Limpia la bandera de interrupción
        TMR0 = 216;           // Reinicia el valor de TMR0
    }
}


void setup(void);//prototipo del setup

///main
void main(void) {
    setup ();
    while(1){
            
    }
    return;
}

/////setup general
void setup(void){
    //definir digitales
    ANSEL = 0;
    ANSELH = 0;
    
    // Configura el puerto E
    TRISEbits.TRISE0 = 0; // Configura RE0 como salida para el LED
    LED_PIN = 0;    // Inicializa RE0 en estado bajo 

    //////////////oscilador
    OSCCONbits.IRCF = 0b111 ; ///8Mhz
    OSCCONbits.SCS = 1;
    

    /////////////// tmr0
    OPTION_REGbits.T0CS = 0; //Usar Timer0 con Fosc/4
    OPTION_REGbits.PSA = 0; //Prescaler con el Timer0
    OPTION_REGbits.PS2 = 1; //Prescaler de 256
    OPTION_REGbits.PS1 = 1;
    OPTION_REGbits.PS0 = 1;  
    TMR0 = 216; ///VALOR INICIAL DEL TMR0
    
    /////////Banderas e interrupciones
    INTCONbits.T0IF = 0; //interrupcion del tmr0
    INTCONbits.T0IE = 1;
    INTCONbits.GIE = 1; //globales
   
    
}
