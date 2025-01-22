#pragma once
#include "imgui/imgui.h"
#include <glfw3.h>
#include <glew.h>

class MenuGUI {
private:
    bool isVisible; // Tracks whether the menu is currently visible
    bool spectateMode; // Tracks the state of "Spectate Mode"
    bool showLiquidAnimation; // Tracks the state of "Show Liquid Animation"


public:
    MenuGUI() : isVisible(false), spectateMode(false), showLiquidAnimation(false) {}

    void toggleVisibility() {
        isVisible = !isVisible;
    }

    bool isMenuVisible() const {
        return isVisible;
    }

    void render(bool& paused, GLFWwindow* window) {
        if (!isVisible) return;

        ImGui::Begin("Menu");

        if (ImGui::Checkbox("Spectate Mode", &spectateMode)) {
            // Handle spectate mode toggle logic
        }

        if (ImGui::Checkbox("Show Liquid Animation", &showLiquidAnimation)) {
            // Handle liquid animation toggle logic
        }

        if (ImGui::Button("Close")) {
            isVisible = false;
            paused = false; // Unpause the game when the menu is closed
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED); // Lock the mouse
        }

        ImGui::End();
    }
};
