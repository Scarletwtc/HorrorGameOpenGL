#ifndef TASK_MANAGER_H
#define TASK_MANAGER_H

#include <vector>
#include <string>
#include <iostream>

class TaskManager {
public:
    std::vector<std::string> tasks;
    int currentTaskIndex;

    TaskManager() {
        currentTaskIndex = 0;
        tasks = {
            "Find the key to escape the house",
            "Find the gun and acquire it",
            "Find the monsters and kill them",
            "Find the exit to the maze",
            "Kill the big boss!",
            "Great job!You have found your long lost friend.\nGo meet her to end the game."
        };
    }

    // Get the current task
    std::string getCurrentTask() {
        if (currentTaskIndex < tasks.size()) {
            return tasks[currentTaskIndex];
        }
        return "Great job!You have found your long lost friend.\nGo meet her to end the game.";
    }

    // Complete a specific task by index
    void completeCurrentTask(int taskIndex) {
        if (taskIndex >= 0 && taskIndex < tasks.size()) {
            currentTaskIndex = taskIndex + 1;  // Move to the next task
        }
    }

    // Check if all tasks are completed
    bool allTasksCompleted() {
        return currentTaskIndex >= tasks.size();
    }
};

#endif // TASK_MANAGER_H
