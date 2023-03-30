/* 
 * File:   Lab01-2.c
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
#define LED_PIN PORTDbits.RD0 /// usar e0 como salida del led

// --------------- Rutina de  interrupciones --------------- 
void __interrupt() isr(void) {
    if (INTCONbits.T0IF) { // si se produce overflow en el TMR0 y se activa la bandera
        LED_PIN = ~LED_PIN; // alternar el estado del led entre encendido y apagado
        TMR0 = _tmr0_value;           // reestablecer el valor necesario para el TMR0
        INTCONbits.T0IF = 0; // limpiar bandera de interrupcion del TMR0
    }
    if (PIR1bits.ADIF) { // si se activa la bandera de interrupcion del ADC
        if (ADCON0bits.CHS == 0b0000){ // si está en ADC AN0
            PORTB = ADRESH; // asignar el PORTB como el potenciometro de PORTA0
        }
        else if (ADCON0bits.CHS == 0b0100){ // si está en ADC AN5
            PORTC = ADRESH; // asignar el PORTC como el potenciometro de PORTA5
        }
        PIR1bits.ADIF = 0; // limpiar la bandera de la interrupcion
    }
}
void setup(void);

// --------------- main --------------- 
void main(void) {
    setup ();
    //setupADC ();
    while(1){
        
        if (ADCON0bits.GO == 0) { // si la lectura del ADC se desactiva
            if(ADCON0bits.CHS == 0b0000)
            {
                ADCON0bits.CHS = 0b0100; // cambiar a ADC AN5
            }
            else if(ADCON0bits.CHS == 0b0100)
            {
                ADCON0bits.CHS = 0b0000; // cambiar a ADC AN0
            }
            __delay_us(50);
            ADCON0bits.GO = 1;
        }
    }
    return;
}

void setup(void){
    //definir digitales
    ANSELbits.ANS0 = 1; //Seleccionamos solo los dos pines que utilizaremos
    ANSELbits.ANS4 = 1;
    ANSELH = 0;
    
    //Definimos puertos que serán salidas
    TRISB = 0;
    TRISC = 0;
    TRISD = 0;
    TRISE = 0;
    
    //Limpiamos los puertos
    PORTA = 0;
    PORTB = 0;
    PORTC = 0;
    PORTD = 0;
    PORTE = 0;
    // Configura el puerto E
    LED_PIN = 0;    // Inicializa RE0 en estado bajo
    
    //////////////oscilador
    OSCCONbits.IRCF = 0b111 ; ///8Mhz
    OSCCONbits.SCS = 1; //Utilizar oscilados interno
    

    /////////////// tmr0
    OPTION_REGbits.T0CS = 0; //Usar Timer0 con Fosc/4
    OPTION_REGbits.PSA = 0; //Prescaler con el Timer0
    OPTION_REGbits.PS2 = 1; //Prescaler de 256
    OPTION_REGbits.PS1 = 1;
    OPTION_REGbits.PS0 = 1;  
    TMR0 = _tmr0_value; ///VALOR INICIAL DEL TMR0
    
    /////////Banderas e interrupciones
    INTCONbits.T0IF = 0; //Apagamos la bandera del TMR0
    INTCONbits.T0IE = 1; //Habilitar interrupciones del TMR0
    INTCONbits.GIE = 1; //Habilitar interrupciones globales
    INTCONbits.PEIE = 1; //Habilitar interrupciones periféricas
    PIE1bits.ADIE = 1; //Habilitar interrupciones del ADC
    PIR1bits.ADIF = 0; //Apagamos la bandera del ADC
    
        //Configuración ADC
    ADCON0bits.CHS = 0b0000; //Elegimos canal RA0 como inicial
    ADCON1bits.VCFG0 = 0; //Voltaje referenciado de 0V
    ADCON1bits.VCFG1 = 0; //Voltaje referenciado de 5V
    ADCON0bits.ADCS = 0b10; // Fosc/32
     
    ADCON1bits.ADFM = 0; //Justificado a la izquierda
    ADCON0bits.ADON = 1;//Encendemos el módulo del ADC
    __delay_ms(1); 
    return;
}