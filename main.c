#include "device_config.h"
#include <stdlib.h>
#include <math.h>
#include <time.h>
#define LCD_DATA_R          PORTD
#define LCD_DATA_W          LATD
#define LCD_DATA_DIR        TRISD
#define LCD_RS              LATCbits.LATC2
#define LCD_RS_DIR          TRISCbits.TRISC2
#define LCD_RW              LATCbits.LATC1
#define LCD_RW_DIR          TRISCbits.TRISC1
#define LCD_E               LATCbits.LATC0
#define LCD_E_DIR           TRISCbits.TRISC0

#define _XTAL_FREQ 8000000
#define SWEEP_FREQ 200

enum por_ACDC {digital, analog};

//void declaration
void portsInit(void);
char key_scanner(void);
void LCD_init(void);
void LCD_cmd(char cx);
void send2LCD(char xy);
int get_num  (char ch);



void main(void){
    
int k2,k1;
char key1,key2,key3;
char opc;
int op;

OSCCON = 0b01110110;// Set the internal oscillator to 8MHz and stable
LCD_DATA_DIR = 0x00;
LCD_RS = 0;
LCD_RW = 0;
LCD_E  = 0;

portsInit();
LCD_init();

while (1){
LCD_cmd(0x1C); //Shift the entire display to the right

key1 = key_scanner(); //Scan the First Digit
k1 = get_num (key1); //Converting Char into value
send2LCD ('key1');
    
key3 = key_scanner(); //Scan the operand
    
LCD_cmd(0x01); // Clear display and move cursor home
LCD_cmd(0x1C); //Shift the entire display to the right
send2LCD ('key3');
LCD_cmd(0x01); //Decrement cursor (shift cursor to left)
send2LCD ('key1');

key2 = key_scanner(); //Scan the Second Digit
k2= get_num (key1); //Converting Char into value
    
LCD_cmd(0x01); // Clear display and move cursor home
LCD_cmd(0x1C); //Shift the entire display to the right
send2LCD ('key2');
LCD_cmd(0x01); //Decrement cursor (shift cursor to left)
send2LCD ('key3');
LCD_cmd(0x01); //Decrement cursor (shift cursor to left)
send2LCD ('key1');

if ( key_scanner() == '14')
{
    switch (key3)
{
    case '10':
    op= k1+k2;
    break;
    case '11':
    op= k2-k1;
    break;
    case '12':
    op= k2*k1;
    break;
    case '13':
    op= k2/k1;
    break;
}
    LCD_cmd(0xC0);//Force cursor to beginning ( 2nd line)
    LCD_cmd(0x1C);//Shift the entire display to the right
    send2LCD ('op');
    
}
if ( key_scanner() == '15')
{
    LCD_cmd(0x01); // Clear display and move cursor home
}   
}
}

void portsInit (void){
    ANSELB = digital;   // Set port A as Digital for keypad driving
    TRISB  = 0xF0;      // For Port A, set pins 4 to 7 as inputs (columns), and pins 0 to 3 as outputs (rows)
    ANSELC = digital;   // Set port C as Digital
    TRISC = 0X00;       //RC0 to E, RC1 to R/W, RC2 to RS
    ANSELD = digital;   // Set port D as Digital
    TRISD = 0x00;       // Data bus (D0-D7)
}

char key_scanner(void){
    LATBbits.LB0 = 0;
    LATBbits.LB1 = 1;
    LATBbits.LB2 = 1;
    LATBbits.LB3 = 1;
    __delay_ms(SWEEP_FREQ);
    if      (PORTBbits.RB4 == 0) {__delay_ms(SWEEP_FREQ); return 1;}
    else if (PORTBbits.RB5 == 0) {__delay_ms(SWEEP_FREQ); return 2;}
    else if (PORTBbits.RB6 == 0) {__delay_ms(SWEEP_FREQ); return 3;}
    else if (PORTBbits.RB7 == 0) {__delay_ms(SWEEP_FREQ); return 10;}
    LATBbits.LB0 = 1;
    LATBbits.LB1 = 0;
    LATBbits.LB2 = 1;
    LATBbits.LB3 = 1;
    __delay_ms(SWEEP_FREQ);
    if      (PORTBbits.RB4 == 0) {__delay_ms(SWEEP_FREQ); return 4;}
    else if (PORTBbits.RB5 == 0) {__delay_ms(SWEEP_FREQ); return 5;}
    else if (PORTBbits.RB6 == 0) {__delay_ms(SWEEP_FREQ); return 6;}
    else if (PORTBbits.RB7 == 0) {__delay_ms(SWEEP_FREQ); return 11;}
    LATBbits.LB0 = 1;
    LATBbits.LB1 = 1;
    LATBbits.LB2 = 0;
    LATBbits.LB3 = 1;
    __delay_ms(SWEEP_FREQ);
    if      (PORTBbits.RB4 == 0) {__delay_ms(SWEEP_FREQ); return 7;}
    else if (PORTBbits.RB5 == 0) {__delay_ms(SWEEP_FREQ); return 8;}
    else if (PORTBbits.RB6 == 0) {__delay_ms(SWEEP_FREQ); return 9;}
    else if (PORTBbits.RB7 == 0) {__delay_ms(SWEEP_FREQ); return 12;}
    LATBbits.LB0 = 1;
    LATBbits.LB1 = 1;
    LATBbits.LB2 = 1;
    LATBbits.LB3 = 0;
    __delay_ms(SWEEP_FREQ);
    if      (PORTBbits.RB4 == 0) {__delay_ms(SWEEP_FREQ); return 14;}
    else if (PORTBbits.RB5 == 0) {__delay_ms(SWEEP_FREQ); return 0;}
    else if (PORTBbits.RB6 == 0) {__delay_ms(SWEEP_FREQ); return 15;}
    else if (PORTBbits.RB7 == 0) {__delay_ms(SWEEP_FREQ); return 13;}   
    else return 'x';
}

// LCD initialization function
void LCD_init(void){
    LATC = 0;               // Make sure LCD control port is low
    LCD_E_DIR = 0;          // Set Enable as output
    LCD_RS_DIR = 0;         // Set RS as output 
    LCD_RW_DIR = 0;         // Set R/W as output
    LCD_cmd(0x38);          // Display to 2x40
    LCD_cmd(0x0F);          // Display on, cursor on and blinking
    LCD_cmd(0x01);          // Clear display and move cursor home
}

// Send command to the LCD
void LCD_cmd(char cx) {
    LCD_RS = 0;             // select IR register
    LCD_RW = 0;             // set WRITE mode
    LCD_E  = 1;             // set to clock data
    Nop();
    LCD_DATA_W = cx;        // send out command
    Nop();                  // No operation (small delay to lengthen E pulse)
    LCD_E = 0;              // complete external write cycle
}

// Function to display data on the LCD
void send2LCD(char xy){
    LCD_RS = 1;
    LCD_RW = 0;
    LCD_E  = 1;
    LCD_DATA_W = xy;
    Nop();
    Nop();
    LCD_E  = 0;
}

int get_num(char ch)         //converting character into integer
{
switch(ch)
{
case '0': return 0; break;
case '1': return 1; break;
case '2': return 2; break;
case '3': return 3; break;
case '4': return 4; break;
case '5': return 5; break;
case '6': return 6; break;
case '7': return 7; break;
case '8': return 8; break;
case '9': return 9; break;
}
return 0;
}
