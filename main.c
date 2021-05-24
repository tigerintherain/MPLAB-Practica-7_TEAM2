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
void LCD_init(void);
void LCD_cmd(char cx);
void send2LCD(char xy);

void main(void){
OSCCON = 0b01110110;// Set the internal oscillator to 8MHz and stable
LCD_DATA_DIR = 0x00;
LCD_RS = 0;
LCD_RW = 0;
LCD_E  = 0;

void LCD_cmd(0x80);//set cursor to first line
void sen2LCD("frequency:");
 
portsInit();
LCD_init();

while (1){
    
 
void LCD_cmd(0xC0); //set cursos to second line
void send2LCD("Hertz");
}
}

void portsInit (void){
    ANSELC = digital;   // Set port C as Digital
    TRISC = 0X00;       //RC0 to E, RC1 to R/W, RC2 to RS
    ANSELD = digital;   // Set port D as Digital
    TRISD = 0x00;       // Data bus (D0-D7)
    
    
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


