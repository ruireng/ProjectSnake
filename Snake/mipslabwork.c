/* mipslabwork.c

   This file written 2015 by F Lundevall
   Updated 2017-04-21 by F Lundevall

   This file should be changed by YOU! So you must
   add comment(s) here with your name(s) and date(s):

   This file modified 2017-04-31 by Ture Teknolog 

   For copyright and licensing, see file COPYING */

#include <stdio.h>
#include <stdint.h>   /* Declarations of uint_32 and the like */
#include <pic32mx.h>  /* Declarations of system-specific addresses etc */
#include "mipslab.h"  /* Declarations for these labs */

/* Definitions for different gamestates */
#define INTRO 0
#define MENU 1
#define GAME 2
#define GAMEOVER 3
#define CONTROLS1 4
#define CONTROLS2 5
#define CONTROLS3 6
#define CREDITS1 7
#define CREDITS2 8
#define EASTEREGG 9

/* Definitions for different directions */
#define EAST 1
#define NORTH 2
#define SOUTH 3
#define WEST 4

int timeoutcount = 0;
int ranX = 0;
int ranY = 0;
int gamestate = 0;
int score = 0;
int notHoldingDown = 0;

/* Global variables for making the game faster while progressing */
int gamespeed = 0;
int checkpoint1 = 0;
int checkpoint2 = 0;
int checkpoint3 = 0;
int checkpoint4 = 0;

/* Two scrambled arrays containing integers 1-32 */
int randomX[] = {
  19, 5, 18, 9, 20, 7, 31, 3,
  1, 10, 15, 17, 22, 16, 13, 29,
  24, 23, 21, 32, 4, 30, 14, 28,
  8, 25, 27, 2, 6, 11, 12, 26
};

int randomY[] = {
  19, 2, 11, 16, 28, 1, 27, 12,
  10, 14, 20, 31, 23, 29, 7, 32,
  4, 18, 15, 6, 8, 22, 5, 25,
  9, 3, 24, 30, 26, 17, 13, 21
};

/* This function written 2022-07-15 by Erik Sirborg */
/* This function modified 2022-07-16 by Roy Liu */
/* A function that returns a pseudo-random number between 1-32 */
int random_x(int timeX)
{
  return randomX[timeX];
}

/* This function written 2022-07-18 by Roy Liu */
/* A function that returns a pseudo-random number between 1-32 */
int random_y(int timeY)
{
  return randomY[timeY];
}

/* myPow because the pow function from <math.h> does not work for some reason */
/* This function written 2022-07-09 by Roy Liu */
/* A standard to-the-power-of function */
int my_pow(int num1, int num2)
{
  int counter = 1;
  int powVal = num1;
  if(num2 == 0)
  {
    return 1;
  }
  else
  {
    while(counter < num2)
    {
      powVal = powVal * num1;
      counter++;
    }
    return powVal;
  }
}

/* This function written 2022-07-12 by Erik Sirborg */
/* A function that checks which button is pressed. Only one button can be pressed at once */
int buttons()
{
  // button 4
  if(getbtns() == 0x4)
  {
    return 4;
  }

  // button 3
  if(getbtns() == 0x2)
  {
    return 3;
  }

  // button 2
  if(getbtns() == 0x1)
  {
    return 2;
  }

  // button 1
  if(getbtnone())
  {
    return 1;
  }

  // no input
  else
  {
    return 0;
  }
}

struct segment
{
  int x;
  int y;
  int rX;
  int rY;
};

struct snake
{
  int x;
  int y;
  int rX;                                             // "recentX" - last x-coordinate
  int rY;
  int direction;                                      // starts with direction = east
  int length;
  struct segment body[50];
};

struct snake player;

/* This function written 2022-08-04 by Roy Liu */
/* A function that initiates the snake's values */
void snake_init(struct snake *s)
{
  s -> x = 15;
  s -> y = 15;
  s -> rX = 0;
  s -> rY = 0;
  s -> direction = EAST;
  s -> length = 0;
}

/* This function written 2022-07-15 by Roy Liu */
/* A function that sets the snake's direction depending on which button is pressed */
void set_direction(struct snake *s)
{
  if((buttons() == 4) && !(s -> direction == EAST))
  {
    s -> direction = 4;                              // west
  }

  if((buttons() == 3) && !(s -> direction == NORTH))
  {
    s -> direction = 3;                              // south
  }

  if((buttons() == 2) && !(s -> direction == SOUTH))
  {
    s -> direction = 2;                              // north
  }

  if((buttons() == 1) && !(s -> direction == WEST))
  {
    s -> direction = 1;                              // east
  }
}

/* This function written 2022-07-15 by Erik Sirborg */
/* This function modified 2022-08-04 by Roy Liu */
/* A function that moves the snake's head depending on its direction. Also remembers its last coordinates */
void move_snake(struct snake *s)
{
  switch(s -> direction)
  {
    case WEST:
    s -> rX = s -> x;
    s -> rY = s -> y;
    s -> x--;
    break;

    case SOUTH:
    s -> rX = s -> x;
    s -> rY = s -> y;
    s -> y++;
    break;

    case NORTH:
    s -> rX = s -> x;
    s -> rY = s -> y;
    s -> y--;
    break;

    case EAST:
    s -> rX = s -> x;
    s -> rY = s -> y;
    s -> x++;
    break;
  }
}

/* This function written 2022-08-04 by Roy Liu */
/* A function that adds a segment to the snake */
void add_segment(struct snake *s)
{
  if(s -> length == 1)
  {
    s -> body[0].x = s -> rX;
    s -> body[0].y = s -> rY;
  }
  else
  {
    int i = s -> length - 1;
    s -> body[i].x = s -> body[i - 1].rX;
    s -> body[i].y = s -> body[i - 1].rY;
  }
}

/* This function written 2022-08-04 by Erik Sirborg & Roy Liu */
/* A function that updates the rest of the snake's body to follow the head */
void update_body(struct snake *s)
{
  if(s -> length != 0)
  {
    int i = 1;
    s -> body[0].rX = s -> body[0].x;
    s -> body[0].rY = s -> body[0].y;
    s -> body[0].x = s -> rX;
    s -> body[0].y = s -> rY;
    while(i <= s -> length)
    {
      s -> body[i].rX = s -> body[i].x;
      s -> body[i].rY = s -> body[i].y;
      s -> body[i].x = s -> body[i - 1].rX;
      s -> body[i].y = s -> body[i - 1].rY;
      i++;
    }
  }
}

/* This function written 2022-07-11 by Roy Liu */
/* A function that fills a pixel on the gameboard */
void set_pixel(int x, int y, uint8_t *gameboard)
{
  int gameIndex = ((y - 1) / 8) * 32 + x - 1;         // calculations for the index on the gameboard
  int gameValue = my_pow(2, (y - 1) % 8);             // calculations for the value on the index
  gameboard[gameIndex] = (gameboard[gameIndex]) | ((uint8_t) gameValue);         // set the pixels
}

/* This function written 2022-07-18 by Roy Liu */
/* A function that places the snake on the gameboard */
void set_snake(struct snake *s, uint8_t *gameboard)
{
  set_pixel(s -> x, s -> y, gameboard);
  if(s -> length > 0)
  {
    int i = 0;
    while(i < s -> length)
    {
      set_pixel(s -> body[i].x, s -> body[i].y, gameboard);
      i++;
    }
  }
}

struct apple
{
  int x;
  int y;
};

struct apple target;

/* This function written 2022-07-11 in collaboration with Mostafa Aziz Zuher */
/* A function that places the apple on the gameboard */
void set_apple(struct apple *a, uint8_t *gameboard)
{
  set_pixel(a -> x, a -> y, gameboard);
}

/* This function written 2022-07-15 by Erik Sirborg */
/* This function modified 2022-07-18 by Roy Liu */
/* A function that gives the apple random coordinates */
void create_apple(struct apple *a)
{
  a -> x = random_x(ranX);
  a -> y = random_y(ranY);
}

/* This function written 2022-07-18 by Erik Sirborg */
/* A function that checks if the player collides with the apple. If so, increments score and adds a body segment */
void apple_collision(struct snake *s, struct apple *a)
{
  if((s -> x == a -> x) && (s -> y == a -> y))
  {
    create_apple(a);
    score++;
    s -> length++;
    if(!(s -> length >= 50))
      add_segment(s);
  }
}

/* This function written 2022-08-01 by Erik Sirborg */
/* A function that checks if the player is out of bounds */
int wall_collision(struct snake *s)
{
  if((s -> x < 1) || (s -> x > 32) || (s -> y < 1) || (s -> y > 32))
  {
    return 1;
  }

  return 0;
}

/* This function written 2022-08-04 by Roy Liu */
/* A function that checks if the player collides with the rest of the body */
int body_collision(struct snake *s)
{
  if(s -> length != 0)
  {
    int i = 0;
    while(i <= s -> length)
    {
      if((s -> x == s -> body[i].x) && (s -> y == s -> body[i].y))
      {
        return 1;
      }
      i++;
    }
  }

  return 0;
}

/* This function written 2022-07-12 by Erik Sirborg */
/* A function that clears the entire gameboard */
void clear_board(uint8_t *gameboard)
{
  int counter = 0;
  while(counter < 128)
  {
    gameboard[counter] = 0;
    counter++;
  }
}

/* This function written 2022-07-18 by Roy Liu */
/* A function that displays the intro screen */
void display_intro()
{
  display_string(0, "SNAKE");
  display_string(1, "Press any");
  display_string(2, "button to");
  display_string(3, "start!");
  display_image(96, intro_icon);
}

/* This function written 2022-07-18 by Roy Liu */
/* A function that displays the menu screen */
void display_menu()
{
  display_string(0, "BTN1 - Play");
  display_string(1, "BTN2 - Controls");
  display_string(2, "BTN3 - Credits");
  display_string(3, "BTN4 - Back");
}

/* This function written 2022-08-01 by Roy Liu */
/* A function that displays the gameboard */
void display_game()
{
  display_string(0, "");
  display_string(1, "Score:");
  display_string(2, itoaconv(score));
  display_string(3, "");
  display_image(96, gameboard);
}

/* This function written 2022-08-01 by Roy Liu */
/* A function that displays the game over screen */
void display_gameover()
{
  display_string(0, "Game over");
  display_string(1, "Your score:");
  display_string(2, itoaconv(score));
  display_string(3, "BTN4 - Menu");
}

/* This function written 2022-07-19 by Roy Liu */
/* A function that displays controls and instructions */
void display_controls1()
{
  display_string(0, "BTN1: Right");
  display_string(1, "BTN2: Up");
  display_string(2, "BTN3: Down");
  display_string(3, "BTN4 - Next page");
}

/* This function written 2022-08-08 by Roy Liu */
/* A function that displays controls and instructions */
void display_controls2()
{
  display_string(0, "BTN4: Left");
  display_string(1, "Apples increase");
  display_string(2, "length & score");
  display_string(3, "BTN4 - Next page");
}

/* This function written 2022-08-08 by Roy Liu */
/* A function that displays controls and instructions */
void display_controls3()
{
  display_string(0, "Watch out for");
  display_string(1, "the snake body");
  display_string(2, "and the wall");
  display_string(3, "BTN4 - Menu");
}

/* This function written 2022-08-01 by Roy Liu */
/* A function that displays the credits */
void display_credits1()
{
  display_string(0, "IS1200 - SNAKE");
  display_string(1, "Developed by");
  display_string(2, "Roy L & Erik S");
  display_string(3, "BTN4 - Next page");
}

/* This function written 2022-08-08 by Roy Liu */
/* A function that displays the credits */
void display_credits2()
{
  display_string(0, "In collab with");
  display_string(1, "Mostafa A Z");
  display_string(2, "Art by Dave L");
  display_string(3, "BTN4 - Menu");
}

/* This function written 2022-08-08 by Roy Liu */
/* A function that displays an easter egg if the score meets the requirement */
void display_easter_egg()
{
  display_string(0, "Easter egg");
  display_string(1, "Your score:");
  display_string(2, "18");
  display_string(3, "BTN4 - Menu");
  display_image(96, easter_egg_icon);
}

/* This function written 2022-08-06 by Erik Sirborg */
/* This function modified 2022-08-08 by Roy Liu */
/* A function containing all the game states */
void game_state()
{
  switch(gamestate)
  {
    case INTRO:
    display_intro();
    if(buttons() == 0)                                // this part checks if any buttons are held down
      notHoldingDown = 1;                             // if not, 
    if(notHoldingDown == 1)                           // you are allowed to continue selecting options
    {                                                 // this is so that not all gamestates accidentally get skipped while just
      if(buttons())                                   // holding down one button
      {
        notHoldingDown = 0;                           // resets the button checker before changing gamestate
        gamestate = MENU;
        break;
      }
    }
    break;

    case MENU:
    display_menu();
    if(buttons() == 0)
      notHoldingDown = 1;
    if(notHoldingDown == 1)
    {
      switch(buttons())
      {
        case 1:
        snake_init(&player);
        create_apple(&target);
        gamespeed = 3;
        notHoldingDown = 0;
        gamestate = GAME;
        break;

        case 2:
        notHoldingDown = 0; 
        gamestate = CONTROLS1;
        break;

        case 3:
        notHoldingDown = 0;
        gamestate = CREDITS1;
        break;

        case 4:
        notHoldingDown = 0;
        gamestate = INTRO;
        break;
      }
    }
    break;

    case GAME:
    clear_board(gameboard);
    move_snake(&player);
    update_body(&player);
    apple_collision(&player, &target);
    if(wall_collision(&player) || body_collision(&player))
    { 
      if(score == 18)
      {
        checkpoint1 = 0;
        checkpoint2 = 0;
        gamespeed = 0;
        gamestate = EASTEREGG;
        break;
      }
      checkpoint1 = 0;
      checkpoint2 = 0;
      checkpoint3 = 0;
      checkpoint4 = 0;
      gamespeed = 0;
      gamestate = GAMEOVER;
      break;
    }
    if((score == 3) && (checkpoint1 == 0))
    {
      gamespeed--;
      checkpoint1 = 1;
    }
    if((score == 10) && (checkpoint2 == 0))
    {
      gamespeed--;
      checkpoint2 = 1;
    }
    if((score == 20) && (checkpoint3 == 0))
    {
      gamespeed--;
      checkpoint3 = 1;
    }
    if((score == 30) && (checkpoint4 == 0))
    {
      gamespeed--;
      checkpoint4 = 1;
    }
    set_snake(&player, gameboard);
    set_apple(&target, gameboard);
    display_game();
    break;

    case GAMEOVER:
    display_gameover();
    if(buttons() == 0)
      notHoldingDown = 1;
    if(notHoldingDown == 1)
    {
      if(buttons() == 4)
      {
        score = 0;
        notHoldingDown = 0;
        gamestate = MENU;
        break;
      }
    }
    break;

    case CONTROLS1:
    display_controls1();
    if(buttons() == 0)
      notHoldingDown = 1;
    if(notHoldingDown == 1)
    {
      if(buttons() == 4)
      {
        notHoldingDown = 0;
        gamestate = CONTROLS2;
        break;
      }
    }
    break;

    case CONTROLS2:
    display_controls2();
    if(buttons() == 0)
      notHoldingDown = 1;
    if(notHoldingDown == 1)
    {
      if(buttons() == 4)
      {
        notHoldingDown = 0;
        gamestate = CONTROLS3;
        break;
      }
    }
    break;

    case CONTROLS3:
    display_controls3();
    if(buttons() == 0)
      notHoldingDown = 1;
    if(notHoldingDown == 1)
    {
      if(buttons() == 4)
      {
        notHoldingDown = 0;
        gamestate = MENU;
        break;
      }
    }
    break;

    case CREDITS1:
    display_credits1();
    if(buttons() == 0)
      notHoldingDown = 1;
    if(notHoldingDown == 1)
    {
      if(buttons() == 4)
      {
        notHoldingDown = 0;
        gamestate = CREDITS2;
        break;
      }
    }
    break;

    case CREDITS2:
    display_credits2();
    if(buttons() == 0)
      notHoldingDown = 1;
    if(notHoldingDown == 1)
    {
      if(buttons() == 4)
      {
        notHoldingDown = 0;
        gamestate = MENU;
        break;
      }
    }
    break;

    case EASTEREGG:
    display_easter_egg();
    if(buttons() == 0)
      notHoldingDown = 1;
    if(notHoldingDown == 1)
    {
      if(buttons() == 4)
      {
        notHoldingDown = 0;
        score = 0;
        gamestate = MENU;
        break;
      }
    }
    break;
  }
}

/* Interrupt Service Routine */
void user_isr()
{
  timeoutcount++;
  ranX++;
  ranY = ranY + 3;

  if(ranX > 31)
  {
    ranX = 0;
  }
  if(ranY > 31)
  {
    ranY = ranY % 32;
  }

  if(timeoutcount == 2 + gamespeed)
  {
    display_update();
    game_state();
    timeoutcount = 0;                                 // reset timeoutcount
  }
  IFSCLR(0) = 0x100;                                  // clear the timer2 interrupt flag
}

/* Lab-specific initialization goes here */
void labinit()
{
  TRISESET = 0x00;                                    // set all bits to 0 (output)
  PORTE = 0x00;                                       // initialize start value to 0
  TRISDSET = 0xfe0;                                   // initialize bits 5 - 11 to 1 (input)
  TRISFSET = 0x2;                                     // initialize the bit controlling BTN1 to 1 (input)
  
  // Assignment 2
  T2CON = 0x0;                                        // reset timer2
  T2CONSET = 0x70;                                    // set prescale on timer2 to 1:256
  PR2 = ((80000000 / 256) / 10);                      // frequency = 80 M / 256 / 10 = 31250
  TMR2 = 0x0;                                         // initialize timer2 to 0
  T2CONSET = 0x8000;                                  // start timer2

  IECSET(0) = 0x100;                                  // enable timer2
  IPCSET(2) = 0xf8;                                   // set highest priority & sub-priority for timer2

  display_string(0, "");                              // wipe the screen at startup
  display_string(1, "");
  display_string(2, "");
  display_string(3, "");
  display_update();

  enable_interrupt();
  return;
}

/* This function is called repetitively from the main program */
void labwork()
{
  PORTE = score;
  if(gamestate == GAME)
    set_direction(&player);
}