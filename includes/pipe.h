#pragma once
#include <SDL.h>
#include <stdlib.h>

#include "consts.h"


typedef struct Pipe
{
    AABB aabb;
    float height;
    int tookScore;
} Pipe;


void Pipes_Init(Pipe *pipes, int size);
void Pipes_Update(Pipe *pipes, int size,float dt,int* global_score,int* closest_pipe_index);
void Pipes_Render(SDL_Renderer *renderer, Pipe *pipes, int size);
