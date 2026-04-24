#pragma once

struct InputState {
    bool windowResized = false;

    float mouseX = 0.0f;
    float mouseY = 0.0f;

    float zoom = 0.0f;

    bool leftDown = false;
    bool leftReleased = false;
    bool leftHeld = false;

    bool rightDown = false;
    bool rightReleased = false;
    bool rightHeld = false;

    bool dockCollapsePressed = false;

    bool noteToolPressed = false;
    bool textToolPressed = false;
    bool imageToolPressed = false;
    bool todoToolPressed = false;
};