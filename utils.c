
#include "utils.h"

float randomFloat(float a){
    return (((float)rand() / RAND_MAX) - 0.5) * 2 * a;
}

int isColliding(AABB aabb1, AABB aabb2){
    return aabb1.x + aabb1.w >= aabb2.x && aabb2.x + aabb2.w >= aabb1.x && aabb1.y + aabb1.h >= aabb2.y && aabb2.y + aabb2.h >= aabb1.y;
}