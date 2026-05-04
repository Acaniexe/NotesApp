#pragma once
#include "ecs.h"
#include "input.h"
#include "canvas.h"
#include "ui.h"

//NodeInteraction functions
void updateNodeInteraction(InputState& input, EntityManager& em, const Canvas& canvas, const UI& ui, Panels& panels);