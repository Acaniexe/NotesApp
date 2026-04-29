#pragma once
#include <SDL3/SDL.h>
#include <cmath>
#include "ecs.h"
#include "canvas.h"
#include "ui.h"
#include "input.h"

//Renderer functions
void beginFrame(SDL_Renderer* renderer);
void renderCanvas(SDL_Renderer* renderer, const Canvas& canvas, int windowWidth, int windowHeight);
void renderNodes(SDL_Renderer* renderer, EntityManager& em, const Canvas& canvas, float windowWidth, float windowHeight);
void renderUI(SDL_Renderer* renderer, const UI& ui, const InputState& input, const Canvas& canvas);
void renderPanels(SDL_Renderer* renderer, const Panels& panels);
void endFrame(SDL_Renderer* renderer);