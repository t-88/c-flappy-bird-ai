#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>

#include "neuralNetworkRender.h"
#include "consts.h"
#include "dna.h"
#include "utils.h"
#include "pipe.h"
#include "agent.h"



float dt;
int closest_pipe_index;
int global_score;

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
    TTF_Init();
    srand(time(NULL));

    SDL_Event event;
    SDL_Window *window;
    SDL_Renderer *renderer;
    SDL_CreateWindowAndRenderer(WIDTH, HEIGHT, SDL_WINDOW_SHOWN, &window, &renderer);
    SDL_SetRenderDrawBlendMode(renderer,SDL_BLENDMODE_BLEND);


    Pipe pipes[4];
    closest_pipe_index = 0;
    Pipes_Init(pipes, 4);
    Agent *pop = calloc(POP_COUNT, sizeof(Agent));

    for (size_t i = 0; i < POP_COUNT; i++)
        pop[i] = Agent_new();

    NN_Renderer nn_renderer;
    nn_renderer.screen_width = WIDTH;
    nn_renderer.screen_height =  HEIGHT; 
    nn_renderer.w = 120;
    nn_renderer.h = 150;  
    nn_renderer.x = WIDTH - nn_renderer.w;
    nn_renderer.y = HEIGHT - nn_renderer.h;  
    NN nn;
    nn.inputs_count = DNA_INPUTS;
    nn.outputs_count = DNA_OUTPUTS;
    nn.hidden_count =  DNA_HIDDEN;




    TTF_Font* font = TTF_OpenFont("assets/FFFFORWA.TTF",32);
    SDL_Color text_color  = {0,0,0};
    SDL_Surface* text_surface = TTF_RenderText_Solid(font,"0",text_color);
    SDL_Texture* text_texture = SDL_CreateTextureFromSurface(renderer,text_surface);
    SDL_Rect text_rect = {WIDTH/2 - 10, 20, 32,32};
    
    
    SDL_Texture* ground;
    ground = IMG_LoadTexture(renderer,"assets/Sprite-0002.png");
    SDL_Texture* grass_bg;
    grass_bg = IMG_LoadTexture(renderer,"assets/grass.png");
    SDL_Rect grass_bg_rect = (SDL_Rect) {0 , HEIGHT - 110 , 1200  , 51 * 2};
    SDL_Texture* pipe_head;
    pipe_head = IMG_LoadTexture(renderer,"assets/pipe-head.png");
    SDL_Texture* pipe_body;
    pipe_body = IMG_LoadTexture(renderer,"assets/pipe-body.png");
    SDL_Texture* clouds;
    clouds = IMG_LoadTexture(renderer,"assets/clouds.png");
    SDL_Rect clouds_rect = (SDL_Rect) {0 , HEIGHT - 170 , 1200  , 51 * 2};

    SDL_Texture* bird = IMG_LoadTexture(renderer,"assets/bird.png");

    
    SDL_FRect ground_rect[2];
    for(int i = 0; i < 2; i++)
        ground_rect[i] = (SDL_FRect) {i * 600, HEIGHT - 50, 600 , 50 };

    int running = 1;
    while (running)
    {
        Uint64 start = SDL_GetPerformanceCounter();
        Uint32 start_frame = SDL_GetTicks();

        while (SDL_PollEvent(&event))
            if (event.type == SDL_QUIT)
                running = 0;

        int score_updated = Pipes_Update(pipes, 4,dt,&global_score,&closest_pipe_index);
        if(score_updated) {
            int lenght = (int)SDL_log10(global_score) * sizeof(char);
            char str[lenght];
            sprintf(str,"%d",global_score);

            text_surface = TTF_RenderText_Solid(font,str,text_color);
            text_texture = SDL_CreateTextureFromSurface(renderer,text_surface);
        }
        
        
        int pop_count = POP_COUNT;
        for (size_t i = 0; i < POP_COUNT; i++) {
            if (!pop[i].dead) {
                Agent_Update(&pop[i], dt, pipes[closest_pipe_index],global_score);
            }
            else {
                pop_count--;
            }
        }

        if (pop_count <= 0) {
            global_score = 0;
            closest_pipe_index = 0;
            pop = Pop_Reset(pop);
            Pipes_Init(pipes, 4);

            text_surface = TTF_RenderText_Solid(font,"0",text_color);
            text_texture = SDL_CreateTextureFromSurface(renderer,text_surface);

            continue;
        }

        int index = Agent_getBest(pop);
        nn.dna = pop[index].dna;
        nn.inputs[0] = pop[index].velY / AGENT_MAX_VELY;
        nn.inputs[1] = pop[index].aabb.y / HEIGHT;
        nn.inputs[2] = pipes[closest_pipe_index].aabb.x / WIDTH;
        nn.inputs[3] = (pipes[closest_pipe_index].aabb.y + pipes[closest_pipe_index].aabb.h + PIPE_GAP) / HEIGHT;



        SDL_RenderCopy(renderer,clouds,NULL,&clouds_rect);
        SDL_RenderCopy(renderer,grass_bg,NULL,&grass_bg_rect);
       
       
       
        Pipes_Render(renderer, pipes, 4,pipe_head,pipe_body);
        for (size_t i = 0; i < POP_COUNT; i++)
            if (!pop[i].dead)
                Agent_Render(renderer, pop[i],bird);
        
        for (int i = 0; i < 2; i++) {
            ground_rect[i].x -= PIPE_SPEED * dt * 2;
            if(ground_rect[i].x + ground_rect[i].w <= 0) {
                ground_rect[i].x = WIDTH;
            }
        }


        for (int i = 0; i < 2; i++) {
            SDL_RenderCopyF(renderer, ground, NULL, &ground_rect[i]);
        }
        
        NN_Render(renderer,nn_renderer,nn);


        SDL_RenderCopy(renderer,text_texture,NULL,&text_rect);

        SDL_RenderPresent(renderer);
        SDL_SetRenderDrawColor(renderer,95,205,228,255);
        SDL_RenderClear(renderer);

        Uint64 elapsedCount = SDL_GetPerformanceCounter() - start;
        if (SDL_GetPerformanceFrequency() / FPS > elapsedCount)
        {
            Uint32 remainingMilliseconds = (SDL_GetPerformanceFrequency() / FPS - elapsedCount) * 1000 / SDL_GetPerformanceFrequency();
            SDL_Delay(remainingMilliseconds);
            dt = (SDL_GetTicks() - start_frame) / 1000.0f * SPEED_UP;
        }
    }



    SDL_FreeSurface(text_surface);
    SDL_DestroyTexture(bird);
    SDL_DestroyTexture(ground);
    SDL_DestroyTexture(grass_bg);
    SDL_DestroyTexture(pipe_head);
    SDL_DestroyTexture(pipe_body);
    SDL_DestroyTexture(clouds);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}