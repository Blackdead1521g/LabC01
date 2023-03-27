/* 
 * File:   Lab01.c
 * Author: Kevin Alarcón
 *
 * Fecha de creación: 26 de marzo de 2023, 05:12 PM
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
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <pic16f887.h>

#define _tmr0_value 200 // valor de tmr0 para la interupcion 
#define _XTAL_FREQ 8000000 
#define LED_PIN PORTEbits.RE0 /// usar e0 como salida del led

void __interrupt() isr (void){
    if (T0IF){
        LED_PIN = ~LED_PIN;
        TMR0 = _tmr0_value; ///VALOR INICIAL DEL TMR0
        T0IF = 0;
    }
}
void setup(void);

void main(void){
    setup();
    while(1){
        ADCON0bits.GO = 1;
        while (ADIF==0); //Mientras la bandera esté apagada que haga esto
        uint16_t adc = (uint16_t)((ADRESH<<8) | ADRESL); //Le ingresamos 16 bits al adc le corremos los primeros 8 bits hacia la isquierda y le colocamos los demás en la derecha
        PORTB = (char) (adc>>2);//(adc>>2); //Le ingresamos al portd los primero 10 bits del adc y corremos dos bits para que solo sean 8 bits
        PORTD = (char)(adc & 0b0011);
        __delay_ms(10);
    }
    return;
}

void setup(void){
    //definir digitales
    ANSEL = 0;
    ANSELH = 0;
    TRISC = 0;
    TRISB = 0;
    TRISD = 0;
    
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
    TMR0 = _tmr0_value; ///VALOR INICIAL DEL TMR0
    
    /////////Banderas e interrupciones
    INTCONbits.T0IF = 0; //interrupcion del tmr0
    INTCONbits.T0IE = 1; //Hbilitar interrupción del TMR0
    INTCONbits.GIE = 1; //globales
    
        //Configuración ADC
    ANSEL = 0b01; //Pines analogicos
    TRISA = 0b01; //Puerto A como entrada
    ADCON0bits.ADCS = 0b10; // Fosc/32
    ADCON0bits.CHS = 0; //Elegimos canal RA0
    __delay_ms(1);  
    ADCON1bits.ADFM = 1; //Justificado a la derecha
    ADCON1bits.VCFG0 = 0; //Voltaje referenciado al pin
    ADCON1bits.VCFG1 = 0;
    ADCON0bits.ADON = 1;//Encendemos el módulo del ADC
    ADIF = 0; //Limpiamos la bandera del ADC
}

