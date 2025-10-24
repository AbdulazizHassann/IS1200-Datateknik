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
    int result = value & 0x1; // mask last bit to get the button status if it pressed or not.
  return result; 
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
    int swch = get_sw();

    if (((swch >> 7) & 0x1) == 1)
    {
      break;
    }
    int button = get_btn();
    if(button == 1){ 
      
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

/* Below is the function that will be called when an interrupt is triggered. */
void handle_interrupt(unsigned cause) 
{}

/* Add your code here for initializing interrupts. */
void labinit(void)
{}

/* Your code goes into main as well as any needed functions. */
int main() {
   //set_leds(0xF); //0x555 shows LEDS: on, off , on, off ,on, off ,on, off ,on, off ,
/* 
    set_leds_seconds();
   
    set_displays(1,0x19);
   set_displays(2,0x24);
   set_displays(3,0x02);
   set_displays(4,0x40);
   set_displays(5,0x30);
   set_displays(6,0x10); 
   set_displays(1,0x12);
   set_displays(2,0xf9);
   set_displays(3,0x10);
   set_displays(4,0x40);
   set_displays(5,0x10);
   set_displays(6,0x12);
 */
   //int swch = get_sw();
   //set_leds(swch);
  
 //show_time_7segment();
   
  // Call labinit()
  labinit();

  // Enter a forever loop
  while (1) {
    show_time_7segment();
    time2string( textstring, mytime ); // Converts mytime to string
    display_string( textstring ); //Print out the string 'textstring'
    delay( 2 );          // Delays 1 sec (adjust this value)
    tick( &mytime );     // Ticks the clock once
  }
}


