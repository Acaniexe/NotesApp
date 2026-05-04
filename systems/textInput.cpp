#include "textInput.h"

void updateTextInput(InputState& input, EntityManager& em) {
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