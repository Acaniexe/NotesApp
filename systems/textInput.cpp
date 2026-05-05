#include "textInput.h"
#include "ecs.h"

void updateTextInput(InputState& input, EntityManager& em) {

    if (!input.textInput.empty() || input.backspaceDown || input.backspacePressed) {
        for (auto& e : em.getEntities()) {
            auto* state = em.getComponent<stateComponent>(e);
            auto* text = em.getComponent<TextComponent>(e);
            if (state && text && state->isSelected) {
                state->isEditing = true;
                state->editTimer = 0.0f;
            }
        }
    }
    if (!input.isTyping) return;

    for (auto& e : em.getEntities()) {
        auto* state = em.getComponent<stateComponent>(e);
        auto* text  = em.getComponent<TextComponent>(e);

        if (!state || !text) continue;
        if (!state->isSelected) continue;

        // Add typed characters
        if (!input.textInput.empty()) {
            text->text += input.textInput;
        }

        // Backspace
        if ((input.backspacePressed || input.backspaceDown) && !text->text.empty()) {
            text->text.pop_back();
        }
    }
}

void updateEditingTimeout(EntityManager& em, float deltaTime) {
    const float editTimeout = 0.7f;

    for (auto& e : em.getEntities()) {
        auto* st = em.getComponent<stateComponent>(e);
        if (!st || !st->isEditing) continue;

        st->editTimer += deltaTime;

        if (st->editTimer >= editTimeout) {
            st->isEditing = false;
            st->editTimer = 0.0f;
        }
    }
}