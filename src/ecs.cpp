#include "ecs.h"
#include <iostream>
#include <algorithm>

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
    em.addComponent<stateComponent>(e);
    return e;
}

static Entity createText(EntityManager& em, float x, float y) {
    Entity e = em.createEntity();
    em.addComponent<PositionComponent>(e, x, y);
    em.addComponent<sizeComponent>(e, 200.0f, 150.0f);
    em.addComponent<TextComponent>(e, "Text");
    em.addComponent<NodeTypeComponent>(e, NodeType::Text);
    em.addComponent<stateComponent>(e);
    return e;
}

static Entity createImage(EntityManager& em, float x, float y) {
    Entity e = em.createEntity();
    em.addComponent<PositionComponent>(e, x, y);
    em.addComponent<sizeComponent>(e, 200.0f, 150.0f);
    em.addComponent<ImageComponent>(e, "");
    em.addComponent<NodeTypeComponent>(e, NodeType::Image);
    em.addComponent<stateComponent>(e);
    return e;
}

static Entity createToDo(EntityManager& em, float x, float y) {
    Entity e = em.createEntity();
    em.addComponent<PositionComponent>(e, x, y);
    em.addComponent<sizeComponent>(e, 200.0f, 150.0f);
    em.addComponent<TextComponent>(e, "Text");
    em.addComponent<NodeTypeComponent>(e, NodeType::ToDo);
    em.addComponent<stateComponent>(e);
    return e;
}

static Entity createLink(EntityManager& em, float x, float y) {
    Entity e = em.createEntity();
    em.addComponent<PositionComponent>(e, x, y);
    em.addComponent<sizeComponent>(e, 150.0f, 50.0f);
    em.addComponent<TextComponent>(e, "Link");
    em.addComponent<NodeTypeComponent>(e, NodeType::Link);
    em.addComponent<stateComponent>(e);
    return e;
}

static Entity createGrid(EntityManager& em, float x, float y) {
    Entity e = em.createEntity();
    em.addComponent<PositionComponent>(e, x, y);
    em.addComponent<sizeComponent>(e, 300.0f, 300.0f);
    em.addComponent<NodeTypeComponent>(e, NodeType::Grid);
    em.addComponent<stateComponent>(e);
    return e;
}

static Entity createLine(EntityManager& em, float x, float y) {
    Entity e = em.createEntity();
    em.addComponent<PositionComponent>(e, x, y);
    em.addComponent<sizeComponent>(e, 200.0f, 15.0f);
    em.addComponent<NodeTypeComponent>(e, NodeType::Line);
    em.addComponent<stateComponent>(e);
    return e;
}

static Entity createDraw(EntityManager& em, float x, float y) {
    Entity e = em.createEntity();
    em.addComponent<PositionComponent>(e, x, y);
    em.addComponent<sizeComponent>(e, 300.0f, 200.0f);
    em.addComponent<NodeTypeComponent>(e, NodeType::Draw);
    return e;
}

static Entity createColour(EntityManager& em, float x, float y) {
    Entity e = em.createEntity();
    em.addComponent<PositionComponent>(e, x, y);
    em.addComponent<sizeComponent>(e, 100.0f, 100.0f);
    em.addComponent<NodeTypeComponent>(e, NodeType::Colour);
    em.addComponent<stateComponent>(e);
    return e;
}

static Entity createComment(EntityManager& em, float x, float y) {
    Entity e = em.createEntity();
    em.addComponent<PositionComponent>(e, x, y);
    em.addComponent<sizeComponent>(e, 200.0f, 100.0f);
    em.addComponent<TextComponent>(e, "Comment");
    em.addComponent<NodeTypeComponent>(e, NodeType::Comment);
    em.addComponent<stateComponent>(e);
    return e;
}

static Entity createCode(EntityManager& em, float x, float y) {
    Entity e = em.createEntity();
    em.addComponent<PositionComponent>(e, x, y);
    em.addComponent<sizeComponent>(e, 300.0f, 150.0f);
    em.addComponent<TextComponent>(e, "Code");
    em.addComponent<NodeTypeComponent>(e, NodeType::Code);
    em.addComponent<stateComponent>(e);
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
        case NodeType::Note:    result = createNote(em, x, y); break;
        case NodeType::Text:    result = createText(em, x, y); break;
        case NodeType::Image:   result = createImage(em, x, y); break;
        case NodeType::ToDo:    result = createToDo(em, x, y); break;
        case NodeType::Link:    result = createLink(em, x, y); break;
        case NodeType::Grid:    result = createGrid(em, x, y); break;
        case NodeType::Line:    result = createLine(em, x, y); break;
        case NodeType::Draw:    result = createDraw(em, x, y); break;
        case NodeType::Colour:  result = createColour(em, x, y); break;
        case NodeType::Comment: result = createComment(em, x, y); break;
        case NodeType::Code:    result = createCode(em, x, y); break;
        default:                result = createText(em, x, y); break;
    };

    std::cout << "Node created: " << result.id
               << " [" << nodeTypeToString(type) << "]"
               << " at (" << x << ", " << y << ")\n";

    return result;
}

void EntityManager::removeEntity(const Entity& entity) {
    entities.erase(
        std::remove_if(
            entities.begin(),
            entities.end(),
            [&entity](const Entity& e) { return e.id == entity.id; }
        ),
        entities.end()
    );

    componentStorage.erase(entity.id);

    std::cout << "Entity removed: " << entity.id << "\n";
}

void deleteNode(EntityManager& em) {
    std::vector<Entity> toDelete;

    for (auto& e : em.getEntities()) {
        auto* state = em.getComponent<stateComponent>(e);
        if (state && state->isSelected) {
            toDelete.push_back(e);
        }
    }

    for (auto& e : toDelete) {
        em.removeEntity(e);
    }
}