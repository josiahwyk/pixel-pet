// Nebelung cat sprite. Front view, sitting.
// '#' = lit pixel, '.' = dark. Edit the rows to change the cat.
#pragma once

const int SPRITE_W = 24;
const int SPRITE_H = 24;

// Body with tail tucked in
const char* const CAT_TAIL_IN[SPRITE_H] = {
  "........................",
  "..##.............##.....",
  "..#.#...........#.#.....",
  "..#..#.........#..#.....",
  "..#...#########...#.....",
  ".#.................#....",
  ".#.................#....",
  ".#.................#....",
  "##.................##...",
  ".#.................#....",
  "##.................##...",
  ".#.................#....",
  "#.#...............#.#...",
  ".#................#.....",
  "..#...............#..#..",
  "..#....#.....#....#.#.#.",
  "..#....#.....#....#.#.#.",
  "..#...............#.#.#.",
  "..#...............##..#.",
  "..#...............#..#..",
  "..#...............###...",
  "..#################.....",
  "........................",
  "........................",
};

// Body with tail swished out
const char* const CAT_TAIL_OUT[SPRITE_H] = {
  "........................",
  "..##.............##.....",
  "..#.#...........#.#.....",
  "..#..#.........#..#.....",
  "..#...#########...#.....",
  ".#.................#....",
  ".#.................#....",
  ".#.................#....",
  "##.................##...",
  ".#.................#....",
  "##.................##...",
  ".#.................#....",
  "#.#...............#.#...",
  ".#................#.....",
  "..#...............#...#.",
  "..#....#.....#....#..#.#",
  "..#....#.....#....#..#.#",
  "..#...............#.#.#.",
  "..#...............##..#.",
  "..#...............#..#..",
  "..#...............###...",
  "..#################.....",
  "........................",
  "........................",
};

// Faces are drawn on top of the body, at FACE_X, FACE_Y
const int FACE_W = 10;
const int FACE_H = 5;
const int FACE_X = 5;
const int FACE_Y = 7;

const char* const FACE_OPEN_ROWS[FACE_H] = {
  ".##....##.",
  ".##....##.",
  "..........",
  "...#.#.#..",
  "....#.#...",
};

const char* const FACE_BLINK_ROWS[FACE_H] = {
  "..........",
  ".##....##.",
  "..........",
  "...#.#.#..",
  "....#.#...",
};

const char* const FACE_HAPPY_ROWS[FACE_H] = {
  "..#....#..",
  ".#.#..#.#.",
  "..........",
  "...#.#.#..",
  "....#.#...",
};

const char* const FACE_HUNGRY_ROWS[FACE_H] = {
  ".##....##.",
  ".##....##.",
  "..........",
  ".....#....",
  "....#.#...",
};

// Eating: eyes shut, mouth open (alternates with the blink face)
const char* const FACE_CHEW_ROWS[FACE_H] = {
  "..........",
  ".##....##.",
  "..........",
  "....##....",
  "....##....",
};

// Small pictures, drawn at normal size (1 pixel = 1 screen pixel)
const int HEART_W = 7;
const int HEART_H = 6;
const char* const HEART_ROWS[HEART_H] = {
  ".##.##.",
  "#######",
  "#######",
  ".#####.",
  "..###..",
  "...#...",
};

const int BOWL_W = 16;
const int BOWL_H = 5;
const char* const BOWL_ROWS[BOWL_H] = {
  "################",
  ".#............#.",
  "..#..........#..",
  "...##########...",
  "................",
};

// White chest patch (a small bib), drawn on top of the body
const int PATCH_W = 5;
const int PATCH_H = 3;
const int PATCH_X = 8;
const int PATCH_Y = 13;
const char* const PATCH_ROWS[PATCH_H] = {
  "#####",
  ".###.",
  "..#..",
};

// Watching the butterfly: eyes look left or right
const char* const FACE_LOOK_L_ROWS[FACE_H] = {
  "##....##..",
  "##....##..",
  "..........",
  "...#.#.#..",
  "....#.#...",
};
const char* const FACE_LOOK_R_ROWS[FACE_H] = {
  "..##....##",
  "..##....##",
  "..........",
  "...#.#.#..",
  "....#.#...",
};

// Grooming: eyes shut, little tongue out
const char* const FACE_GROOM_ROWS[FACE_H] = {
  "..........",
  ".##....##.",
  "..........",
  "...#.#.#..",
  ".....#....",
};

// The two front legs (short lines low on the body): columns, and top row (2 rows tall)
const int LEG_LEFT_COL = 7;    // left of the screen
const int LEG_RIGHT_COL = 13;  // right of the screen
const int LEG_TOP = 15;

// A raised paw (drawn at cat size, 2x2 per pixel). Used for scratching.
const int PAW_W = 4;
const int PAW_H = 3;
const char* const PAW_ROWS[PAW_H] = {
  ".##.",
  "####",
  ".##.",
};

// Grooming: small paw rubbing the cheek, with the arm below it.
// Drawn at GROOM_X, GROOM_Y. Two positions: paw up and paw down.
const int GROOM_W = 4;
const int GROOM_H = 9;
const int GROOM_X = 3;
const int GROOM_Y = 7;
const char* const GROOM_UP_ROWS[GROOM_H] = {
  ".#.#",
  "####",
  "####",
  ".##.",
  ".#..",
  ".#..",
  ".#..",
  "#...",
  "#...",
};
const char* const GROOM_DOWN_ROWS[GROOM_H] = {
  "....",
  "....",
  ".#.#",
  "####",
  "####",
  ".##.",
  ".#..",
  ".#..",
  "#...",
};

// Butterfly, two wing positions (normal size)
const int FLY_W = 7;
const int FLY_H = 5;
const char* const FLY_OPEN_ROWS[FLY_H] = {
  "##...##",
  "###.###",
  ".##.##.",
  "###.###",
  ".#...#.",
};
const char* const FLY_SHUT_ROWS[FLY_H] = {
  ".......",
  "..#.#..",
  "..###..",
  "..#.#..",
  ".......",
};

// Brush for grooming the cat (normal size)
const int BRUSH_W = 10;
const int BRUSH_H = 4;
const char* const BRUSH_ROWS[BRUSH_H] = {
  "##########",
  "##########",
  "#.#.#.#.#.",
  "#.#.#.#.#.",
};
