#include "Graphics\window.h"
#include "Camera\camera.h"
#include "Shaders\shader.h"
#include "Model Loading\mesh.h"
#include "Model Loading\texture.h"
#include "Model Loading\meshLoaderObj.h"
#include "Graphics/TaskManager.h"

#define STB_IMAGE_IMPLEMENTATION
#include "Model Loading\stb_image.h"

const unsigned int width = 800;
const unsigned int height = 800;

//bool isGameStarted = false;

//skyBox vertices

float skyboxVertices[] = {
	-500.0f, -500.0f,  500.0f,
	 500.0f, -500.0f,  500.0f,
	 500.0f, -500.0f, -500.0f,
	-500.0f, -500.0f, -500.0f,
	-500.0f,  500.0f,  500.0f,
	 500.0f,  500.0f,  500.0f,
	 500.0f,  500.0f, -500.0f,
	-500.0f,  500.0f, -500.0f
};

unsigned int skyboxIndices[] =
{
	// Right
	1, 2, 6,
	6, 5, 1,
	// Left
	0, 4, 7,
	7, 3, 0,
	// Top
	4, 5, 6,
	6, 7, 4,
	// Bottom
	0, 3, 2,
	2, 1, 0,
	// Front
	0, 1, 5,
	5, 4, 0,
	// Back
	3, 7, 6,
	6, 2, 3
};

void processKeyboardInput();

float deltaTime = 0.0f;	// time between current frame and last frame
float lastFrame = 0.0f;


bool gunAcquired = false;
bool allMonstersDead = false;

//dragon speed pew pew 
float dragonSpeed = 2.0f;   // Controls how fast they move
float dragonAmplitude = 20.0f; // Controls how far they move from their base position


Window window("Game Engine", 800, 800);
Camera camera(glm::vec3(-124.58f, -1.71484f, 1.30934f));

glm::vec3 lightColor = glm::vec3(1.0f);
glm::vec3 lightPos = glm::vec3(-180.0f, 100.0f, -200.0f);


std::vector<glm::vec3> dragonPositions = {
			glm::vec3(-59.3f, -1.0f, 0.0f),
			glm::vec3(-32.0f, -1.0f, 0.0f),
			glm::vec3(-10.0f, -1.0f, 0.0f),
			glm::vec3(-85.0f, -1.0f, 0.0f),
};




float playerHealth = 100.0f;


struct BoundingBox {
	glm::vec3 min;
	glm::vec3 max;
};

BoundingBox hutBoundingBox = {
	glm::vec3(-4.0f, -3.0f, -4.0f),  // Minimum corner
	glm::vec3(4.0f, 6.0f, 4.0f)      // Maximum corner
};

BoundingBox boxBoundingBox = {
	glm::vec3(64.7131f, -4.71484f, -40.7417f),
	glm::vec3(70.7131f, 1.28516f, -34.7417f)
};


BoundingBox barrierBoundingBox = {
	glm::vec3(6.06431f, -1.71484f, -50.555f),
	glm::vec3(18.9963f, -1.71484f, 62.0326f)
};

BoundingBox monsterZone = {
	glm::vec3(-300.125f, -1.71484f, -20.0f),
	glm::vec3(300.9963f, -1.71484f, 40.0f)
};

BoundingBox mazeExitBoundingBox = {
	glm::vec3(64.7131f, -4.71484f, -40.7417f),
	glm::vec3(70.7131f, 1.28516f, -34.7417f)
};

bool isColliding(const glm::vec3& playerPosition, const BoundingBox& box)
{
	return (playerPosition.x >= box.min.x && playerPosition.x <= box.max.x) &&
		(playerPosition.y >= box.min.y && playerPosition.y <= box.max.y) &&
		(playerPosition.z >= box.min.z && playerPosition.z <= box.max.z);
}

bool arePositionsEqual(const glm::vec3& pos1, const glm::vec3& pos2, float epsilon = 0.0001f)
{
	return std::fabs(pos1.x - pos2.x) < epsilon &&
		std::fabs(pos1.y - pos2.y) < epsilon &&
		std::fabs(pos1.z - pos2.z) < epsilon;
}

void printCameraCoordinates(Camera camera) {
	glm::vec3 position = camera.getCameraPosition();
	std::cout << "Camera Coordinates: X = " << position.x
		<< ", Y = " << position.y
		<< ", Z = " << position.z << std::endl;
}
bool mazecomplete = false;

void checkMazeExit() {
	if (isColliding(camera.getCameraPosition(), boxBoundingBox)) {
		if (mazecomplete == false) {
			mazecomplete = true;
			std::cout << "Maze Exit Reached!" << std::endl;
			window.taskManager.completeCurrentTask(3);  // Update task to "Boss Fight"
		}
	}
}

bool barrierActive = true;
//bool mazecomplete = false;
void updateBarrierState() {
	if (dragonPositions.empty()) {
		barrierActive = false;
	}
}

bool checkCollision(glm::vec3 newPosition, BoundingBox box) {
	return (newPosition.x >= box.min.x && newPosition.x <= box.max.x &&
		newPosition.y >= box.min.y && newPosition.y <= box.max.y &&
		newPosition.z >= box.min.z && newPosition.z <= box.max.z);
}
void checkMonsterCollision() {
	float currentTime = glfwGetTime();

	for (size_t i = 0; i < dragonPositions.size(); i++) {
		// Calculate dragon's current position based on its movement pattern
		glm::vec3 dynamicDragonPos = dragonPositions[i];
		dynamicDragonPos.z += dragonAmplitude * sin(dragonSpeed * currentTime + i);  // Apply Z-movement

		glm::vec3 playerPos = camera.getCameraPosition();
		glm::vec3 adjustedPlayerPos = glm::vec3(playerPos.x, dynamicDragonPos.y, playerPos.z);  // Match Y to dragon

		float distance = glm::length(adjustedPlayerPos - dynamicDragonPos);  // Compare with updated dragon position

		if (distance < 10.0f) {  // Collision radius
			playerHealth -= 0.01f;
			//std::cout << "Monster collision! Player HP: " << playerHealth << std::endl;

			if (playerHealth <= 0) {
				std::cout << "Player defeated by monsters!" << std::endl;

				// glfwSetWindowShouldClose(window.getWindow(), GLFW_TRUE);
			}
		}
		

	}
}







// Health and combat variables
//float playerHealth = 100.0;
int monsterHealth = 100;
int monsterAttackDamage = 10;  // Damage monster deals when close to player

// Shooting mechanics
struct Bullet
{
	glm::vec3 position;
	glm::vec3 direction;
	float traveledDistance;
	bool isActive;
};
std::vector<Bullet> bullets; // Store active bullets
std::vector<Bullet> monsterBullets; // Store active monster bullets
float shootingCooldown = 0.5f; // Seconds between shots (for player)
float monsterShootingCooldown = 5.0f; // Seconds between shots (for monster)
float lastShotTime = 0.0f;
float lastMonsterShotTime = 0.0f;
int shootingDamage = 20; // Damage dealt by a shot
int monsterBulletDamage = 15; // Damage dealt by monster's bullet
float maxBulletRange = 500.0f; // Maximum distance bullets can travel

// Monster position and AI parameters
glm::vec3 monsterPosition = glm::vec3(66.2012f, -1.71484f, 22.8688f) + glm::vec3(10.0f, 0.0f, 10.0f);
float monsterSpeed = 2.0f; // Monster movement speed
bool isMonsterAlive = true; // Flag to indicate monster's status
bool isNewScene = false;
bool isLastScene = false;
float monsterAttackRange = 2.0f; // Range at which monster can attack the player

std::vector<int> dragonHealth(dragonPositions.size(), 100);
bool showLoadingScreen = true;

//loading screen variables
struct Button {
	float x, y, width, height;
	bool isHovered(float mouseX, float mouseY) {
		return mouseX >= x && mouseX <= x + width &&
			mouseY >= y && mouseY <= y + height;
	}
} startButton = { 300, 400, 200, 50 };


// Button for starting the game and transitioning


// Function declarations
void processKeyboardInput();
void handleShooting();
void moveMonsterTowardPlayer(float deltaTime);
void updateBullets(float deltaTime);
void handleMonsterShooting(float deltaTime);
void handleMonsterAttack();
void updateMonsterBullets(float deltaTime);
void renderMonsterHealthBar(Shader& shader, const glm::vec3& color);
void renderVictoryScene(Shader& shader, Mesh& plane, GLuint wood, Mesh& womanModel, GLuint womanTexture);
void renderMonsterScene(Shader& sunShader, Shader& shader, Shader& skyboxShader, Mesh& sun, Mesh& box, Mesh& plane2, Mesh& monster, Mesh& gun, Mesh& bulletModel, GLuint tex, unsigned int& skyboxVAO, unsigned int& cubemapTexture, Mesh& plane, GLuint wood, Mesh& womanModel, GLuint womanTexture);

//doesnt work yet
bool hasKey = false;

bool isInsideBoundary()
{
	// Get the camera position directly inside the function
	glm::vec3 position = camera.getCameraPosition();

	// Define the boundary limits (adjust the values to your specific needs)
	glm::vec3 minBoundary = glm::vec3(-129.521f, -1.71484f, -9.39559f); // Bottom-left corner
	glm::vec3 maxBoundary = glm::vec3(-110.773f, -1.71484f, 10.219f);  // Top-right corner

	// Check if the position is inside the boundary
	return (position.x >= minBoundary.x && position.x <= maxBoundary.x &&
		position.z >= minBoundary.z && position.z <= maxBoundary.z);
}

// Render button with text
GLuint backgroundTexture;

void loadBackgroundTexture() {
	backgroundTexture = loadBMP("Resources/Textures/background.bmp");  // Replace with your image
	if (backgroundTexture == 0) {
		std::cerr << "Failed to load background texture!" << std::endl;
	}
	else {
		std::cout << "Background texture loaded successfully." << std::endl;
	}
}

// Render the full-screen background quad
void renderBackground() {
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, backgroundTexture);

	glBegin(GL_QUADS);
	glTexCoord2f(0.0f, 0.0f); glVertex2f(-1.0f, -1.0f);  // Bottom-left
	glTexCoord2f(1.0f, 0.0f); glVertex2f(1.0f, -1.0f);   // Bottom-right
	glTexCoord2f(1.0f, 1.0f); glVertex2f(1.0f, 1.0f);    // Top-right
	glTexCoord2f(0.0f, 1.0f); glVertex2f(-1.0f, 1.0f);   // Top-left
	glEnd();

	glDisable(GL_TEXTURE_2D);
}

// Render button with text
void renderButton(const Button& button, const std::string& text) {
	glBegin(GL_QUADS);
	glVertex2f(button.x / 400.0f - 1.0f, 1.0f - button.y / 400.0f);
	glVertex2f((button.x + button.width) / 400.0f - 1.0f, 1.0f - button.y / 400.0f);
	glVertex2f((button.x + button.width) / 400.0f - 1.0f, 1.0f - (button.y + button.height) / 400.0f);
	glVertex2f(button.x / 400.0f - 1.0f, 1.0f - (button.y + button.height) / 400.0f);
	glEnd();

	//std::cout << text << std::endl;
}


// Render the start screen
void renderStartScreen() {
	glClear(GL_COLOR_BUFFER_BIT);
	renderBackground();
	//renderButton(startButton, "Start Game");
}

// Handle input for the start screen
void handleStartScreenInput(float mouseX, float mouseY, bool mousePressed) {
	if (startButton.isHovered(mouseX, mouseY) && mousePressed) {
		std::cout << "Game Starting..." << std::endl;
		showLoadingScreen = false;
	}
}




int main()
{
	glClearColor(0.0f, 0.0f, 0.3f, 1.0f);


	//building and compiling shader program
	Shader shader("Shaders/vertex_shader.glsl", "Shaders/fragment_shader.glsl");
	Shader sunShader("Shaders/sun_vertex_shader.glsl", "Shaders/sun_fragment_shader.glsl");
	Shader skyboxShader("Shaders/skybox_vertex.glsl", "Shaders/skybox_fragment.glsl");

	glDepthMask(GL_FALSE);
	skyboxShader.use();
	glUniform1i(glGetUniformLocation(skyboxShader.getId(), "skybox"), 0);

	//Textures
	GLuint tex = loadBMP("Resources/Textures/wood.bmp");
	GLuint tex2 = loadBMP("Resources/Textures/rock.bmp");
	GLuint tex3 = loadBMP("Resources/Textures/orange.bmp");
	GLuint texWood = loadBMP("Resources/Textures/woodhut.bmp");
	GLuint tex4 = loadBMP("Resources/Textures/dragon2.bmp");
	GLuint womanTexture = loadBMP("Resources/Textures/WomanCasual2Color.bmp");

	if (womanTexture == 0) {
		std::cerr << "Failed to load woman model texture!" << std::endl;
	}
	else {
		std::cout << "Successfully loaded woman model texture!" << std::endl;
	}
	if (texWood == 0) {
		std::cerr << "Failed to load wood hut texture!" << std::endl;
	}
	else {
		std::cout << "Wood hut texture loaded successfully. ID: " << texWood << std::endl;
	}
	GLuint texOIP = loadBMP("Resources/Textures/OIP.bmp");
	if (texOIP == 0) {
		std::cerr << "Failed to load OIP texture!" << std::endl;
	}
	else {
		std::cout << "OIP texture loaded successfully. ID: " << texOIP << std::endl;
	}
	GLuint texSoil = loadBMP("Resources/Textures/soil.bmp");
	if (texOIP == 0) {
		std::cerr << "Failed to load soil texture!" << std::endl;
	}
	else {
		std::cout << "soil texture loaded successfully. ID: " << texOIP << std::endl;
	}

	GLuint texDragon = loadBMP("Resources/Textures/liz.bmp");
	if (texOIP == 0) {
		std::cerr << "Failed to load dragon texture!" << std::endl;
	}
	else {
		std::cout << "draogn texture loaded successfully. ID: " << texOIP << std::endl;
	}
	GLuint texGun = loadBMP("Resources/Textures/gun.bmp");
	if (texOIP == 0) {
		std::cerr << "Failed to load dragon texture!" << std::endl;
	}
	else {
		std::cout << "draogn texture loaded successfully. ID: " << texOIP << std::endl;
	}

	GLuint texTree = loadBMP("Resources/Textures/treetexture4.bmp"); // Replace with the correct texture file for the tree
	if (texTree == 0) {
		std::cerr << "Failed to load tree texture!" << std::endl;
	}
	else {
		std::cout << "Tree texture loaded successfully." << std::endl;
	}
	GLuint texKey = loadBMP("Resources/Textures/gold.bmp");
	if (texOIP == 0) {
		std::cerr << "Failed to load dragon texture!" << std::endl;
	}
	else {
		std::cout << "draogn texture loaded successfully. ID: " << texOIP << std::endl;
	}
	GLuint texBat = loadBMP("Resources/Textures/bat.bmp");
	if (texOIP == 0) {
		std::cerr << "Failed to load bat texture!" << std::endl;
	}
	else {
		std::cout << "bat texture loaded successfully. ID: " << texOIP << std::endl;
	}



	glEnable(GL_DEPTH_TEST);
	loadBackgroundTexture();  // Load background at startup


	//Test custom mesh loading
	std::vector<Vertex> vert;
	vert.push_back(Vertex());
	vert[0].pos = glm::vec3(10.5f, 10.5f, 0.0f);
	vert[0].textureCoords = glm::vec2(1.0f, 1.0f);

	vert.push_back(Vertex());
	vert[1].pos = glm::vec3(10.5f, -10.5f, 0.0f);
	vert[1].textureCoords = glm::vec2(1.0f, 0.0f);

	vert.push_back(Vertex());
	vert[2].pos = glm::vec3(-10.5f, -10.5f, 0.0f);
	vert[2].textureCoords = glm::vec2(0.0f, 0.0f);

	vert.push_back(Vertex());
	vert[3].pos = glm::vec3(-10.5f, 10.5f, 0.0f);
	vert[3].textureCoords = glm::vec2(0.0f, 1.0f);

	vert[0].normals = glm::normalize(glm::cross(vert[1].pos - vert[0].pos, vert[3].pos - vert[0].pos));
	vert[1].normals = glm::normalize(glm::cross(vert[2].pos - vert[1].pos, vert[0].pos - vert[1].pos));
	vert[2].normals = glm::normalize(glm::cross(vert[3].pos - vert[2].pos, vert[1].pos - vert[2].pos));
	vert[3].normals = glm::normalize(glm::cross(vert[0].pos - vert[3].pos, vert[2].pos - vert[3].pos));

	std::vector<int> ind = { 0, 1, 3,
		1, 2, 3 };

	std::vector<Texture> textures;
	textures.push_back(Texture());
	textures[0].id = tex;
	textures[0].type = "texture_diffuse";

	std::vector<Texture> textures2;
	textures2.push_back(Texture());
	textures2[0].id = tex2;
	textures2[0].type = "texture_diffuse";

	std::vector<Texture> textures3;
	textures3.push_back(Texture());
	textures3[0].id = tex3;
	textures3[0].type = "texture_diffuse";

	std::vector<Texture> textures4;
	textures4.push_back(Texture());
	textures4[0].id = tex4;
	textures4[0].type = "texture_diffuse";


	Mesh mesh(vert, ind, textures3);

	// Create Obj files - easier :)
	// we can add here our textures :)
	MeshLoaderObj loader;
	Mesh sun = loader.loadObj("Resources/Models/sphere.obj");
	Mesh box = loader.loadObj("Resources/Models/cube.obj", textures3);
	Mesh plane = loader.loadObj("Resources/Models/plane1.obj");
	Mesh hut = loader.loadObj("Resources/Models/hut.obj");
	Mesh dragon = loader.loadObj("Resources/Models/Dragon2.obj");
	Mesh maze = loader.loadObj("Resources/Models/maze.obj");
	Mesh plane2 = loader.loadObj("Resources/Models/plane1.obj", textures);
	Mesh monster = loader.loadObj("Resources/Models/dragonel.obj", textures4);
	Mesh bulletModel = loader.loadObj("Resources/Models/sphere.obj", textures3); // Using sphere for bullets
	Mesh gun = loader.loadObj("Resources/Models/gun.obj", textures);
	Mesh orangecube = loader.loadObj("Resources/Models/cube.obj", textures3);
	Mesh key = loader.loadObj("Resources/Models/key.obj");
	Mesh womanModel = loader.loadObj("Resources/Models/WomanCasual2.obj");
	Mesh tree = loader.loadObj("Resources/Models/Hazelnut.obj");
	Mesh bat = loader.loadObj("Resources/Models/bird.obj");


	// Where your other global variables are (above main()):
// Bat parameters
	glm::vec3 batCenter = glm::vec3(-50.0f, 40.0f, 0.0f); // The center around which the bat will circle
	float batRadius = 70.0f;                         // Circle radius
	float batSpeed = 0.5f;                          // Radians per second (angular speed)

	// We'll track the bat's current angle
	float batAngle = 0.0f;

	// This variable will hold the bat's position each frame
	glm::vec3 batPosition;


	//VBO,EBO,VAO SKYBOX
	unsigned int skyboxVAO, skyboxVBO,	skyboxEBO;
	glGenVertexArrays(1, &skyboxVAO);
	glGenBuffers(1, &skyboxVBO);
	glGenBuffers(1, &skyboxEBO);
	glBindVertexArray(skyboxVAO);
	glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, skyboxEBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(skyboxIndices), &skyboxIndices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	//faces of the skybox
	std::vector<std::string> faces{
	"cube_right.png",
	"cube_left.png",
	"cube_up.png",
	"cube_down.png",
	"cube_back.png",
	"cube_front.png"
	};

	unsigned int cubemapTexture;
	glGenTextures(1, &cubemapTexture);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);


	for (unsigned int i = 0; i < 6; i++)
	{
		int width, height, nrChannels;
		unsigned char* data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
		if (data)
		{
			stbi_set_flip_vertically_on_load(false);
			glTexImage2D(
				GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
				0,
				GL_RGB,
				width,
				height,
				0,
				GL_RGB,
				GL_UNSIGNED_BYTE,
				data
			);
			stbi_image_free(data);
			std::cout << "Load image part: " << faces[i] << std::endl;
		}
		else
		{
			std::cout << "Cubemap tex failed to load at path: " << faces[i] << std::endl;
			stbi_image_free(data);
		}
	}
	


	//check if we close the window or press the escape button
	while (!window.isPressed(GLFW_KEY_ESCAPE) &&
		glfwWindowShouldClose(window.getWindow()) == 0)
	{

		window.handleEscapeToMenuInput();
		window.update();
		window.clear();
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		double mouseX, mouseY;
		glfwGetCursorPos(window.getWindow(), &mouseX, &mouseY);
		bool mousePressed = glfwGetMouseButton(window.getWindow(), GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS;

		batAngle += batSpeed*2 * deltaTime;
		float heading = batAngle - 3.14159f / 2.0f; 


		// 2) Compute bat's position in circle
		batPosition.x = batCenter.x + batRadius * cos(batAngle);
		batPosition.z = batCenter.z + batRadius * sin(batAngle);
		batPosition.y = batCenter.y; // if you want it stable at y=10 or so
		
		shader.use();

			processKeyboardInput();
			handleShooting();  // Check bullet collisions with the regular dragons
			updateBarrierState();
			checkMonsterCollision();
			checkMazeExit();  // Detect maze exit


			updateBullets(deltaTime);
			//test mouse input
			if (window.isMousePressed(GLFW_MOUSE_BUTTON_LEFT))
			{
				std::cout << "Pressing mouse button" << std::endl;
			}
			//// Code for the light ////

			sunShader.use();




			glm::mat4 ProjectionMatrix = glm::perspective(90.0f, window.getWidth() * 1.0f / window.getHeight(), 0.1f, 10000.0f);
			glm::mat4 ViewMatrix = glm::lookAt(camera.getCameraPosition(), camera.getCameraPosition() + camera.getCameraViewDirection(), camera.getCameraUp());

			GLuint MatrixID = glGetUniformLocation(sunShader.getId(), "MVP");

			//Test for one Obj loading = light source

			glm::mat4 ModelMatrix = glm::mat4(1.0);
			ModelMatrix = glm::translate(ModelMatrix, lightPos);
			glm::mat4 MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;
			glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);

			sun.draw(sunShader);

			//// End code for the light ////

			shader.use();

			///// Test Obj files for box ////

			GLuint MatrixID2 = glGetUniformLocation(shader.getId(), "MVP");
			GLuint ModelMatrixID = glGetUniformLocation(shader.getId(), "model");

			/*ModelMatrix = glm::mat4(1.0);
			ModelMatrix = glm::translate(ModelMatrix, glm::vec3(0.0f, 0.0f, 0.0f));*/
			MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;
			glUniformMatrix4fv(MatrixID2, 1, GL_FALSE, &MVP[0][0]);
			glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &ModelMatrix[0][0]);
			glUniform3f(glGetUniformLocation(shader.getId(), "lightColor"), lightColor.x, lightColor.y, lightColor.z);
			glUniform3f(glGetUniformLocation(shader.getId(), "lightPos"), lightPos.x, lightPos.y, lightPos.z);
			glUniform3f(glGetUniformLocation(shader.getId(), "viewPos"), camera.getCameraPosition().x, camera.getCameraPosition().y, camera.getCameraPosition().z);

			/*box.draw(shader);*/

			///// Test plane Obj file //////
			float time = glfwGetTime();
			glUniform1f(glGetUniformLocation(shader.getId(), "time"), time);

			ModelMatrix = glm::mat4(1.0);
			ModelMatrix = glm::translate(ModelMatrix, glm::vec3(0.0f, -20.0f, 0.0f));
			MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;
			glUniformMatrix4fv(MatrixID2, 1, GL_FALSE, &MVP[0][0]);
			glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &ModelMatrix[0][0]);
			glActiveTexture(GL_TEXTURE0);  // Activate Texture Unit 0
			glBindTexture(GL_TEXTURE_2D, texSoil);  // Bind the wood texture
			glUniform1i(glGetUniformLocation(shader.getId(), "texture1"), 0);  // Bind texture to sampler

			plane.draw(shader);

			//hut

			ModelMatrix = glm::mat4(1.0);
			ModelMatrix = glm::rotate(ModelMatrix, (0.0f), glm::vec3(1.0f, 0.0f, 0.0f));
			ModelMatrix = glm::scale(ModelMatrix, glm::vec3(2.0f, 2.0f, 2.0f));
			ModelMatrix = glm::translate(ModelMatrix, glm::vec3(-60.0f, -3.0f, 0.0f));
			MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;
			glUniformMatrix4fv(MatrixID2, 1, GL_FALSE, &MVP[0][0]);
			glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &ModelMatrix[0][0]);

			glActiveTexture(GL_TEXTURE0);  // Activate Texture Unit 0
			glBindTexture(GL_TEXTURE_2D, texWood);  // Bind the wood texture
			glUniform1i(glGetUniformLocation(shader.getId(), "texture1"), 0);  // Bind texture to sampler

			hut.draw(shader);
			//maze
			ModelMatrix = glm::mat4(1.0);
			ModelMatrix = glm::mat4(1.0f);
			ModelMatrix = glm::rotate(ModelMatrix, (0.0f), glm::vec3(0.0f, 90.0f, 0.0f));
			ModelMatrix = glm::translate(ModelMatrix, glm::vec3(65.0f, -10.0f, 0.0f)); // Adjust the translation
			ModelMatrix = glm::scale(ModelMatrix, glm::vec3(0.3f, 0.75f, 0.3f)); // Adjust the scale
			MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;
			glUniformMatrix4fv(MatrixID2, 1, GL_FALSE, &MVP[0][0]);
			glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &ModelMatrix[0][0]);
			glActiveTexture(GL_TEXTURE0);  // Activate Texture Unit 0
			glBindTexture(GL_TEXTURE_2D, texOIP);  // Bind the wood texture
			glUniform1i(glGetUniformLocation(shader.getId(), "texture1"), 0);  // Bind texture to sampler

			maze.draw(shader);


			//gun
			if (gunAcquired == false) {
				ModelMatrix = glm::mat4(1.0);
				ModelMatrix = glm::rotate(ModelMatrix, (0.0f), glm::vec3(1.0f, 0.0f, 0.0f));
				ModelMatrix = glm::scale(ModelMatrix, glm::vec3(1.0f, 1.0f, 1.0f));
				ModelMatrix = glm::translate(ModelMatrix, glm::vec3(-131.956f, -2.0f, -51.7117f));
				MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;
				glUniformMatrix4fv(MatrixID2, 1, GL_FALSE, &MVP[0][0]);
				glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &ModelMatrix[0][0]);
				glActiveTexture(GL_TEXTURE0);  // Activate Texture Unit 0
				glBindTexture(GL_TEXTURE_2D, texGun);  // Bind the wood texture
				glUniform1i(glGetUniformLocation(shader.getId(), "texture1"), 0);  // Bind texture to sampler

				gun.draw(shader);
			}
			if (gunAcquired == true) {
				ModelMatrix = glm::mat4(1.0f);

				// Offset the gun position relative to the camera
				glm::vec3 gunOffset = glm::vec3(0.2f, -0.2f, -0.5f); // Adjust offset for "holding" the gun
				glm::vec3 gunPosition = camera.getCameraPosition() + gunOffset;

				// Apply the camera's view direction for the gun's orientation
				glm::mat4 gunOrientation = glm::lookAt(gunPosition, gunPosition + camera.getCameraViewDirection(), camera.getCameraUp());
				gunOrientation = glm::inverse(gunOrientation); // Convert lookAt matrix to a model matrix

				// Apply transformations
				ModelMatrix = gunOrientation;
				ModelMatrix = glm::translate(ModelMatrix, gunOffset); // Fine-tune position in front of the camera
				ModelMatrix = glm::scale(ModelMatrix, glm::vec3(0.1f, 0.1f, 0.1f)); // Scale down the gun model

				// Calculate MVP matrix for the gun
				MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;

				// Send the transformation to the shader
				glUniformMatrix4fv(MatrixID2, 1, GL_FALSE, &MVP[0][0]);
				glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &ModelMatrix[0][0]);
				glActiveTexture(GL_TEXTURE0);  // Activate Texture Unit 0
				glBindTexture(GL_TEXTURE_2D, texGun);  // Bind the wood texture
				glUniform1i(glGetUniformLocation(shader.getId(), "texture1"), 0);  // Bind texture to sampler


				// Render the gun model
				gun.draw(shader);
			}

			//key  X = -117.807, Y = -1.71484, Z = 4.22483
			ModelMatrix = glm::mat4(1.0);
			ModelMatrix = glm::mat4(1.0f);
			ModelMatrix = glm::rotate(ModelMatrix, (0.0f), glm::vec3(0.0f, 90.0f, 0.0f));
			ModelMatrix = glm::translate(ModelMatrix, glm::vec3(-118.806f, -2.91484f, 1.22483f)); // Adjust the translation
			ModelMatrix = glm::scale(ModelMatrix, glm::vec3(1.0f, 1.0f, 1.0f)); // Adjust the scale
			MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;
			glUniformMatrix4fv(MatrixID2, 1, GL_FALSE, &MVP[0][0]);
			glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &ModelMatrix[0][0]);
			glActiveTexture(GL_TEXTURE0);  // Activate Texture Unit 0
			glBindTexture(GL_TEXTURE_2D, texKey);  // Bind the wood texture
			glUniform1i(glGetUniformLocation(shader.getId(), "texture1"), 0);  // Bind texture to sampler
			if (hasKey == false) {
				key.draw(shader);
			}
			//dragons/ monsters
			/*td::vector<glm::vec3> dragonPositions = {
				glm::vec3(-59.3f, -1.0f, 0.0f),
				glm::vec3(-32.0f, -1.0f, 0.0f),
				glm::vec3(-10.0f, -1.0f, 0.0f),
				glm::vec3(-85.0f, -1.0f, 0.0f),
				};


			std::vector<int> dragonHealth(dragonPositions.size(), 100);*/
			// In your main loop or near the top of the file:
			std::vector<glm::vec3> treePositions = {
				glm::vec3(-89.0521f, -11.171484f, -34.5141f),
				glm::vec3(-84.0693f, -11.171484f,  26.1401f),
				glm::vec3(-59.9897f, -11.171484f,  26.2717f),
				glm::vec3(-33.0427f, -11.171484f,  23.5639f),
				glm::vec3(-10.5343f, -11.171484f,  21.3022f),
				glm::vec3(-7.63697f,  -11.171484f, -30.6742f),
				glm::vec3(-32.7458f,  -11.171484f, -32.9304f),
				glm::vec3(-60.0724,  -11.171484f, -39.64f),

				// If there's an 8th position, add it here
			};


			for (const Bullet& bullet : bullets)
			{
				if (!bullet.isActive) continue;

				glm::mat4 ModelMatrix = glm::mat4(1.0f);
				ModelMatrix = glm::translate(ModelMatrix, bullet.position);
				ModelMatrix = glm::scale(ModelMatrix, glm::vec3(0.3f));  // Scale bullet size

				glm::mat4 MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;
				glUniformMatrix4fv(MatrixID2, 1, GL_FALSE, &MVP[0][0]);
				glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &ModelMatrix[0][0]);
				bulletModel.draw(shader);
			}

			float currentTime = glfwGetTime();
			glm::vec3 playerPos = camera.getCameraPosition();
			//shader.use();

			// For each dragon, compute a "swing" along the Z axis
			for (size_t i = 0; i < dragonPositions.size(); i++) {
				float distance = glm::length(dragonPositions[i] - playerPos);

				// Choose a collision radius that feels right (e.g. 3.0f)
				if (distance < 5.0f)
				{
					// Reduce player health 
					playerHealth -= 0.01;  // Or however much damage you want 
					// Possibly add a cooldown or something so it doesn't 
					// drain all your HP in one frame.
				/*	std::cout << "Dragon " << i + 1 << " touches player! Player HP: "
						<< playerHealth << std::endl;*/
					printCameraCoordinates(camera);


					// Check if player is dead
					if (playerHealth <= -300)
					{
						std::cout << "Player has been defeated by dragon collision!"
							<< std::endl;
						//glfwSetWindowShouldClose(window.getWindow(), GLFW_TRUE);
					}
				}
				dragonPositions[i].y = -1.71484;  // So Y matches your bullet or is consistent
				shader.use();
				// Start with the base/original position
				glm::vec3 pos = dragonPositions[i];

				// Add a sine-based offset to the Z value
				// Optionally add "+ i" so each dragon is out of phase
				pos.z += dragonAmplitude * sin(dragonSpeed * currentTime + i);

				// Now build the ModelMatrix using this updated position
				glm::mat4 ModelMatrix = glm::mat4(1.0f);
				ModelMatrix = glm::translate(ModelMatrix, pos);

				ModelMatrix = glm::scale(ModelMatrix, glm::vec3(0.1f, 0.1f, 0.1f));

				ModelMatrix = glm::rotate(ModelMatrix, 180.0f, glm::vec3(0.0f, 1.0f, 0.0f));

				// Standard uniform uploads
				glm::mat4 MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;
				glUniformMatrix4fv(MatrixID2, 1, GL_FALSE, &MVP[0][0]);
				glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &ModelMatrix[0][0]);
				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, texDragon);
				glUniform1i(glGetUniformLocation(shader.getId(), "texture1"), 0);

				// Finally, draw the dragon
				dragon.draw(shader);

				skyboxShader.use();
				glDepthFunc(GL_LEQUAL);
				skyboxShader.use();
				glm::mat4 view = glm::mat4(glm::mat3(camera.getViewMatrix()));
				glm::mat4 projection = glm::perspective(90.0f, window.getWidth() * 1.0f / window.getHeight(), 0.1f, 10000.0f);
				glUniformMatrix4fv(glGetUniformLocation(skyboxShader.getId(), "view"), 1, GL_FALSE, glm::value_ptr(view));
				glUniformMatrix4fv(glGetUniformLocation(skyboxShader.getId(), "projection"), 1, GL_FALSE, glm::value_ptr(projection));

				glBindVertexArray(skyboxVAO);
				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
				glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
				glBindVertexArray(0);
				glDepthFunc(GL_LESS);
				glDepthMask(GL_TRUE);

				ModelMatrix = glm::mat4(1.0f);
				ModelMatrix = glm::translate(ModelMatrix, glm::vec3(60.0231f, -3.6548f, 3.01384f));
				MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;
				glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);
				glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &ModelMatrix[0][0]);
				box.draw(shader);
				
			}


			glm::vec3 playerPosition = camera.getCameraPosition(); // Assuming player position is the camera position
			if (isColliding(playerPosition, boxBoundingBox))
			{
				//std::cout << "Player is touching the box!" << std::endl;
				isNewScene = true;
			}
			if (isNewScene) {
				renderMonsterScene(sunShader, shader, skyboxShader, sun, box, plane2, monster, gun, bulletModel, tex2, skyboxVAO, cubemapTexture, plane, texWood, womanModel, womanTexture);
			}

			shader.use();

			if (!isNewScene) {
					//shader.use();


				glm::mat4 batModel = glm::mat4(1.0f);
				batModel = glm::translate(batModel, batPosition);
				batModel = glm::rotate(batModel, heading, glm::vec3(0.0f, 1.0f, 0.0f));
				batModel = glm::scale(batModel, glm::vec3(0.5f, 0.5f, 0.5f));

				// (C) Send uniforms and draw
				glm::mat4 MVP = ProjectionMatrix * ViewMatrix * batModel;
				glUniformMatrix4fv(MatrixID2, 1, GL_FALSE, glm::value_ptr(MVP));
				glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, glm::value_ptr(batModel));

				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, texBat);
				glUniform1i(glGetUniformLocation(shader.getId(), "texture1"), 0);

				bat.draw(shader);

				

				for (const auto& treePosition : treePositions) {
					float time = glfwGetTime();       // Current time from GLFW
					float amplitude = 60.0f;         // Maximum spin angle in degrees
					float frequency = 2.0f;          // 0.5 Hz means 1 cycle every 2 seconds 
					//    => 1 second rotating left, 
					//       1 second rotating right

					// Compute the angle with a sine wave
					float angle = amplitude * sin(2.0f * 3.14159f * frequency * time);
					glm::mat4 ModelMatrix = glm::mat4(1.0f);
					ModelMatrix = glm::translate(ModelMatrix, treePosition);  // Position the tree
					ModelMatrix = glm::scale(ModelMatrix, glm::vec3(2.0f, 2.0f, 2.0f));  // Adjust tree size

					ModelMatrix = glm::rotate(ModelMatrix, glm::radians(angle), glm::vec3(0.0f, 1.0f, 0.0f));

					glm::mat4 MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;

					glUniformMatrix4fv(MatrixID2, 1, GL_FALSE, &MVP[0][0]);
					glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &ModelMatrix[0][0]);
					/*glActiveTexture(GL_TEXTURE0);*/
					glBindTexture(GL_TEXTURE_2D, texTree);  // Bind the tree texture
					glUniform1i(glGetUniformLocation(shader.getId(), "texture1"), 0);
					tree.draw(shader);
				}
			}

			

			//window.renderGUI();
		

		/*window.update();*/
	}
}
glm::vec3 initialPosition = camera.getCameraPosition(); // Save the initial camera position
void renderGUI() {
	ImGui::Begin("Task Progress");
	ImGui::Text("Current Task: %s", window.taskManager.getCurrentTask().c_str());
	ImGui::End();
}

void processKeyboardInput()
{
	checkMazeExit();  // Check if player reaches the maze exit

	float cameraSpeed = 30 * deltaTime;
	float rotationSpeed = 60.0f * deltaTime;

	glm::vec3 newPos;


	bool isInsideBoundaryCheck = isInsideBoundary();

	if (isInsideBoundaryCheck == false) {
		if (hasKey == false) {
			camera.setCameraPosition(initialPosition);
		}
	}
	
	// Movement with collision detection and boundary restriction
	if (hasKey || isInsideBoundaryCheck) {
		// WASD Movement
		if (window.isPressed(GLFW_KEY_W)) {
			newPos = camera.getCameraPosition() + camera.getCameraViewDirection() * cameraSpeed;
			if (!checkCollision(newPos, hutBoundingBox)) {
				camera.keyboardMoveFront(cameraSpeed);
			}
		}
		if (window.isPressed(GLFW_KEY_S)) {
			newPos = camera.getCameraPosition() - camera.getCameraViewDirection() * cameraSpeed;
			if (!checkCollision(newPos, hutBoundingBox)) {
				camera.keyboardMoveBack(cameraSpeed);
			}
		}
		if (window.isPressed(GLFW_KEY_A)) {
			newPos = camera.getCameraPosition() - camera.getCameraRight() * cameraSpeed;
			if (!checkCollision(newPos, hutBoundingBox)) {
				camera.keyboardMoveLeft(cameraSpeed);
			}
		}
		if (window.isPressed(GLFW_KEY_D)) {
			newPos = camera.getCameraPosition() + camera.getCameraRight() * cameraSpeed;
			if (!checkCollision(newPos, hutBoundingBox)) {
				camera.keyboardMoveRight(cameraSpeed);
			}
		}
	}
	if (window.isPressed(GLFW_KEY_R))
		camera.keyboardMoveUp(cameraSpeed);
	if (window.isPressed(GLFW_KEY_F))
		camera.keyboardMoveDown(cameraSpeed);

	if (window.isPressed(GLFW_KEY_Z)) {
		printCameraCoordinates(camera);
	}
	
	// Shooting trigger (player)
	if (gunAcquired && window.isPressed(GLFW_KEY_SPACE) && glfwGetTime() - lastShotTime > shootingCooldown) {
		Bullet newBullet;
		glm::vec3 bulletOffset = camera.getCameraViewDirection() * 2.0f;
		newBullet.position = camera.getCameraPosition() + bulletOffset;

		// Ensure the bullet always moves forward
		glm::vec3 forwardDirection = glm::normalize(camera.getCameraViewDirection());
		newBullet.direction = forwardDirection;

		newBullet.traveledDistance = 0.0f;
		newBullet.isActive = true;

		bullets.push_back(newBullet);
		lastShotTime = glfwGetTime();

		std::cout << "Bullet Fired at Position: "
			<< newBullet.position.x << ", "
			<< newBullet.position.y << ", "
			<< newBullet.position.z << std::endl;
	}
	else if (!gunAcquired && window.isPressed(GLFW_KEY_SPACE)) {
		std::cout << "You need to acquire the gun to shoot!" << std::endl;
	}

	// Rotation
	if (window.isPressed(GLFW_KEY_LEFT))
		camera.rotateOy(rotationSpeed);
	if (window.isPressed(GLFW_KEY_RIGHT))
		camera.rotateOy(-rotationSpeed);
	  /*if (window.isPressed(GLFW_KEY_UP))
		  camera.rotateOx(rotationSpeed);
	  if (window.isPressed(GLFW_KEY_DOWN))
		  camera.rotateOx(-rotationSpeed);*/
	glm::vec3 gunPosition = glm::vec3(-131.956f, -2.0f, -51.7117f); // Gun position

	if (glm::distance(glm::vec2(camera.getCameraPosition().x, camera.getCameraPosition().z), glm::vec2(gunPosition.x, gunPosition.z)) < 2.5f) {
		// The distance is small enough to consider the player near the gun
		if (window.isPressed(GLFW_KEY_E)) {
			gunAcquired = true;
			std::cout << "Gun acquired!" << std::endl;
		}
	}

	glm::vec3 keyPosition = glm::vec3(-118.806f, -2.91484f, 1.22483f); // Example position of the key

	if (glm::distance(glm::vec2(camera.getCameraPosition().x, camera.getCameraPosition().z), glm::vec2(keyPosition.x, keyPosition.z)) < 5.5f) {
		// The distance is small enough to consider the player near the key
		if (window.isPressed(GLFW_KEY_E)) {
			hasKey = true;
			std::cout << "Key acquired!" << std::endl;
		}
	}
	if (barrierActive && checkCollision(camera.getCameraPosition(), barrierBoundingBox)) {
		camera.setCameraPosition(initialPosition); // Reset to previous position if blocked by barrier
	}
	if (hasKey && !gunAcquired) {
		window.taskManager.completeCurrentTask(0);  // Key found
	}
	if (gunAcquired && !allMonstersDead) {
		window.taskManager.completeCurrentTask(1);  // Gun acquired
	}
	//if (allMonstersDead && !mazecomplete) {
	//	window.taskManager.completeCurrentTask(2);  // Monsters killed
	//	mazecomplete = true;
	//}
	if (mazecomplete && isMonsterAlive == false && isLastScene == false) {
		window.taskManager.completeCurrentTask(3);  // Reached the maze exit
	}
	if (isMonsterAlive == false && isLastScene) {
		window.taskManager.completeCurrentTask(4);  // Final boss killed
	}
 

}

void renderMonsterScene(Shader& sunShader, Shader& shader, Shader& skyboxShader, Mesh& sun, Mesh& box, Mesh& plane2, Mesh& monster, Mesh& gun, Mesh& bulletModel, GLuint tex, unsigned int& skyboxVAO, unsigned int& cubemapTexture, Mesh& plane, GLuint texWood, Mesh& womanModel, GLuint womanTexture) {
	window.clear();
	glClearColor(0.502f, 0.502f, 0.502f, 1.0f);
	float currentFrame = glfwGetTime();
	//deltaTime = currentFrame - lastFrame;
	//lastFrame = currentFrame;

	processKeyboardInput();
	
	// Update shooting mechanics
	handleShooting();
	updateBullets(deltaTime);

	// Update monster behavior only if it's alive
	if (isMonsterAlive)
	{
		moveMonsterTowardPlayer(deltaTime);
		handleMonsterShooting(deltaTime);  // Monster shooting logic
		handleMonsterAttack();  // Check if monster attacks the player
		updateMonsterBullets(deltaTime);  // Update monster's bullets
	}
	if (isMonsterAlive == false) {
		isLastScene = true;
	}

	if (isLastScene) {
		renderVictoryScene(shader, plane, texWood, womanModel, womanTexture);
	}

	if (isMonsterAlive)
	{
		// Render light
		sunShader.use();

		glm::mat4 ProjectionMatrix = glm::perspective(90.0f, window.getWidth() * 1.0f / window.getHeight(), 0.1f, 10000.0f);
		glm::mat4 ViewMatrix = glm::lookAt(camera.getCameraPosition(), camera.getCameraPosition() + camera.getCameraViewDirection(), camera.getCameraUp());

		GLuint MatrixID = glGetUniformLocation(sunShader.getId(), "MVP");

		// Light source (sun) rendering
		glm::mat4 ModelMatrix = glm::mat4(1.0f);
		ModelMatrix = glm::translate(ModelMatrix, lightPos);
		glm::mat4 MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;
		glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);
		sun.draw(sunShader);

		// Render the player and environment (box, plane)
		shader.use();
		GLuint MatrixID2 = glGetUniformLocation(shader.getId(), "MVP");
		GLuint ModelMatrixID = glGetUniformLocation(shader.getId(), "model");

		// Box rendering (Player)
		ModelMatrix = glm::mat4(1.0f);
		ModelMatrix = glm::translate(ModelMatrix, camera.getCameraPosition());
		MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;
		glUniformMatrix4fv(MatrixID2, 1, GL_FALSE, &MVP[0][0]);
		glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &ModelMatrix[0][0]);
		glUniform3f(glGetUniformLocation(shader.getId(), "lightColor"), lightColor.x, lightColor.y, lightColor.z);
		glUniform3f(glGetUniformLocation(shader.getId(), "lightPos"), lightPos.x, lightPos.y, lightPos.z);
		glUniform3f(glGetUniformLocation(shader.getId(), "viewPos"), camera.getCameraPosition().x, camera.getCameraPosition().y, camera.getCameraPosition().z);
		//box.draw(shader);

		// Plane rendering
		ModelMatrix = glm::mat4(1.0f);
		ModelMatrix = glm::translate(ModelMatrix, glm::vec3(0.0f, -20.0f, 0.0f));
		MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;
		glUniformMatrix4fv(MatrixID2, 1, GL_FALSE, &MVP[0][0]);
		glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &ModelMatrix[0][0]);
		plane2.draw(shader);

		// Render monster only if it's alive
		if (isMonsterAlive)
		{
			glm::vec3 redColor(1.0f, 0.0f, 0.0f);
			renderMonsterHealthBar(shader, redColor);
			ModelMatrix = glm::mat4(1.0f);
			ModelMatrix = glm::translate(ModelMatrix, monsterPosition);
			ModelMatrix = glm::rotate(ModelMatrix, 180.0f, glm::vec3(0.0f, 1.0f, 0.0f));
			MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;
			glUniformMatrix4fv(MatrixID2, 1, GL_FALSE, &MVP[0][0]);
			glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &ModelMatrix[0][0]);
			monster.draw(shader);
		}


		// Render bullets
		for (const Bullet& bullet : bullets)
		{
			if (!bullet.isActive) continue;

			ModelMatrix = glm::mat4(1.0f);
			ModelMatrix = glm::translate(ModelMatrix, bullet.position);
			MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;
			glUniformMatrix4fv(MatrixID2, 1, GL_FALSE, &MVP[0][0]);
			glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &ModelMatrix[0][0]);
			bulletModel.draw(shader);
		}

		// Render monster bullets
		for (const Bullet& bullet : monsterBullets)
		{
			if (!bullet.isActive) continue;

			ModelMatrix = glm::mat4(1.0f);
			ModelMatrix = glm::translate(ModelMatrix, bullet.position);
			MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;
			glUniformMatrix4fv(MatrixID2, 1, GL_FALSE, &MVP[0][0]);
			glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &ModelMatrix[0][0]);
			bulletModel.draw(shader);
		}

		// Render the gun near the camera as if the player is holding it
		ModelMatrix = glm::mat4(1.0f);

		// Offset the gun position relative to the camera
		glm::vec3 gunOffset = glm::vec3(0.2f, -0.2f, -0.5f); // Adjust offset for "holding" the gun
		glm::vec3 gunPosition = camera.getCameraPosition() + gunOffset;

		// Apply the camera's view direction for the gun's orientation
		glm::mat4 gunOrientation = glm::lookAt(gunPosition, gunPosition + camera.getCameraViewDirection(), camera.getCameraUp());
		gunOrientation = glm::inverse(gunOrientation); // Convert lookAt matrix to a model matrix

		// Apply transformations
		ModelMatrix = gunOrientation;
		ModelMatrix = glm::translate(ModelMatrix, gunOffset); // Fine-tune position in front of the camera
		ModelMatrix = glm::scale(ModelMatrix, glm::vec3(0.1f, 0.1f, 0.1f)); // Scale down the gun model

		// Calculate MVP matrix for the gun
		MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;

		// Send the transformation to the shader
		glUniformMatrix4fv(MatrixID2, 1, GL_FALSE, &MVP[0][0]);
		glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &ModelMatrix[0][0]);


		// Render the gun model
		gun.draw(shader);
	}

	skyboxShader.use();
	glDepthFunc(GL_LEQUAL);
	skyboxShader.use();
	glm::mat4 view = glm::mat4(glm::mat3(camera.getViewMatrix()));
	glm::mat4 projection = glm::perspective(90.0f, window.getWidth() * 1.0f / window.getHeight(), 0.1f, 10000.0f);
	glUniformMatrix4fv(glGetUniformLocation(skyboxShader.getId(), "view"), 1, GL_FALSE, glm::value_ptr(view));
	glUniformMatrix4fv(glGetUniformLocation(skyboxShader.getId(), "projection"), 1, GL_FALSE, glm::value_ptr(projection));

	glBindVertexArray(skyboxVAO);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
	glDepthFunc(GL_LESS);
	glDepthMask(GL_TRUE);
	// Update window 
	//window.update();
}


void handleShooting()
{

	for (Bullet& bullet : bullets)
	{
		if (!bullet.isActive) continue;

		// Check for collision with the main monster (boss)
		glm::vec3 directionToMonster = monsterPosition - bullet.position;

		if (glm::length(directionToMonster) < 5.0f)  // Boss collision
		{
			bullet.isActive = false;
			if (isNewScene)
			{
				if (isMonsterAlive)
				{
					monsterHealth -= shootingDamage;
					std::cout << "Monster hit! Monster health: " << monsterHealth << std::endl;

					if (monsterHealth <= 0)
					{
						isMonsterAlive = false;
						isNewScene = true;
						std::cout << "The monster has been defeated!" << std::endl;
						window.taskManager.completeCurrentTask(4);
						if (window.taskManager.allTasksCompleted())
						{
							std::cout << "All tasks completed!" << std::endl;
						}

					}

				}
			}
		}
		
		// Check for collision with dragons before the maze
		for (size_t i = 0; i < dragonPositions.size(); i++)
		{
			dragonPositions[i].y = -1.71484;
			glm::vec3 directionToDragon = dragonPositions[i] - bullet.position;
			float distance = glm::length(directionToDragon);
			//std::cout << "Bullet-Dragon " << i + 1 << " Distance: " << distance << std::endl;

			if (distance < 10.0f && std::fabs(bullet.position.y - dragonPositions[i].y) < 3.0f)
			{
				bullet.isActive = false;
				dragonHealth[i] -= shootingDamage;

				std::cout << "Dragon " << i + 1 << " hit! Health: " << dragonHealth[i] << std::endl;

				if (dragonHealth[i] <= 0)
				{
					std::cout << "Dragon " << i + 1 << " has been eliminated!" << std::endl;
					dragonPositions.erase(dragonPositions.begin() + i);
					dragonHealth.erase(dragonHealth.begin() + i);
					i--;  // Prevent skipping dragons
				}
			}
		}
		// Check if all dragons are dead
		if (dragonPositions.empty())
		{
			allMonstersDead = true;
			//std::cout << "All dragons are dead! allMonstersDead = true" << std::endl;
			//X = 51.9082, Y = -1.71484, Z = -12.4834
			camera.setCameraPosition(glm::vec3(51.9082f, -1.71484f, -12.4834f));
			window.taskManager.completeCurrentTask(2);
			//mazecomplete = true;


		}
		else
		{
			allMonstersDead = false;
		}
	}
}


void updateBullets(float deltaTime)
{
	for (Bullet& bullet : bullets)
	{
		if (!bullet.isActive) continue;

		// Move bullet forward
		bullet.position += bullet.direction * 100.0f * deltaTime;
		bullet.traveledDistance += glm::length(bullet.direction * 100.0f * deltaTime);

		// Deactivate if too far
		if (bullet.traveledDistance > maxBulletRange)
		{
			bullet.isActive = false;
		}
	}
}

void moveMonsterTowardPlayer(float deltaTime)
{
	glm::vec3 playerPosition = camera.getCameraPosition();
	glm::vec3 directionToPlayer = glm::normalize(playerPosition - monsterPosition);
	monsterPosition += directionToPlayer * monsterSpeed * deltaTime;
}

void handleMonsterShooting(float deltaTime)
{
	if (glfwGetTime() - lastMonsterShotTime > monsterShootingCooldown)
	{
		// Add a new bullet
		Bullet newBullet;
		newBullet.position = monsterPosition;
		glm::vec3 directionToPlayer = camera.getCameraPosition() - monsterPosition;
		newBullet.direction = glm::normalize(directionToPlayer);
		newBullet.traveledDistance = 0.0f;
		newBullet.isActive = true;

		monsterBullets.push_back(newBullet);
		lastMonsterShotTime = glfwGetTime();
	}
}

void updateMonsterBullets(float deltaTime)
{
	for (Bullet& bullet : monsterBullets)
	{
		if (!bullet.isActive) continue;

		// Move bullet toward player
		bullet.position += bullet.direction * 100.0f * deltaTime;
		bullet.traveledDistance += glm::length(bullet.direction * 100.0f * deltaTime);

		// Deactivate if too far
		if (bullet.traveledDistance > maxBulletRange)
		{
			bullet.isActive = false;
		}

		// Check if monster's bullet hits the player
		glm::vec3 directionToPlayer = camera.getCameraPosition() - bullet.position;
		if (glm::length(directionToPlayer) < 5.0f)  // Approximation for collision
		{
			bullet.isActive = false;
			playerHealth -= monsterBulletDamage;
			std::cout << "Player hit by monster bullet! Player health: " << playerHealth << std::endl;


			if (playerHealth <= 0)
			{
				std::cout << "Player has been defeated!" << std::endl;
				glfwSetWindowShouldClose(window.getWindow(), GLFW_TRUE); // End game
			}
		}
	}
}

void handleMonsterAttack()
{
	glm::vec3 playerPosition = camera.getCameraPosition();
	float distanceToPlayer = glm::length(monsterPosition - playerPosition);

	// If the monster is close enough, it attacks
	if (distanceToPlayer < monsterAttackRange)
	{
		playerHealth -= monsterAttackDamage;
		std::cout << "Player hit! Player health: " << playerHealth << std::endl;

		if (playerHealth <= 0)
		{
			std::cout << "Player has been defeated!" << std::endl;
			glfwSetWindowShouldClose(window.getWindow(), GLFW_TRUE); // End game
		}
	}
}

void renderVictoryScene(Shader& shader, Mesh& plane, GLuint texWood, Mesh& womanModel, GLuint womanTexture) {
	shader.use();
	glClearColor(0.4f, 0.698f, 1.0f, 1.0f);
	// Plane for new scene (wood floor)
	glm::mat4 ModelMatrix = glm::mat4(1.0f);
	ModelMatrix = glm::translate(ModelMatrix, glm::vec3(0.0f, -9.0f, 0.0f));
	glm::mat4 ProjectionMatrix = glm::perspective(90.0f, window.getWidth() * 1.0f / window.getHeight(), 0.1f, 10000.0f);
	glm::mat4 ViewMatrix = glm::lookAt(camera.getCameraPosition(), camera.getCameraPosition() + camera.getCameraViewDirection(), camera.getCameraUp());
	glm::mat4 MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;
	GLuint MatrixID = glGetUniformLocation(shader.getId(), "MVP");
	GLuint ModelMatrixID = glGetUniformLocation(shader.getId(), "model");
	glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);
	glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &ModelMatrix[0][0]);
	plane.draw(shader);

	// Render the woman model
	ModelMatrix = glm::mat4(1.0f);
	ModelMatrix = glm::translate(ModelMatrix, glm::vec3(2.0f, 6.0f, 0.0f)); 
	ModelMatrix = glm::rotate(ModelMatrix, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	ModelMatrix = glm::scale(ModelMatrix, glm::vec3(6.0f, 6.0f, 6.0f));
	MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;
	glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);
	glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &ModelMatrix[0][0]);

	// Bind the woman texture
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, womanTexture);
	glUniform1i(glGetUniformLocation(shader.getId(), "texture1"), 0); // Texture unit 0

	// Draw the woman model
	womanModel.draw(shader);

	glm::vec3 womanPos(2.0f, 6.0f, 0.0f);             
	glm::vec3 cameraPos = camera.getCameraPosition(); 
	float distance = glm::distance(cameraPos, womanPos);

	//If close enough, close the game
	float threshold = 8.0f; // pick a suitable threshold
	if (distance < threshold) {
		// Close the game/window
		glfwSetWindowShouldClose(window.getWindow(), GLFW_TRUE);
	}
}

void renderMonsterHealthBar(Shader& shader, const glm::vec3& color) {
	float barWidth = 10.0f;
	float barHeight = 1.0f;
	float healthPercentage = monsterHealth / 100.0f;

	// Calculate health bar position and scale
	glm::mat4 ModelMatrix = glm::mat4(1.0f);
	glm::vec3 barPosition = monsterPosition + glm::vec3(0.0f, 10.0f, 0.0f); // Position above the monster
	ModelMatrix = glm::translate(ModelMatrix, barPosition);
	ModelMatrix = glm::scale(ModelMatrix, glm::vec3(barWidth * healthPercentage, barHeight, 1.0f));

	// View and projection matrices
	glm::mat4 ProjectionMatrix = glm::perspective(90.0f, window.getWidth() * 1.0f / window.getHeight(), 0.1f, 10000.0f);
	glm::mat4 ViewMatrix = glm::lookAt(camera.getCameraPosition(), camera.getCameraPosition() + camera.getCameraViewDirection(), camera.getCameraUp());
	glm::mat4 MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;

	// Pass the MVP matrix to the shader
	GLuint MatrixID = glGetUniformLocation(shader.getId(), "MVP");
	glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);

	// Pass the health bar color to the shader
	GLuint colorUniformID = glGetUniformLocation(shader.getId(), "overrideColor");
	glUniform3f(colorUniformID, color.r, color.g, color.b);

	// Render the health bar as a quad
	glBegin(GL_QUADS);
	glVertex3f(-0.5f, 0.5f, 0.0f);  // Top-left
	glVertex3f(0.5f, 0.5f, 0.0f);   // Top-right
	glVertex3f(0.5f, -0.5f, 0.0f);  // Bottom-right
	glVertex3f(-0.5f, -0.5f, 0.0f); // Bottom-left
	glEnd();
}


//STOP UNDOING TILL HERE """
//STOP UNDOING
//STOOOOOOOOOOOOOOOOOP UNDOU