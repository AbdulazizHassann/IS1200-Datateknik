====================================
     IS1200 Mini Project - Snake Game
        
====================================


------------------------------------
1. Description
------------------------------------
This is our mini project for the DTEK-V board.  
We created a simple Snake game shown on the VGA screen.  
The player controls the snake using the toggle switches, 
and the score is displayed on the 7-segment display.

Controls:
 - SW0 = turn right
 - SW1 = turn left
 - Push button = restart game

------------------------------------
2. How to Compile and Run
------------------------------------
1. Open the DTEK-V environment in linux terminal.  
2. Compile with:
       make
3. Upload to the board:
       make upload
4. The game starts automatically on VGA.

------------------------------------
3. Notes
------------------------------------
- VGA draws the game map.  
- Score is updated on the 7-segment display.  
- Game restarts with push button(KEY1).
