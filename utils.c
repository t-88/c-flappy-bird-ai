
#include "utils.h"

float randomFloat(float a){
    return (((float)rand() / RAND_MAX) - 0.5) * 2 * a;
}

int isColliding(AABB aabb1, AABB aabb2){
    return aabb1.x + aabb1.w >= aabb2.x && aabb2.x + aabb2.w >= aabb1.x && aabb1.y + aabb1.h >= aabb2.y && aabb2.y + aabb2.h >= aabb1.y;
}


float map(float value,float start1,float end1,float start2,float end2) {
    float maped1 = end1 - start1;
    float maped2 = end2 - start2;

    return (maped2 / maped1) * (value - start1) + start2;  
}