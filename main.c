#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include "neuralNetworkRender.h"
#include "consts.h"
#include "dna.h"
#include "utils.h"
#include "pipe.h"
#include "agent.h"



float dt;
int closest_pipe_index;
int global_score;
int holdingJump = 0;



Agent* Pop_Reset(Agent *pop)
{
    DNA gen_bestDna = pop[0].dna;
    DNA gen_secondBestDna = pop[0].dna;

    float gen_bestFitness = SDL_MIN_SINT32;
    int tmp = 0;
    for (size_t i = 0; i < POP_COUNT; i++)
    {
        if (pop[i].fitness > gen_bestFitness)
        {
            gen_bestFitness = pop[i].fitness;
            gen_bestDna = pop[i].dna;
        }
        else if (pop[i].fitness > pop[tmp].fitness)
        {
            gen_secondBestDna = pop[i].dna;
            tmp = i;
        }
    }

    DNA_Print(gen_bestDna);
    Agent *new_pop = calloc(POP_COUNT, sizeof(Agent));

    for (size_t i = 0; i < (int)(POP_COUNT / 3); i++){
        new_pop[i] = Agent_newFromDna(DNA_Mutate(gen_bestDna));
    }
    for (size_t i = (int)(POP_COUNT / 3); i < (int)(2*POP_COUNT / 3); i++){
        new_pop[i] = Agent_newFromDna(DNA_Breed(DNA_Mutate(gen_bestDna), DNA_Mutate(gen_secondBestDna)));
    }
    for (size_t i = (int)(2 * POP_COUNT / 3); i < POP_COUNT; i++) {
    new_pop[i] = Agent_new();
    }

    free(pop);
    return new_pop;
}

int main(void)
{
    SDL_Init(SDL_INIT_VIDEO);
    SDL_Event event;
    SDL_Window *window;
    SDL_Renderer *renderer;

    srand(time(NULL));

    Pipe pipes[4];
    closest_pipe_index = 0;
    Pipes_Init(pipes, 4);

    Agent *pop = calloc(POP_COUNT, sizeof(Agent));

    for (size_t i = 0; i < POP_COUNT; i++)
        pop[i] = Agent_new();

    NN_Renderer nn_renderer;
    nn_renderer.screen_width = WIDTH;
    nn_renderer.screen_height =  HEIGHT; 
    nn_renderer.w = 200;
    nn_renderer.h = 200;  
    nn_renderer.x = WIDTH - nn_renderer.w;
    nn_renderer.y = HEIGHT - nn_renderer.h;  
    NN nn;
    nn.inputs_count = DNA_INPUTS;
    nn.outputs_count = DNA_OUTPUTS;
    nn.hidden_count =  DNA_HIDDEN;




    SDL_CreateWindowAndRenderer(WIDTH, HEIGHT, SDL_WINDOW_SHOWN, &window, &renderer);
    int running = 1;

    SDL_Rect rect = {10, 10, 50, 50};
    
    
    
    SDL_Texture* ground;
    ground = IMG_LoadTexture(renderer,"assets/Sprite-0002.png");
    SDL_Texture* grass_bg;
    grass_bg = IMG_LoadTexture(renderer,"assets/grass.png");
    SDL_Texture* pipe_head;
    pipe_head = IMG_LoadTexture(renderer,"assets/pipe-head.png");
    SDL_Texture* pipe_body;
    pipe_body = IMG_LoadTexture(renderer,"assets/pipe-body.png");

    
    SDL_Rect grass_bg_rect = (SDL_Rect) {0 , HEIGHT - 110 , 1200  , 51 * 2};


    SDL_FRect ground_rect[2];
    for(int i = 0; i < 2; i++) {
        ground_rect[i] = (SDL_FRect) {i * 600, HEIGHT - 50, 600 , 50 };
    } 

    while (running)
    {
        Uint64 start = SDL_GetPerformanceCounter();
        Uint32 start_frame = SDL_GetTicks();

        while (SDL_PollEvent(&event))
            if (event.type == SDL_QUIT)
                running = 0;

        Pipes_Update(pipes, 4,dt,&global_score,&closest_pipe_index);
        int pop_count = POP_COUNT;
        for (size_t i = 0; i < POP_COUNT; i++)
        {
            if (!pop[i].dead) {
                Agent_Update(&pop[i], dt, pipes[closest_pipe_index],global_score);
            }
            else {
                pop_count--;
            }
        }

        // if (pop_count <= 0) {
            // global_score = 0;
            // closest_pipe_index = 0;
            // pop = Pop_Reset(pop);
            // Pipes_Init(pipes, 4);
            // continue;
        // }

        int index = Agent_getBest(pop);
        nn.dna = pop[index].dna;
        nn.inputs[0] = pop[index].velY / AGENT_MAX_VELY;
        nn.inputs[1] = pop[index].aabb.y / HEIGHT;
        nn.inputs[2] = pipes[closest_pipe_index].aabb.x / WIDTH;
        nn.inputs[3] = (pipes[closest_pipe_index].aabb.y + pipes[closest_pipe_index].aabb.h + PIPE_GAP) / HEIGHT;



        SDL_RenderCopy(renderer,grass_bg,NULL,&grass_bg_rect);
        Pipes_Render(renderer, pipes, 4,pipe_head,pipe_body);
        for (size_t i = 0; i < POP_COUNT; i++)
            if (!pop[i].dead)
                Agent_Render(renderer, pop[i]);
        
        for (int i = 0; i < 2; i++) {
            ground_rect[i].x -= PIPE_SPEED * dt * 2;
            if(ground_rect[i].x + ground_rect[i].w <= 0) {
                ground_rect[i].x = WIDTH;
            }
        }


        for (int i = 0; i < 2; i++) {
            SDL_RenderCopyF(renderer, ground, NULL, &ground_rect[i]);
        }
        
        // NN_Render(renderer,nn_renderer,nn);


        SDL_SetRenderDrawColor(renderer,95,205,228,255);
        SDL_RenderPresent(renderer);
        SDL_RenderClear(renderer);

        Uint64 elapsedCount = SDL_GetPerformanceCounter() - start;
        if (SDL_GetPerformanceFrequency() / FPS > elapsedCount)
        {
            Uint32 remainingMilliseconds = (SDL_GetPerformanceFrequency() / FPS - elapsedCount) * 1000 / SDL_GetPerformanceFrequency();
            SDL_Delay(remainingMilliseconds);
            dt = (SDL_GetTicks() - start_frame) / 1000.0f * SPEED_UP;
        }
    }

    SDL_DestroyTexture(ground);
    SDL_DestroyTexture(grass_bg);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}