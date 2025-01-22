#include "window.h"
#include "../Graphics/imgui/imgui.h"
#include "../Graphics/imgui/imgui_impl_glfw.h"
#include "../Graphics/imgui/imgui_impl_opengl3.h"
#include "TaskManager.h"

bool isGameStarted = false;

Window::Window(char* name, int width, int height)
{
	this->name = name;
	this->width = width;
	this->height = height;
	this->paused = false;
	this->lastMenuToggleTime = 0.0;
	init();

	for (int i = 0; i < MAX_KEYBOARD; i++)
	{
		this->keys[i] = false;
	}

	for (int i = 0; i < MAX_MOUSE; i++)
	{
		this->mouseButtons[i] = false;
	}
}

Window::~Window()
{
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	glfwTerminate();
}

void Window::init()
{
	if (!glfwInit())
	{
		std::cout << "Error initializing glfw!" << std::endl;
	}
	else
	{
		std::cout << "Successfully initializing glfw!" << std::endl;
	}

	window = glfwCreateWindow(width, height, name, NULL, NULL);

	if (window == NULL)
	{
		std::cout << "Failed to create a GLFW window" << std::endl;
		glfwTerminate();
		return;
	}

	glfwMakeContextCurrent(window);
	glfwSwapInterval(1); // Enable vsync

	//callbacks for user input
	glfwSetWindowUserPointer(window, this);
	glfwSetKeyCallback(window, key_callback);
	glfwSetMouseButtonCallback(window, mouse_button_callback);
	glfwSetCursorPosCallback(window, cursor_position_callback);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

	if (glewInit() != GLEW_OK)
	{
		std::cout << "Error initializing glew!" << std::endl;
	}
	else
	{
		std::cout << "Successfully initializing glew!" << std::endl;
	}

	std::cout << "Open GL " << glGetString(GL_VERSION) << std::endl;
	glEnable(GL_BLEND);


	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();
	//ImGui::StyleColorsLight();

	// Initialize ImGui backend for GLFW and OpenGL
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 330"); // GLSL version
}

void Window::update()
{
	glfwPollEvents();
	glfwGetFramebufferSize(window, &width, &height); //updates the width and height variables after resize
	glViewport(0, 0, width, height);
	handleEscapeToMenuInput();
	//
	renderGUI(isGameStarted);
	glfwSwapBuffers(window);
	

}

void Window::clear()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

GLFWwindow* Window::getWindow()
{
	return window;
}

int Window::getWidth()
{
	return width;
}

int Window::getHeight()
{
	return height;
}

void Window::setKey(int key, bool ok)
{
	this->keys[key] = ok;
}

void Window::setMouseButton(int button, bool ok)
{
	this->mouseButtons[button] = ok;
}

void Window::setMousePos(double xpos, double ypos)
{
	this->xpos = xpos;
	this->ypos = ypos;
}

void Window::getMousePos(double& xpos, double& ypos)
{
	xpos = this->xpos;
	ypos = this->ypos;
}

//Handling key pressed
bool Window::isPressed(int key)
{
	return keys[key];
}

//Handling mouse buttons pressed
bool Window::isMousePressed(int button)
{
	return mouseButtons[button];
}

//Handling keyboard actions
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	Window* wind = (Window*)glfwGetWindowUserPointer(window);

	if (action != GLFW_RELEASE)
		wind->setKey(key, true);
	else
		wind->setKey(key, false);
}

//Handling mouse actions
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
	Window* wind = (Window*)glfwGetWindowUserPointer(window);

	if (action != GLFW_RELEASE)
		wind->setMouseButton(button, true);
	else
		wind->setMouseButton(button, false);
}

//Handling cursor position
void cursor_position_callback(GLFWwindow* window, double xpos, double ypos)
{
	Window* wind = (Window*)glfwGetWindowUserPointer(window);
	wind->setMousePos(xpos, ypos);
}

void Window::handleEscapeToMenuInput() {
	double currentTime = glfwGetTime();

	// Check if LEFT_ALT is pressed and debounce
	if (this->isPressed(GLFW_KEY_LEFT_ALT) && (currentTime - lastMenuToggleTime >= 1.0)) {
		lastMenuToggleTime = currentTime; // Update the last toggle time
		menu.toggleVisibility();         // Toggle the menu visibility
		paused = menu.isMenuVisible();   // Set the paused state based on menu visibility

		// Enable or disable the mouse cursor based on the menu state
		if (menu.isMenuVisible()) {
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		}
		else {

			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		}
	}
}



//void Window::renderGUI() {
//
//
//	ImGui_ImplOpenGL3_NewFrame();
//	ImGui_ImplGlfw_NewFrame();
//	ImGui::NewFrame();
//
//	menu.render(paused, window);
//	//instructions.render();
//
//	ImGui::Render();
//	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
//
//}

TaskManager taskManager;
void Window::renderGUI(bool& isGameStarted) {
	static bool isStoryShown = false;         // Tracks if the story has been shown
	static bool isInstructionsShown = false; // Tracks if the instructions have been shown

	// Start a new ImGui frame
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	// Get the display size
	ImVec2 displaySize = ImGui::GetIO().DisplaySize;

	if (!isGameStarted) {
		// Set the window position and size to fill the entire screen
		ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_Always);
		ImGui::SetNextWindowSize(displaySize, ImGuiCond_Always);

		// Begin a fullscreen window with no title bar or resize options
		ImGui::Begin("Game GUI", nullptr,
			ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse |
			ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);

		// Common style settings for both scenes
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(10, 20));
		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(15, 15));
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(10, 10));

		if (!isStoryShown) {
			// Scene 1: Story
			ImGui::GetIO().FontGlobalScale = 1.5f;
			const char* storyText =
				"Free the Truth: Escape the Maze\n\n\n\n\n"
				"Awaken as a young girl lost in a realm of eerie desolation, where the air is thick with foreboding \n"
				"and your past is a blank slate. Trapped within a crumbling barn hidden deep in a fog-laden, \n"
				"sinister forest, the only fragment of your memory is a fleeting image of your friend before \n"
				"the world was swallowed by darkness. \n\n"
				"Your journey begins with a daring escape from the barn, but beyond its rotting walls lies no sanctuary. \n"
				"The forest is alive—a labyrinth of shadows prowled by monstrous beings bent on your destruction. \n"
				"With newfound abilities at your disposal, you must stand your ground, uncover the forest's deadly secrets, \n"
				"and forge a path through the chaos. \n\n"
				"Will you piece together the fragments of your shattered past, save your friend, and defy the forest’s grip? \n"
				"Or will its unrelenting darkness claim you both, leaving your story untold forever?";


			ImVec2 textSize = ImGui::CalcTextSize(storyText);
			ImVec2 textPos((displaySize.x - textSize.x) / 2, (displaySize.y - textSize.y) / 2);
			ImGui::SetCursorPos(textPos);
			ImGui::TextWrapped(storyText);

			// Center the Next button
			ImVec2 buttonSize(200, 50);
			ImVec2 buttonPos((displaySize.x - buttonSize.x) / 2, displaySize.y - 100);
			ImGui::SetCursorPos(buttonPos);
			if (ImGui::Button("Next", buttonSize)) {
				isStoryShown = true; // Mark the story as shown
			}
		}
		else if (!isInstructionsShown) {
			// Scene 2: Instructions
			const char* instructionsText =
				"Instructions:\n"
				"- Press W to go in front.\n"
				"- Press S to go in backwards.\n"
				"- Press A to go to the left.\n"
				"- Press D to go to the right.\n"
				"- Press Left Key to rotate to the left.\n"
				"- Press Right Key to rotate to the right.\n"
				"- Press E to aquire the objects.\n"
				"- Press Space bar to shoot. \n";

			ImVec2 textSize = ImGui::CalcTextSize(instructionsText);
			ImVec2 textPos((displaySize.x - textSize.x) / 2, (displaySize.y - textSize.y) / 2);
			ImGui::SetCursorPos(textPos);
			ImGui::TextWrapped(instructionsText);

			// Center the Next button
			ImVec2 buttonSize(200, 50);
			ImVec2 buttonPos((displaySize.x - buttonSize.x) / 2, displaySize.y - 100);
			ImGui::SetCursorPos(buttonPos);
			if (ImGui::Button("Next", buttonSize)) {
				isInstructionsShown = true; // Mark the instructions as shown
			}
		}
		else {
			// Scene 3: Start Game
			const char* startText = "Are you ready to begin your journey?";
			ImVec2 textSize = ImGui::CalcTextSize(startText);
			ImVec2 textPos((displaySize.x - textSize.x) / 2, (displaySize.y - textSize.y) / 2);
			ImGui::SetCursorPos(textPos);
			ImGui::TextWrapped(startText);

			// Center the Start Game button
			ImVec2 buttonSize(200, 50);
			ImVec2 buttonPos((displaySize.x - buttonSize.x) / 2, displaySize.y - 100);
			ImGui::SetCursorPos(buttonPos);
			if (ImGui::Button("Start Game", buttonSize)) {
				isGameStarted = true; // Start the game
			}
		}

		ImGui::PopStyleVar(3); // Restore previous styles
		ImGui::End(); // End the fullscreen window
	}
	else {
		ImGui::GetIO().FontGlobalScale = 1.0f;
		// Render the task system window as previously defined
		ImGui::SetNextWindowPos(ImVec2(10, 10), ImGuiCond_Always);
		ImGui::SetNextWindowSize(ImVec2(500, 200), ImGuiCond_Always);

		ImGui::Begin("Task System");
		ImGui::Text("Current Task: %s", taskManager.getCurrentTask().c_str());

		if (ImGui::Button("Complete Task")) {
			if (!taskManager.allTasksCompleted()) {
				int taskIndex = taskManager.currentTaskIndex;
				taskManager.completeCurrentTask(taskIndex);
			}
		}
		ImGui::End();
	}

	// Render ImGui
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}


