#pragma once
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdlib.h>

#include "consts.h"
#include "utils.h"


typedef struct Pipe
{
    AABB aabb;
    float height;
    int tookScore;
} Pipe;


void Pipes_Init(Pipe *pipes, int size);
Pipe Pipe_init(int i);
void Pipes_Update(Pipe *pipes, int size,float dt,int* global_score,int* closest_pipe_index);
void Pipes_Render(SDL_Renderer *renderer, Pipe *pipes, int size,SDL_Texture* pipe_head,SDL_Texture* pipe_body);
