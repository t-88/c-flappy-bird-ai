#include "pipe.h"


void Pipes_Init(Pipe *pipes, int size)
{
    for (size_t i = 0; i < size; i++)
    {
        pipes[i].height = random() % (HEIGHT - PIPE_GAP_MARGIN - PIPE_GAP) + PIPE_GAP_MARGIN;
        pipes[i].aabb = (AABB){PIPE_OFFSET_X + (PIPE_DISTANCE + 60) * i, 0, 60, pipes[i].height};
        pipes[i].tookScore = 0;
    }
}
void Pipes_Update(Pipe *pipes, int size,float dt,int* global_score,int* closest_pipe_index)
{
    for (size_t i = 0; i < size; i++)
    {
        pipes[i].aabb.x -= PIPE_SPEED * dt * TARGET_FPS;
        if (pipes[i].aabb.x + pipes[i].aabb.w < 0)
        {
            pipes[i].aabb.x = PIPE_OFFSET_X + (PIPE_DISTANCE)*3 + pipes[i].aabb.w;
            if ((*closest_pipe_index) == i)
            {
                (*closest_pipe_index) = ((*closest_pipe_index) + 1) % size;

                (*global_score)++;
            }
        }
    }
}
void Pipes_Render(SDL_Renderer *renderer, Pipe *pipes, int size,SDL_Texture* pipe_head,SDL_Texture* pipe_body)
{
    SDL_Rect *rects;
    rects = calloc(size * 2, sizeof(SDL_Rect));

    for (size_t i = 0; i < size; i++)
    {
    
        rects[i] = (SDL_Rect){pipes[i].aabb.x, pipes[i].aabb.y, pipes[i].aabb.w, pipes[i].aabb.h};
        rects[4 + i] = (SDL_Rect){
            pipes[i].aabb.x,
            pipes[i].aabb.y + pipes[i].height + PIPE_GAP,
            pipes[i].aabb.w,
            HEIGHT - (pipes[i].aabb.h + PIPE_GAP)
            };
        // SDL_Rect dest = (SDL_Rect){pipes[i].aabb.y + pipes[i].height + PIPE_GAP,pipes[i].aabb.w,HEIGHT - (pipes[i].aabb.h + PIPE_GAP)};

    }


    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderFillRects(renderer, rects, size * 2);

    for (size_t i = 0; i < size; i++) {

        SDL_Rect body_top = (SDL_Rect){rects[i].x,0,rects[i].w,rects[i].h};
        SDL_Rect body_bottom = (SDL_Rect){rects[i].x,rects[i].y + rects[i].h + PIPE_GAP,rects[i].w,HEIGHT - (rects[i].h + PIPE_GAP)};        
        SDL_RenderCopy(renderer,pipe_body,NULL,&body_top);
        SDL_RenderCopy(renderer,pipe_body,NULL,&body_bottom);

        SDL_Rect head_top = (SDL_Rect){rects[i].x - 4,rects[i].h,rects[i].w + 8,32};
        SDL_Rect head_bottom = (SDL_Rect){rects[i].x - 4,rects[i].y + rects[i].h + PIPE_GAP,rects[i].w + 8,32};
        SDL_RenderCopy(renderer,pipe_head,NULL,&head_top);
        SDL_RenderCopy(renderer,pipe_head,NULL,&head_bottom);


    }

    free(rects);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
}
