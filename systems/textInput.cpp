#include "textInput.h"
#include "ecs.h"
#include <algorithm>

static float parseFloatOrDefault(const std::string& value, float fallback) {
    if (value.empty() || value == "-" || value == ".") return fallback;
    try {
        return std::stof(value);
    } catch (...) {
        return fallback;
    }
}

static int parseIntOrDefault(const std::string& value, int fallback) {
    if (value.empty() || value == "-" || value == ".") return fallback;
    try {
        return std::stoi(value);
    } catch (...) {
        return fallback;
    }
}

static std::string filterInputForField(const std::string& textInput, PropertyField field) {
    std::string result;
    for (char c : textInput) {
        switch (field) {
            case PropertyField::PositionX:
            case PropertyField::PositionY:
            case PropertyField::Width:
            case PropertyField::Height:
                if ((c >= '0' && c <= '9') || c == '.' || c == '-') result.push_back(c);
                break;
            case PropertyField::BackgroundR:
            case PropertyField::BackgroundG:
            case PropertyField::BackgroundB:
            case PropertyField::BackgroundA:
            case PropertyField::TextR:
            case PropertyField::TextG:
            case PropertyField::TextB:
            case PropertyField::TextA:
                if ((c >= '0' && c <= '9') || c == '-') result.push_back(c);
                break;
            default:
                result.push_back(c);
                break;
        }
    }
    return result;
}

void updateTextInput(InputState& input, EntityManager& em) {
    if (input.propertyEditing && input.activeProperty != PropertyField::None) {
        Entity selected = getSelectedEntity(em);
        if (selected.id == 0) {
            input.propertyEditing = false;
            input.activeProperty = PropertyField::None;
            input.propertyEditBuffer.clear();
            return;
        }

        if (!input.textInput.empty()) {
            std::string filtered = filterInputForField(input.textInput, input.activeProperty);
            input.propertyEditBuffer += filtered;
        }

        if ((input.backspacePressed || input.backspaceDown) && !input.propertyEditBuffer.empty()) {
            input.propertyEditBuffer.pop_back();
        }

        if (input.activeProperty == PropertyField::Text) {
            if (auto* text = em.getComponent<TextComponent>(selected)) {
                text->text = input.propertyEditBuffer;
            }
        } else if (input.activeProperty == PropertyField::ImagePath) {
            if (auto* image = em.getComponent<ImageComponent>(selected)) {
                image->path = input.propertyEditBuffer;
            }
        } else if (input.activeProperty == PropertyField::Title) {
            if (auto* title = em.getComponent<TitleComponent>(selected)) {
                title->title = input.propertyEditBuffer;
            }
        } else if (input.activeProperty == PropertyField::PositionX || input.activeProperty == PropertyField::PositionY ||
                   input.activeProperty == PropertyField::Width || input.activeProperty == PropertyField::Height) {
            float value = parseFloatOrDefault(input.propertyEditBuffer, 0.0f);
            if (input.activeProperty == PropertyField::PositionX || input.activeProperty == PropertyField::PositionY) {
                if (auto* pos = em.getComponent<PositionComponent>(selected)) {
                    if (input.activeProperty == PropertyField::PositionX) pos->x = value;
                    else pos->y = value;
                }
            } else if (auto* size = em.getComponent<sizeComponent>(selected)) {
                if (input.activeProperty == PropertyField::Width) size->width = value;
                else size->height = value;
            }
        } else if (auto* style = em.getComponent<StyleComponent>(selected)) {
            int value = parseIntOrDefault(input.propertyEditBuffer, 0);
            if (value < 0) value = 0;
            if (value > 255) value = 255;
            switch (input.activeProperty) {
                case PropertyField::BackgroundR: style->backgroundColour.r = (Uint8)value; break;
                case PropertyField::BackgroundG: style->backgroundColour.g = (Uint8)value; break;
                case PropertyField::BackgroundB: style->backgroundColour.b = (Uint8)value; break;
                case PropertyField::BackgroundA: style->backgroundColour.a = (Uint8)value; break;
                case PropertyField::TextR: style->textColour.r = (Uint8)value; break;
                case PropertyField::TextG: style->textColour.g = (Uint8)value; break;
                case PropertyField::TextB: style->textColour.b = (Uint8)value; break;
                case PropertyField::TextA: style->textColour.a = (Uint8)value; break;
                default: break;
            }
        }

        return;
    }

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

