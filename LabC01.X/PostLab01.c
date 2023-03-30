 /* Archivo: PostLab01.c
 * Dispositivo: PIC16F887
 * Autor: Kevin Alarcón
 * Compilador: XC8(v2.40), MPLABX V6.05
 * 
 * 
 * Programa: Convertir una señal analógica a digital
 * Hardware: Potenciómetros en RA0 Y RA5; 3 displays en RD0, RD1 Y RD2
 * 
 * Creado: 27 de marzo, 2023
 * Última modificación: 30 de marzo, 2023
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

//---------------------Librerías----------------------------------
#include <xc.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <pic16f887.h>


#define _tmr0_value 255 // valor de tmr0 para la interupcion 
#define _XTAL_FREQ 8000000 //definimos la frecuencia del oscilador

//--------------------Variables------------------------------------
int banderas = 0;
int contador = 0;
int residuo = 0;
int potenciometro = 0;
int centenas = 0;
int decenas = 0;
int unidades = 0;
int display1 = 0;
int display2 = 0;
int display3 = 0;

uint8_t table (int X){ //Función que contiene los valores que se le ingresarán a los displays
    switch(X){ //Realizamos un switch case para ir verificando en qué número está el parámetro (dependiendo el número le asignamos su respecto valor al display)
        case 0: 
            return 0b00111111; //(0)
        case 1:
            return 0b00000110; //(1)
        case 2:
            return 0b01011011; //(2)
        case 3:
            return 0b01001111; //(3)
        case 4:
            return 0b01100110; //(4)
        case 5:
            return 0b01101101; //(5)
        case 6:
            return 0b01111101; //(6)
        case 7:
            return 0b00000111; //(7)
        case 8:
            return 0b01111111; //(8)
        case 9:
            return 0b01101111; //(9)
        case 10:
            return 0b01110111; //(A)
        case 11:
            return 0b01111100; //(B)
        case 12:
            return 0b00111001; //(C)
        case 13:
            return 0b01011110; //(D)
        case 14:
            return 0b01111001; //(E)
        case 15:
            return 0b01110001; //(F)
    }
}

//-------------------Prototipos de funciones------------------
void setup(void);
void displays (int numero);
void preparar_displays(void);
uint8_t table (int X);

// --------------- Rutina de  interrupciones --------------- 
void __interrupt() isr(void) {
    if (INTCONbits.T0IF) { // si se produce overflow en el TMR0 y se activa la bandera
        PORTD = 0; //Inicialmente limpiamos el puerto donde estará el multiplexado
        if (banderas == 0){ //Verificamos si bandera es igual a cero
            PORTC = display3; //Si si es igual a cero le asignamos al puerto C el valor de display3 (centenas)
            PORTD = 1; //Encendemos el primer bit del puerto de multiplexado
            banderas = 1; //Igualamos a uno la variable de banderas
        }
        else if (banderas == 1){ //Verificamos si bandera es igual a uno
            PORTC = display2;  //Si si es igual a uno le asignamos al puerto C el valor de display2 (decenas)
            PORTD = 2;  //Encendemos el segundo bit del puerto de multiplexado
            banderas = 2; //Igualamos a uno la variable de banderas
        }
        else if (banderas == 2){ //Verificamos si bandera es igual a dos
            PORTC = display1; //Si si es igual a dos le asignamos al puerto C el valor de display1 (unidades)
            PORTD = 4; //Encendemos el tercer bit del puerto de multiplexado
            banderas = 0; //Igualamos a cero la variable de banderas para que se reinicie el diclo
        }
        TMR0 = _tmr0_value; //Reestablecer el valor necesario para el TMR0
        INTCONbits.T0IF = 0; //Limpiar bandera de interrupcion del TMR0
    }
    if (PIR1bits.ADIF) { //Si se activa la bandera de interrupcion del ADC
        if (ADCON0bits.CHS == 0b0000){ //Si está en ADC AN0
            PORTB = ADRESH; //Asignar el PORTB el valor del potenciomnetro del PORTA0
        }
        else if (ADCON0bits.CHS == 0b0100){ //Si está en ADC AN4
            potenciometro = ADRESH; //Asignar a la variable potenciometro el valor del potenciometro del PORTA5
        }
        PIR1bits.ADIF = 0; //Limpiar la bandera de la interrupcion del ADC
    }
    return; 
}
// --------------- main --------------- 
void main(void) {
    setup (); 
    ADCON0bits.GO = 1; //Activamos la lectura del ADC
    while(1){ //loop forever
        preparar_displays(); //Llamamos a nuestra función que prepara los valores de los displays
        
        contador = 1.960784314*potenciometro; //Le ingresamos a la variable contador el valor que tiene el potenciometro convertido con el fin de que vaya en un  rango de 0 - 500
        displays(contador); //Llamamos a nuestra función de displays y le ingresamos como parámetro la variable contador
        
        if (ADCON0bits.GO == 0) { // Si la lectura del ADC se desactiva
            if(ADCON0bits.CHS == 0b0000) //Revisamos si el canal esta en el AN0
            {
                ADCON0bits.CHS = 0b0100; //Si, sí está cambiamos el ADC al canal AN4
            }
            else if(ADCON0bits.CHS == 0b0100) //Revisamos si el canal esta en el AN4
            {
                ADCON0bits.CHS = 0b0000; //Si, sí está cambiamos el ADC al canal AN0
            }
            __delay_us(1000); //Este es el tiempo que se dará cada vez que se desactiva la lectura
            ADCON0bits.GO = 1; //Activamos la lectura del ADC
        }
    }
    return;
}

void displays (int numero){ //Esta función nos permite dividir la variable de nuestro contador en unidades, decenas y centenas.
    centenas = (uint8_t)(numero/100); //Para centenas dividimos el contador dentro de 100 y que tome solo el valor entero de esa división
    residuo = numero % 100; //Le asignamos a la variable residuo el residuo de la división dentro de 100
    
    decenas = residuo/10; //Para decenas tomamos el residuo anterior y los dividimos dentro de 10 para que nos de solo las decenas
    residuo = numero % 10; //Le asignamos a la variable residuo el residuo de la división dentro de 10 (esto nos dará las unidades)
    
    unidades = residuo; //Para unidades solo la igualamos al residuo que calculamos antetriormente
    return;
}

void preparar_displays(void){ //En esta función le asignaremos a cada variable de display su respectiva unidad
    display3 = table(centenas); //Al display3 le asignamos las centenas
    display2 = table(decenas); //Al display2 le asignamos las decenas
    display1 = table(unidades); //Al display1 le asignamos las unidades
    return;
}

void setup(void){
    //definir digitales
    ANSELbits.ANS0 = 1; //Seleccionamos solo los dos pines que utilizaremos como analógicos
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