#include "ecs.h"
#include <iostream>

// ENTITY
Entity EntityManager::createEntity() {
    Entity e;
    e.id = nextEntityId++;
    entities.push_back(e);
    return e;
}

std::vector<Entity> EntityManager::getEntities() const {
    return entities;
}

// NODE FACTORIES
static Entity createNote(EntityManager& em, float x, float y) {
    Entity e = em.createEntity();

    em.addComponent<PositionComponent>(e, x, y);
    em.addComponent<sizeComponent>(e, 200.0f, 150.0f);
    em.addComponent<TextComponent>(e, "New note");
    em.addComponent<NodeTypeComponent>(e, NodeType::Note);

    return e;
}

static Entity createText(EntityManager& em, float x, float y) {
    Entity e = em.createEntity();

    em.addComponent<PositionComponent>(e, x, y);
    em.addComponent<sizeComponent>(e, 200.0f, 150.0f);
    em.addComponent<TextComponent>(e, "Text");
    em.addComponent<NodeTypeComponent>(e, NodeType::Text);

    return e;
}

static Entity createImage(EntityManager& em, float x, float y) {
    Entity e = em.createEntity();

    em.addComponent<PositionComponent>(e, x, y);
    em.addComponent<sizeComponent>(e, 200.0f, 150.0f);
    em.addComponent<ImageComponent>(e, "");
    em.addComponent<NodeTypeComponent>(e, NodeType::Image);

    return e;
}

static Entity createToDo(EntityManager& em, float x, float y) {
    Entity e = em.createEntity();

    em.addComponent<PositionComponent>(e, x, y);
    em.addComponent<sizeComponent>(e, 200.0f, 150.0f);
    em.addComponent<TextComponent>(e, "Text");
    em.addComponent<NodeTypeComponent>(e, NodeType::ToDo);

    return e;
}

// PUBLIC ENTRY
const char* nodeTypeToString(NodeType type) {
    switch (type) {
        case NodeType::Note:    return "Note";
        case NodeType::Text:    return "Text";
        case NodeType::Image:   return "Image";
        case NodeType::ToDo:    return "ToDo";
        case NodeType::Link:    return "Link";
        case NodeType::Grid:    return "Grid";
        case NodeType::Line:    return "Line";
        case NodeType::Draw:    return "Draw";
        case NodeType::Colour:  return "Colour";
        case NodeType::Comment: return "Comment";
        case NodeType::Code:    return "Code";
        default:                return "Unknown";
    }
}

Entity createNode(EntityManager& em, NodeType type, float x, float y) {
    Entity result;

    switch (type) {
        case NodeType::Note:
            result = createNote(em, x, y);
            break;

        case NodeType::Text:
            result = createText(em, x, y);
            break;

        case NodeType::Image:
            result = createImage(em, x, y);
            break;

        case NodeType::ToDo:
            result = createToDo(em, x, y);
            break;

        default:
            result = createText(em, x, y);
            break;
    };

    std::cout << "Node created: " << result.id
               << " [" << nodeTypeToString(type) << "]"
               << " at (" << x << ", " << y << ")\n";

    return result;
}