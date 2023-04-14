#include "pipe.h"


Pipe Pipe_init(int i) {
    Pipe pipe;
    
    pipe.height = map(random() % HEIGHT , 0 , HEIGHT, 40,HEIGHT - PIPE_GAP - 140);  
    pipe.aabb = (AABB){PIPE_OFFSET_X + (PIPE_DISTANCE + 60) * i, 0, 60, pipe.height};
    pipe.tookScore = 0;

    SDL_Log("%f",pipe.aabb.h);

    return pipe;

}

void Pipes_Init(Pipe *pipes, int size)
{
    for (size_t i = 0; i < size; i++) {
       pipes[i] = Pipe_init(i);
    }
}
int Pipes_Update(Pipe *pipes, int size,float dt,int* global_score,int* closest_pipe_index)
{
    int score_updated = 0;
    for (size_t i = 0; i < size; i++)
    {
        pipes[i].aabb.x -= PIPE_SPEED * dt * TARGET_FPS;
        if (pipes[i].aabb.x + pipes[i].aabb.w < 0) {
            pipes[i] = Pipe_init(i);
            pipes[i].aabb.x = PIPE_OFFSET_X + (PIPE_DISTANCE)*3 + pipes[i].aabb.w;
            if ((*closest_pipe_index) == i) {
                (*closest_pipe_index) = ((*closest_pipe_index) + 1) % size;
                (*global_score)++;
                score_updated = 1;
            }
        }
    }
    return score_updated;
}
void Pipes_Render(SDL_Renderer *renderer, Pipe *pipes, int size,SDL_Texture* pipe_head,SDL_Texture* pipe_body)
{
    for (size_t i = 0; i < size; i++) {
        SDL_Rect body_top = (SDL_Rect){pipes[i].aabb.x,0,pipes[i].aabb.w,pipes[i].aabb.h};
        SDL_Rect body_bottom = (SDL_Rect){pipes[i].aabb.x,pipes[i].aabb.y + pipes[i].aabb.h + PIPE_GAP,pipes[i].aabb.w,HEIGHT - (pipes[i].aabb.h + PIPE_GAP)};        
        SDL_RenderCopy(renderer,pipe_body,NULL,&body_top);
        SDL_RenderCopy(renderer,pipe_body,NULL,&body_bottom);

        SDL_Rect head_top = (SDL_Rect){pipes[i].aabb.x - 4,pipes[i].aabb.h - 32,pipes[i].aabb.w + 8,32};
        SDL_Rect head_bottom = (SDL_Rect){pipes[i].aabb.x - 4,pipes[i].aabb.y + pipes[i].aabb.h + PIPE_GAP,pipes[i].aabb.w + 8,32};
        SDL_RenderCopy(renderer,pipe_head,NULL,&head_top);
        SDL_RenderCopy(renderer,pipe_head,NULL,&head_bottom);
    }
}
