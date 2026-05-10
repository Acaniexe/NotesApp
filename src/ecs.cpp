#include "ecs.h"
#include <iostream>
#include <algorithm>

//Create entity
Entity EntityManager::createEntity() {
    Entity e;
    e.id = nextEntityId++;
    entities.push_back(e);
    return e;
}

//Stores entities
std::vector<Entity> EntityManager::getEntities() const {
    return entities;
}

//NODE BLUEPRINTS
//Node Note
static Entity createNote(EntityManager& em, float x, float y) {
    Entity e = em.createEntity();
    em.addComponent<PositionComponent>(e, x, y);
    em.addComponent<sizeComponent>(e, 200.0f, 150.0f);
    em.addComponent<TextComponent>(e, "New note");
    em.addComponent<TitleComponent>(e, "Note");
    em.addComponent<NodeTypeComponent>(e, NodeType::Note);
    em.addComponent<stateComponent>(e);
    em.addComponent<StyleComponent>(e);

    auto* style = em.getComponent<StyleComponent>(e);

    style->backgroundColour = {255, 255, 100, 255};
    style->textColour = {0, 0, 0, 255};
    style->fontSize = 16;

    return e;
}

//Node Text
static Entity createText(EntityManager& em, float x, float y) {
    Entity e = em.createEntity();
    em.addComponent<PositionComponent>(e, x, y);
    em.addComponent<sizeComponent>(e, 200.0f, 150.0f);
    em.addComponent<TextComponent>(e, "Default");
    em.addComponent<TitleComponent>(e, "Text");
    em.addComponent<NodeTypeComponent>(e, NodeType::Text);
    em.addComponent<stateComponent>(e);
    em.addComponent<StyleComponent>(e);

    auto* style = em.getComponent<StyleComponent>(e);

    style->backgroundColour = {100, 200, 255, 255};
    style->textColour = {0, 0, 0, 255};
    style->fontSize = 16;

    return e;
}

//Node Image
static Entity createImage(EntityManager& em, float x, float y) {
    Entity e = em.createEntity();
    em.addComponent<PositionComponent>(e, x, y);
    em.addComponent<sizeComponent>(e, 200.0f, 150.0f);
    em.addComponent<ImageComponent>(e, "");
    em.addComponent<TitleComponent>(e, "Image");
    em.addComponent<NodeTypeComponent>(e, NodeType::Image);
    em.addComponent<stateComponent>(e);
    em.addComponent<StyleComponent>(e);

    auto* style = em.getComponent<StyleComponent>(e);

    style->backgroundColour = {255, 150, 150, 255};
    style->textColour = {0, 0, 0, 255};
    style->fontSize = 16;

    return e;
}

//Node ToDo
static Entity createToDo(EntityManager& em, float x, float y) {
    Entity e = em.createEntity();
    em.addComponent<PositionComponent>(e, x, y);
    em.addComponent<sizeComponent>(e, 200.0f, 150.0f);
    em.addComponent<TextComponent>(e, "Text");
    em.addComponent<TitleComponent>(e, "ToDo");
    em.addComponent<NodeTypeComponent>(e, NodeType::ToDo);
    em.addComponent<stateComponent>(e);
    em.addComponent<StyleComponent>(e);

    auto* style = em.getComponent<StyleComponent>(e);

    style->backgroundColour = {0, 255, 0, 255};
    style->textColour = {0, 0, 0, 255};
    style->fontSize = 16;

    return e;
}

//Node Link
static Entity createLink(EntityManager& em, float x, float y) {
    Entity e = em.createEntity();
    em.addComponent<PositionComponent>(e, x, y);
    em.addComponent<sizeComponent>(e, 150.0f, 50.0f);
    em.addComponent<TextComponent>(e, "Link");
    em.addComponent<TitleComponent>(e, "Link");
    em.addComponent<NodeTypeComponent>(e, NodeType::Link);
    em.addComponent<stateComponent>(e);
    em.addComponent<StyleComponent>(e);

    auto* style = em.getComponent<StyleComponent>(e);

    style->backgroundColour = {245, 222, 179, 255};
    style->textColour = {0, 0, 0, 255};
    style->fontSize = 16;

    return e;
}

//Node Grid
static Entity createGrid(EntityManager& em, float x, float y) {
    Entity e = em.createEntity();
    em.addComponent<PositionComponent>(e, x, y);
    em.addComponent<sizeComponent>(e, 300.0f, 300.0f);
    em.addComponent<TitleComponent>(e, "Grid");
    em.addComponent<NodeTypeComponent>(e, NodeType::Grid);
    em.addComponent<stateComponent>(e);
    em.addComponent<StyleComponent>(e);

    auto* style = em.getComponent<StyleComponent>(e);

    style->backgroundColour = {152, 251, 152, 255};
    style->textColour = {0, 0, 0, 255};
    style->fontSize = 16;

    return e;
}

//Node Line
static Entity createLine(EntityManager& em, float x, float y) {
    Entity e = em.createEntity();
    em.addComponent<PositionComponent>(e, x, y);
    em.addComponent<sizeComponent>(e, 200.0f, 15.0f);
    em.addComponent<TitleComponent>(e, "Line");
    em.addComponent<NodeTypeComponent>(e, NodeType::Line);
    em.addComponent<stateComponent>(e);
    em.addComponent<StyleComponent>(e);

    auto* style = em.getComponent<StyleComponent>(e);

    style->backgroundColour = {30, 155, 255, 255};
    style->textColour = {0, 0, 0, 255};
    style->fontSize = 16;

    return e;
}

/*//Node Draw (Not actually a node, This is free hand draw)
static Entity createDraw(EntityManager& em, float x, float y) {
    Entity e = em.createEntity();
    em.addComponent<PositionComponent>(e, x, y);
    em.addComponent<sizeComponent>(e, 300.0f, 200.0f);
    em.addComponent<NodeTypeComponent>(e, NodeType::Draw);
    return e;
}*/

//Node Colour
static Entity createColour(EntityManager& em, float x, float y) {
    Entity e = em.createEntity();
    em.addComponent<PositionComponent>(e, x, y);
    em.addComponent<sizeComponent>(e, 100.0f, 100.0f);
    em.addComponent<TitleComponent>(e, "Colour");
    em.addComponent<NodeTypeComponent>(e, NodeType::Colour);
    em.addComponent<stateComponent>(e);
    em.addComponent<StyleComponent>(e);

    auto* style = em.getComponent<StyleComponent>(e);

    style->backgroundColour = {0, 255, 0, 255};
    style->textColour = {0, 0, 0, 255};
    style->fontSize = 16;

    return e;
}

//Node Comment (Theoretical node, will be used as a drag select + 'c' to comment things)
static Entity createComment(EntityManager& em, float x, float y) {
    Entity e = em.createEntity();
    em.addComponent<PositionComponent>(e, x, y);
    em.addComponent<sizeComponent>(e, 200.0f, 100.0f);
    em.addComponent<TextComponent>(e, "Comment");
    em.addComponent<TitleComponent>(e, "Comment");
    em.addComponent<NodeTypeComponent>(e, NodeType::Comment);
    em.addComponent<stateComponent>(e);
    em.addComponent<StyleComponent>(e);

    auto* style = em.getComponent<StyleComponent>(e);

    style->backgroundColour = {255, 228, 225, 255};
    style->textColour = {0, 0, 0, 255};
    style->fontSize = 16;

    return e;
}


//Node Code
static Entity createCode(EntityManager& em, float x, float y) {
    Entity e = em.createEntity();
    em.addComponent<PositionComponent>(e, x, y);
    em.addComponent<sizeComponent>(e, 300.0f, 150.0f);
    em.addComponent<TextComponent>(e, "Code");
    em.addComponent<TitleComponent>(e, "Code");
    em.addComponent<NodeTypeComponent>(e, NodeType::Code);
    em.addComponent<stateComponent>(e);
    em.addComponent<StyleComponent>(e);

    auto* style = em.getComponent<StyleComponent>(e);

    style->backgroundColour = {200, 200, 200, 255};
    style->textColour = {0, 0, 0, 255};
    style->fontSize = 16;

    return e;
}

//Node type to string
const char* nodeTypeToString(NodeType type) {
    switch (type) {
        case NodeType::Note:    return "Note";
        case NodeType::Text:    return "Text";
        case NodeType::Image:   return "Image";
        case NodeType::ToDo:    return "ToDo";
        case NodeType::Link:    return "Link";
        case NodeType::Grid:    return "Grid";
        case NodeType::Line:    return "Line";
        //case NodeType::Draw:    return "Draw";
        case NodeType::Colour:  return "Colour";
        case NodeType::Comment: return "Comment";
        case NodeType::Code:    return "Code";
        default:                return "Unknown";
    }
}

//Create node function
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
        //case NodeType::Draw:    result = createDraw(em, x, y); break;
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

Entity cloneNode(EntityManager& em, Entity source, float x, float y) {
    auto* type = em.getComponent<NodeTypeComponent>(source);

    if (!type) return {};

    Entity newNode = createNode(em, type->type, x, y);

    if (auto* srcText = em.getComponent<TextComponent>(source)) {
        auto* dstText = em.getComponent<TextComponent>(newNode);

        if (dstText)
            dstText->text = srcText->text;
    }

    if (auto* srcSize = em.getComponent<sizeComponent>(source)) {

        auto* dstSize = em.getComponent<sizeComponent>(newNode);

        if (dstSize) {
            dstSize->width = srcSize->width;
            dstSize->height = srcSize->height;
        }
    }

    if (auto* srcImage = em.getComponent<ImageComponent>(source)) {

        auto* dstImage = em.getComponent<ImageComponent>(newNode);

        if (dstImage)
            dstImage->path = srcImage->path;
    }

    return newNode;
};

Entity getSelectedEntity(EntityManager& em) {
    for (auto e : em.getEntities()) {
        auto* st = em.getComponent<stateComponent>(e);

        if (st && st->isSelected)
            return e;
    }
    return {};
}

//Handles deletion of nodes
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

void EntityManager::bringToFront(const Entity& entity) {
    auto it = std::find_if(entities.begin(), entities.end(), [&entity](const Entity& e) { return e.id == entity.id; });

    if (it != entities.end()) {
        Entity temp = *it;
        entities.erase(it);
        entities.push_back(temp);
    }
}