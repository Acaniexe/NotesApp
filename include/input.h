#pragma once

//Input states
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

    bool ctrlDown = false;
    bool ctrlReleased = false;
    bool ctrlHeld = false;
    bool prevCtrlHeld = false;

    bool delPressed = false;

    bool dockCollapsePressed = false;
};