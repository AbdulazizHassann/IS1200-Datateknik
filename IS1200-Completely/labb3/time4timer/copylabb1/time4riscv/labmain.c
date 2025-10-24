#include <stdio.h>

/* main.c

   This file written 2024 by Artur Podobas and Pedro Antunes

   For copyright and licensing, see file COPYING */


/* Below functions are external and found in other files. */
extern void print(const char*);
extern void print_dec(unsigned int);
extern void display_string(char*);
extern void time2string(char*,int);
extern void tick(int*);
extern void delay(int);
extern int nextprime( int );


int mytime = 0x5957;
char textstring[] = "text, more text, and even more text!";
int timeoutcount = 0; 

//function that trun on LEDS assignment c
void set_leds(int led_mask){

    volatile int *LEDS = (volatile int*) 0x04000000;
    *LEDS = led_mask;
    
}
// function that sets LEDS every second . assignment d.
void set_leds_seconds(){
  int count = 0; 
  while(count <= 0xF){
    set_leds(count); // print out the value in LEDS
    for(volatile int i = 0; i < 900000; i++){
      
     
    } // delay 1 second
       
      count++;
  }
   set_leds(0xF);   
  
}

//  function to display on the 7 display Segment. assignment e 
 void set_displays(int display_number, int value){
 // 1 = off and 0 = on 
  volatile int *segm7;
    volatile int* digit_segment[6] = {
      (volatile int*) 0x04000050,
      (volatile int*) 0x04000060,
      (volatile int*) 0x04000070,
      (volatile int*) 0x04000080,
      (volatile int*) 0x04000090,
      (volatile int*) 0x040000A0
    };

    const int digit_codes[10] = {
      0x40, // 0
      0xF9, // 1
      0x24, // 2
      0x30, // 3
      0x19, // 4
      0x12, // 5
      0x02, // 6
      0xF8, // 7
      0x00, // 8
      0x10  // 9
    };

  segm7 = digit_segment[display_number];
  *segm7 = digit_codes[value];
}
 // function that return swichs status  assignment f 
 int get_sw(void){
    volatile int* swechis = (volatile int*) 0x04000010;
    int value = *swechis; 
    int result = value & 0x3FF;  // mask all bit beside 10 least bits 
    return result;
 }
// function that return the state of the button KEY1. assignment g  
 int get_btn(void){
  volatile int* button = (volatile int*) 0x040000d0;
    int value = *button;
  return value & 0x1; // (value >> 1) & 0x1 this give not KEY1 why ask teacher. but like this now it works.
 }

//function that shows H:M:S in 7 display segment. assigment h.
 void show_time_7segment(){
  int sec = 0; 
  int min = 0; 
  int hour = 0; 

  while(1){
    if(sec == 60 ){ // if sec = 60 then increase min by one . then restart seconds again.
      min++;
      sec = 0; 
    }
    if(min == 60 ){ // if min = 60 then increase hour by one . restart minutes again.
      hour++;
      min = 0; 
    }
    
     for(volatile int i = 0; i < 900000; i++){} // delay 1 second
       
    sec++;
   
    int button = get_btn();
    if(button == 1){ 
      int swch = get_sw();
      int updateValue = ( swch & 0x3F);// mask last 6 bits from swch. 
      int leftmostSW = (swch >> 8) & 0x3; //  exp 0011 0000 0000 >> 8 = 0000 0000 0011 & 0x2 =  0000 0000 0011.
      if(leftmostSW == 0x1){ // seconds two most right 7 segment 
        sec = updateValue;
        

      } else if( leftmostSW == 0x2){ // minutes the two middle 7 segment
        min = updateValue;
        
      }else if(leftmostSW == 0x3){ // hours the most left 7 segment
        hour = updateValue;  
      }
    }

    // updates shows display 
      set_displays(0,(sec % 10));
      set_displays(1,(sec / 10));
      set_displays(2,(min % 10));
      set_displays(3,(min / 10));
      set_displays(4,(hour % 10));
      set_displays(5,(hour / 10));


  }
 }
  void show_time(int *mytime){
    // Plocka ut timmar, minuter, sekunder ur mytime
    int sec  =  *mytime        & 0xFF;
    int min  = (*mytime >> 8)  & 0xFF;
    int hour = (*mytime >> 16) & 0xFF;

    int button = get_btn();
    int swch = get_sw();
    if(button == 1){ 
        int updateValue = (swch & 0x3F);       // mask last 6 bits from swch
        int leftmostSW  = (swch >> 8) & 0x3;   // check top 2 switches
        if(leftmostSW == 0x1) {        // seconds
            sec = updateValue;
        } else if(leftmostSW == 0x2) { // minutes
            min = updateValue;
        } else if(leftmostSW == 0x3) { // hours
            hour = updateValue;
        }
        // bygg ihop mytime igen (BCD)
        *mytime = (hour << 16) | (min << 8) | sec;
    }

    // BCD -> separate digits (beräknas efter ev. uppdatering)
    int sec_ones   = sec & 0xF;
    int sec_tens   = (sec >> 4) & 0xF;
    int min_ones   = min & 0xF;
    int min_tens   = (min >> 4) & 0xF;
    int hour_ones  = hour & 0xF;
    int hour_tens  = (hour >> 4) & 0xF;

    // write to 7-segment display
    set_displays(0, sec_ones);
    set_displays(1, sec_tens);
    set_displays(2, min_ones);
    set_displays(3, min_tens);
    set_displays(4, hour_ones);
    set_displays(5, hour_tens); 
}


/* Below is the function that will be called when an interrupt is triggered. */
void handle_interrupt(unsigned cause) 
{}

/* Add your code here for initializing interrupts. */
/*function for timer . assignment 2 */
void labinit(void){
  volatile int* timer = (volatile int*) 0x04000020;

  /*to calculate PeriodRegister :  T(timeout) = P(Register) * T( av frekvens)
    T = 1/f = 1/ (30*10^6)  ,  T(timeout) = 100ms = 0,1
    P(Register) = 0,1 * (30*10^6) =  3*10^6 deci   = hex = 2DC6C0
    bin = 0010 1101 1100 0110 1100 0000 
    tot 4 byes = 0000 0000 0010 1101 1100 0110 1100 0000
    periodR = 1100 0110 1100 0000 = 0xC6C0
    periodL = 0000 0000 0010 1101 = 0x002D
    
  */
 int periodL = 0xC6C0;
 int periodH = 0x002D;
 *(timer + 2 ) = periodL;
 *(timer + 3) = periodH;
 int conT = 0x7; // {bit3 (stop), bit2(start), bit1(conT), bit0(ITO): 0111 = 0x7 sets start ,conT and TO to 1}
  // timer address + 4 bytes = control. and so on for the other offsets.
*(timer + 1) = conT;  


}

/* Your code goes into main as well as any needed functions. */
int main() {
  // Call labinit()
  labinit();
  
  // Enter a forever loop
  while (1) {
  
    volatile int* timer = (volatile int*) 0x04000020;
    int status = *timer ; // reading the satus timer 
    int TO = status & 0x1; // mask last bit that is for TO (timeout)
 
    /*we only display when the timout happen */
    if(TO == 1){
     *timer = 1;  // write back T0 in timer (1 to last bit)
    
     timeoutcount++; //assigment c : count how many 100 ms timeouts have passed
     if(timeoutcount == 10){ // exp 200 * 100 ms = 20 secs, then update the displays
                              //note: for the labb it´s  10 * 100ms = 0.1 secs 
        timeoutcount = 0; 
        show_time(&mytime);
        time2string( textstring, mytime ); // Converts mytime to string
        display_string( textstring ); //Print out the string 'textstring'
        tick( &mytime );     // Ticks the clock once
      }
    }
    
    
  }
}


