#pragma once
#include <SDL.h>



typedef struct NN {
    int inputs_count , outputs_count , hidden_count;
    float* w1 , w2;
    float* b1 , b2;
} NN;
typedef struct NN_Renderer { 
    int screen_width ,screen_height;
    float x , y ,w , h;
    NN nn;

} NN_Renderer;

void NN_Render(SDL_Renderer* renderer,NN_Renderer nn_renderer,NN nn);
    