#pragma once
#include "input.h"
#include "ecs.h"

void updateTextInput(InputState& input, EntityManager& em);
void updateEditingTimeout(EntityManager& em, float deltaTime);