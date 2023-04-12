#pragma once
#include <SDL.h>


#include "consts.h"
#include "dna.h"



typedef struct NN {
    int inputs_count , outputs_count , hidden_count;
    DNA dna;
    float inputs[DNA_INPUTS];
} NN;
typedef struct NN_Renderer { 
    int screen_width ,screen_height;
    float x , y ,w , h;
    NN nn;

} NN_Renderer;

void NN_Render(SDL_Renderer* renderer,NN_Renderer nn_renderer,NN nn);
    