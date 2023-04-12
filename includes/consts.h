#pragma once


#define WIDTH 600
#define HEIGHT 800
#define SPEED_UP 1

#define PIPE_GAP 250
#define PIPE_DISTANCE 300
#define PIPE_OFFSET_X WIDTH - 100
#define PIPE_SPEED 300 / SPEED_UP
#define PIPE_GAP_MARGIN 50

#define POP_COUNT 10
#define AGENT_START_X 50

#define DNA_INPUTS 4
#define DNA_HIDDEN 6
#define DNA_OUTPUTS 1
#define DNA_MUTATION_CHANCE 10
#define DNA_LR 0.1

#define GRAVITY 35 * SPEED_UP
#define AGENT_JUMP_FORCE 600 * SPEED_UP
#define AGENT_MAX_VELY 20 * SPEED_UP
#define AGENT_JUMP_DELAY 0.2 * SPEED_UP

#define FPS 60
#define TARGET_FPS 1


typedef struct AABB{
    float x, y, w, h;
} AABB;
