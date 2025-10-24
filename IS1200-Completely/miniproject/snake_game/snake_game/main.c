#include "snake-lib.h"

int main(void)
{
    init_switch_interrupt(); 
    while (1)
    {
        playGame(); // run the main game

        // wait for button to restart game
        while (1)
        {
            if (get_btn())
            {
                while (get_btn()); // wait until released
                break; // restart
            }
        }
    }
}

