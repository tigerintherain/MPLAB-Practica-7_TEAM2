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

#define PULSE               PORT.RA4

#define _XTAL_FREQ 8000000
#define SWEEP_FREQ 200

enum por_ACDC {digital, analog};

//void declaration
void portsInit(void);
void LCD_init(void);
void LCD_cmd(char cx);
void send2LCD(char xy);

int overflow; //for interrupts
char cnt;

char LB, HB; //for frequency calculation
long t_elapse;

void __interrupt (high_priority)  high_priority_ISR(void)
{
 if (INTCON.TMROIF == 1)  // if TIMER0 interrupt occurs
{
     overflow++;          // increments overflow count
     INTCON.TMR0IF = 0;   //clears intrrupt flag
}
 if (PIR1.TM1IF == 1 )    // if TIMER1 interrupt occurs
{
     TMR1H = 0x0B;       //Time register low and high
     TMR1L = 0xDC;       
     cnt++;              //Increments count (cnt)
     PIR1.TMR1IF = 0;     // Clear Timer1 interrrupt flag
}
}

void main(void)
{
portsInit();
LCD_init();
 
OSCCON = 0b01110110;// Set the internal oscillator to 8MHz and stable
LCD_DATA_DIR = 0x00;
LCD_RS = 0;
LCD_RW = 0;
LCD_E  = 0;
 
LCD_cmd(0x80);//set cursor to first line
send2LCD("frequency:");

T0CON = 0x28; //TIMER0 as 16-bit, no prescaler
T1CON = 0x36; //TIMER1 for 250 ms overflow
 
PIE1 = 0x01  //Enable TIMER 1 interrupts
PIR1.TMR1IF = 0; //Clear TIMER1 interrupt flag
INTCON = 0xE0;   //Enable TIMER0 AND TIMER1 interrupts

while (1)
{
TMR0H = 0; //Clear TIMER0 register
TMR0L = 0;
TMR1H = 0x0B; //TIMER1 registers 3036 = 0x0BDC for 250 ms
TMR1L = 0xDC;
overflow = 0; //clear overflow count
cnt = 0;      //clear cnt
 
TOCON.TMR0ON = 1; //start TIMER0, incrments when pulse is detected. 
T1CON.TMR1ON = 1;//start TIMER1 to count 250ms(4) = 1 s

while(cnt !=4); //wait fo 4 cycles
 
TOCON.TMR0ON = 0; // Stop TIMER0
T1CON.TMR1ON = 0; // Stop TIMER1

LB = TMR0L; //gets TIMER0 values
HB = TMROH
 
t_elapse = (long)256*HB + LB;
t_elapse = 65536*overflow + t_lapse;
 
char t_elapsechar = (char)t_elapse;
 
 LCD_cmd(0xC0); //set cursor to second line
 send2LCD(t_elapsechar); //send frequency
 
 LCD_cmd(0x14); //shift cursor position to right
 LCD_cmd(0x14); //shift cursor position to right
 LCD_cmd(0x14); //shift cursor position to right
 LCD_cmd(0x14); //shift cursor position to right
 
 send2LCD("Hz"); //send Hz

}
}

void portsInit (void){
    ANSELC = digital;   // Set port C as Digital
    TRISC = 0X00;       // RC0 to E, RC1 to R/W, RC2 to RS
    ANSELD = digital;   // Set port D as Digital
    TRISD = 0x00;       // Data bus (D0-D7) 
    ANSELA = analog;    //set port a as analog
    TRIS.RA4 = 1;       // Set RA4 as input
    
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


