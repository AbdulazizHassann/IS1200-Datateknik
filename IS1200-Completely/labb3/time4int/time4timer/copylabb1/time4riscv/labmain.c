#include <stdio.h>

/* main.c

   This file written 2024 by Artur Podobas and Pedro Antunes

   For copyright and licensing, see file COPYING */

/* Below functions are external and found in other files. */
extern void print(const char *);
extern void print_dec(unsigned int);
extern void display_string(char *);
extern void time2string(char *, int);
extern void tick(int *);
extern void delay(int);
extern int nextprime(int);
extern void enable_interrupt(void);

int mytime = 0x5957;
char textstring[] = "text, more text, and even more text!";
int prime = 1234567;
int timeoutcount = 0;

// function that trun on LEDS assignment c
void set_leds(int led_mask)
{
  volatile int *LEDS = (volatile int *)0x04000000;
  *LEDS = led_mask;
}

// function that sets LEDS every second . assignment d.
void set_leds_seconds()
{
  int count = 0;
  while (count <= 0xF)
  {
    set_leds(count); // print out the value in LEDS
    for (volatile int i = 0; i < 900000; i++)
    {
    } // delay 1 second
    count++;
  }
  set_leds(0xF);
}

//  function to display on the 7 display Segment. assignment e
void set_displays(int display_number, int value)
{
  volatile int *segm7;
  volatile int *digit_segment[6] = {
      (volatile int *)0x04000050,
      (volatile int *)0x04000060,
      (volatile int *)0x04000070,
      (volatile int *)0x04000080,
      (volatile int *)0x04000090,
      (volatile int *)0x040000A0};

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
int get_sw(void)
{
  volatile int *swechis = (volatile int *)0x04000010;
  int value = *swechis;
  int result = value & 0x3FF; // mask all bit beside 10 least bits
  return result;
}

// function that return the state of the button KEY1. assignment g
int get_btn(void)
{
  volatile int *button = (volatile int *)0x040000d0;
  int value = *button;
  return value & 0x1;
}

// function that shows H:M:S in 7 display segment. assigment h.
void show_time_7segment()
{
  int sec = 0;
  int min = 0;
  int hour = 0;

  while (1)
  {
    if (sec == 60)
    {
      min++;
      sec = 0;
    }
    if (min == 60)
    {
      hour++;
      min = 0;
    }

    for (volatile int i = 0; i < 900000; i++)
    {
    } // delay 1 second
    sec++;

    int button = get_btn();
    if (button == 1)
    {
      int swch = get_sw();
      int updateValue = (swch & 0x3F);
      int leftmostSW = (swch >> 8) & 0x3;
      if (leftmostSW == 0x1)
      {
        sec = updateValue;
      }
      else if (leftmostSW == 0x2)
      {
        min = updateValue;
      }
      else if (leftmostSW == 0x3)
      {
        hour = updateValue;
      }
    }

    set_displays(0, (sec % 10));
    set_displays(1, (sec / 10));
    set_displays(2, (min % 10));
    set_displays(3, (min / 10));
    set_displays(4, (hour % 10));
    set_displays(5, (hour / 10));
  }
}

void show_time(int *mytime)
{
  int sec = *mytime & 0xFF;
  int min = (*mytime >> 8) & 0xFF;
  int hour = (*mytime >> 16) & 0xFF;

  int button = get_btn();
  int swch = get_sw();
  if (button == 1)
  {
    int updateValue = (swch & 0x3F);
    int leftmostSW = (swch >> 8) & 0x3;
    if (leftmostSW == 0x1)
    {
      sec = updateValue;
    }
    else if (leftmostSW == 0x2)
    {
      min = updateValue;
    }
    else if (leftmostSW == 0x3)
    {
      hour = updateValue;
    }

    *mytime = (hour << 16) | (min << 8) | sec;
  }

  int sec_ones = sec & 0xF;
  int sec_tens = (sec >> 4) & 0xF;
  int min_ones = min & 0xF;
  int min_tens = (min >> 4) & 0xF;
  int hour_ones = hour & 0xF;
  int hour_tens = (hour >> 4) & 0xF;

  set_displays(0, sec_ones);
  set_displays(1, sec_tens);
  set_displays(2, min_ones);
  set_displays(3, min_tens);
  set_displays(4, hour_ones);
  set_displays(5, hour_tens);
}

/* Interrupt handler */
void handle_interrupt(unsigned cause)
{
  if (cause == 17)
  {
    /* Switch interrupt */
    volatile int *sw_base = (volatile int *)0x04000010;
    volatile int *edgecapture = (volatile int *)(0x04000010 + 0xC); // offset 3*4
    int edges = *edgecapture;

    if (edges & 0x2)
    {                     // SW1 is bit1
      *edgecapture = 0x2; // acknowledge SW1 edge

      for (volatile int i = 0; i < 8000; i++)
      {
        int dummy = *sw_base;
        (void)dummy;
      }

      tick(&mytime);
      time2string(textstring, mytime);
      show_time(&mytime);
    }
    return;
  }
  if (cause == 16)
  {
    volatile int *timer = (volatile int *)0x04000020;
    int status = *timer;
    int TO = status & 0x1;

    if (TO == 1)
    {
      *timer = 1; // reset TO flag
      timeoutcount++;
      if (timeoutcount == 10)
      {
        timeoutcount = 0;
        show_time(&mytime);
        time2string(textstring, mytime);
        tick(&mytime);
      }
      return;
    }
  }
}

/* Init both timer and switch interrupts */
void labinit(void)
{
  /* Timer setup */
  volatile int *timer = (volatile int *)0x04000020;
  int periodL = 0xC6C0;
  int periodH = 0x002D;
  *(timer + 2) = periodL;
  *(timer + 3) = periodH;
  *(timer + 1) = 0x7; // start, continuous, ITO

  /* Switch (PIO) setup */
  volatile int *edgecapture = (volatile int *)(0x04000010 + 0xC);   // offset 3*4
  volatile int *interruptmask = (volatile int *)(0x04000010 + 0x8); // offset 2*4
  *edgecapture = 0xFFFFFFFF;                                        // clear old edges
  *interruptmask = 0x2;                                             // enable only SW1 (bit1)

  enable_interrupt();
}

int main(void)
{
  labinit();
  while (1)
  {
    print("Prime: ");
    prime = nextprime(prime);
    print_dec(prime);
    print("\n");
  }
}
