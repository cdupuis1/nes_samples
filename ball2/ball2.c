
//
// Bounces four balls around the screen
//
#include <stdint.h>
#include <stdlib.h>
#include "neslib.h"

// Palette data
unsigned char ball2_pal[] = {
  // Background palette data..all black
  0xd, 0xd, 0xd, 0xd,
  0xd, 0xd, 0xd, 0xd,
  0xd, 0xd, 0xd, 0xd,
  0xd, 0xd, 0xd, 0xd,

  // Sprite palette data
  0x0d, 0x12, 0x0d, 0x0d,
  0x0d, 0x24, 0x0d, 0x0d,
  0x0d, 0x29, 0x0d, 0x0d,
  0x0d, 0x27, 0x0d, 0x0d
};

unsigned int ball2_pal_len = 32;

#define NUM_SPRITES   4
#define DELTA_PX 10

// Keep track of the x position of each sprite
uint8_t ball_x_pos[NUM_SPRITES];

// Keep track of the y position of each sprite
uint8_t ball_y_pos[NUM_SPRITES];

// Keep track of how much left or right a ball is moving
int8_t ball_x_delta[NUM_SPRITES];

// Keep track of how much up or down a ball is moving
int8_t ball_y_delta[NUM_SPRITES];

// Initialize the NES to display our sprites
void setup_graphics()
{
  // Clear sprite memory
  oam_clear();

  // Set the palette data 
  pal_all(ball2_pal);

  // Turn the picture unit on
  ppu_on_all();
}

// main function, run after console reset
void main(void) {
  char oam_id;
  int i;

  setup_graphics();

  // Set initial positions and deltas
  for (i = 0; i < NUM_SPRITES; i++) {
    ball_x_pos[i] = rand();
    ball_y_pos[i] = rand();
    ball_x_delta[i] = DELTA_PX;
    ball_y_delta[i] = DELTA_PX;
  }

  // "Game" logic
  while (1) {
    // We always start with our offset into the OAM data at 0 for the first sprite
    oam_id = 0;
    
    for (i = 0; i < NUM_SPRITES; i++) {
      // oam_spr returns the next offset in OAM for the next sprite
      oam_id = oam_spr(ball_x_pos[i], ball_y_pos[i], i, i, oam_id);

      // Move the ball horizontally for the next scan
      ball_x_pos[i] += ball_x_delta[i];

      // Check for screen collision along the x axis
      if (ball_x_pos[i] <= (0 + DELTA_PX)) {
        ball_x_delta[i] = 10;
      } else if (ball_x_pos[i] >= (255 - DELTA_PX)) {
        ball_x_delta[i] = -10;
      }

      // Move the ball vertically for the next scan
      ball_y_pos[i] += ball_y_delta[i];

      // Check for screen collision along the y axis
      if (ball_y_pos[i] <= (0 + DELTA_PX)) {
        ball_y_delta[i] = 10;
      } else if (ball_y_pos[i] >= (240 - DELTA_PX)) {
        ball_y_delta[i] = -10;
      }
    }

    // Wait for next frame to be displayed
    ppu_wait_frame();
  }
}
