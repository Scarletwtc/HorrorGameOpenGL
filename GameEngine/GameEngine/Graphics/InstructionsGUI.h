#pragma once
#include "imgui/imgui.h"


class InstructionsGUI {
private:
    bool isVisible; // Tracks whether the instructions dialog is visible

public:
    InstructionsGUI() : isVisible(false) {}

    void toggleVisibility() {
        isVisible = !isVisible;
    }

    bool isDialogVisible() const {
        return isVisible;
    }

    void render() {
        if (!isVisible) return;

        ImGui::Begin("Game Instructions");

        ImGui::TextWrapped("Welcome to the game! Here are the instructions:");
        ImGui::BulletText("Use W, A, S, D to move.");
        ImGui::BulletText("Press Space to jump.");
        ImGui::BulletText("Escape to toggle the menu.");

        if (ImGui::Button("Close")) {
            isVisible = false;
        }

        ImGui::End();
    }
};
