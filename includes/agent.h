#pragma once
#include <SDL.h>
#include <stdlib.h>

#include "consts.h"
#include "dna.h"
#include "pipe.h"

typedef struct Agent
{
    DNA dna;
    AABB aabb;
    float velY;
    int dead;
    float fitness;

    float jump_delay, jump_timer;
} Agent;

Agent Agent_new();
Agent Agent_newFromDna(DNA dna);
void Agent_Update(Agent *agent, float dt, Pipe closest_pipe,int global_score);
int Agent_FeedForward(Agent agent, Pipe pipe);
void Agent_Render(SDL_Renderer *renderer, Agent agent);
int Agent_getBest(Agent* pop);