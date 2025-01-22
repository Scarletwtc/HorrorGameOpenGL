#include "Graphics\window.h"
#include "Camera\camera.h"
#include "Shaders\shader.h"
#include "Model Loading\mesh.h"
#include "Model Loading\texture.h"
#include "Model Loading\meshLoaderObj.h"


void processKeyboardInput ();

float deltaTime = 0.0f;	// time between current frame and last frame
float lastFrame = 0.0f;

Window window("Game Engine", 800, 800);
Camera camera(glm::vec3(0.0f, 0.0f, 10.0f)); 

glm::vec3 lightColor = glm::vec3(1.0f);
glm::vec3 lightPos = glm::vec3(-180.0f, 100.0f, -200.0f);

struct BoundingBox {
	glm::vec3 min;
	glm::vec3 max;
};

BoundingBox hutBoundingBox = {
	glm::vec3(-4.0f, -3.0f, -4.0f),  // Minimum corner
	glm::vec3(4.0f, 6.0f, 4.0f)      // Maximum corner
};

bool checkCollision(glm::vec3 newPosition, BoundingBox box) {
	return (newPosition.x >= box.min.x && newPosition.x <= box.max.x &&
		newPosition.y >= box.min.y && newPosition.y <= box.max.y &&
		newPosition.z >= box.min.z && newPosition.z <= box.max.z);
}

int main()
{
	glClearColor(0.2f, 0.8f, 1.0f, 1.0f);

	//building and compiling shader program
	Shader shader("Shaders/vertex_shader.glsl", "Shaders/fragment_shader.glsl");
	Shader sunShader("Shaders/sun_vertex_shader.glsl", "Shaders/sun_fragment_shader.glsl");

	//Textures
	GLuint tex = loadBMP("Resources/Textures/wood.bmp");
	GLuint tex2 = loadBMP("Resources/Textures/rock.bmp");
	GLuint tex3 = loadBMP("Resources/Textures/orange.bmp");
	GLuint texWood = loadBMP("Resources/Textures/woodhut.bmp");
	if (texWood == 0) {
		std::cerr << "Failed to load wood hut texture!" << std::endl;
	}
	else {
		std::cout << "Wood hut texture loaded successfully. ID: " << texWood << std::endl;
	}



	//glEnable(GL_DEPTH_TEST);

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


	Mesh mesh(vert, ind, textures3);

	// Create Obj files - easier :)
	// we can add here our textures :)
	MeshLoaderObj loader;
	Mesh sun = loader.loadObj("Resources/Models/sphere.obj");
	Mesh box = loader.loadObj("Resources/Models/cube.obj", textures);
	Mesh plane = loader.loadObj("Resources/Models/plane1.obj", textures2);
	Mesh hut = loader.loadObj("Resources/Models/hut.obj");
	Mesh fish = loader.loadObj("Resources/Models/fish.obj");



	//check if we close the window or press the escape button
	while (!window.isPressed(GLFW_KEY_ESCAPE) &&
		glfwWindowShouldClose(window.getWindow()) == 0)
	{
		window.clear();
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		processKeyboardInput();

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

		plane.draw(shader);

		//hut

		ModelMatrix = glm::mat4(1.0);
		ModelMatrix = glm::rotate(ModelMatrix, (0.0f), glm::vec3(1.0f, 0.0f, 0.0f)); 
		ModelMatrix = glm::scale(ModelMatrix, glm::vec3(5.0f, 5.0f, 5.0f));

		ModelMatrix = glm::translate(ModelMatrix, glm::vec3(0.0f, -3.0f, 0.0f));
		MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;
		glUniformMatrix4fv(MatrixID2, 1, GL_FALSE, &MVP[0][0]);
		glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &ModelMatrix[0][0]);

		glActiveTexture(GL_TEXTURE0);  // Activate Texture Unit 0
		glBindTexture(GL_TEXTURE_2D, texWood);  // Bind the wood texture
		glUniform1i(glGetUniformLocation(shader.getId(), "texture1"), 0);  // Bind texture to sampler





		hut.draw(shader);

		//fish
		std::vector<glm::vec3> fishPositions = {
			glm::vec3(-10.0f, 0.0f, 50.0f),
			glm::vec3(10.0f, 0.0f, 70.0f),
			glm::vec3(20.0f, 5.0f, 40.0f),
			glm::vec3(30.0f, -5.0f, 60.0f),
			glm::vec3(25.0f, 10.0f, 80.0f)
			};

		for (const auto& pos : fishPositions) {
			ModelMatrix = glm::mat4(1.0f);
			ModelMatrix = glm::translate(ModelMatrix, pos);
			ModelMatrix = glm::scale(ModelMatrix, glm::vec3(0.9f, 0.9f, 0.9f)); 
			MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;
			glUniformMatrix4fv(MatrixID2, 1, GL_FALSE, &MVP[0][0]);
			glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &ModelMatrix[0][0]);

			fish.draw(shader);
		}

		

		window.update();
	}
}void processKeyboardInput()
{
    float cameraSpeed = 30 * deltaTime;
    float rotationSpeed = 20.0f * deltaTime;

    glm::vec3 newPos;

    // Movement with collision detection
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
    if (window.isPressed(GLFW_KEY_R))
        camera.keyboardMoveUp(cameraSpeed);
    if (window.isPressed(GLFW_KEY_F))
        camera.keyboardMoveDown(cameraSpeed);

    // Rotation
    if (window.isPressed(GLFW_KEY_LEFT))
        camera.rotateOy(rotationSpeed);
    if (window.isPressed(GLFW_KEY_RIGHT))
        camera.rotateOy(-rotationSpeed);
    if (window.isPressed(GLFW_KEY_UP))
        camera.rotateOx(rotationSpeed);
    if (window.isPressed(GLFW_KEY_DOWN))
        camera.rotateOx(-rotationSpeed);
}