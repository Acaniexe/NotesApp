#pragma once
#include "ecs.h"
#include "input.h"
#include "canvas.h"
#include "ui.h"

void updateNodeInteraction(InputState& input, EntityManager& em, const Canvas& canvas, const UI& ui);