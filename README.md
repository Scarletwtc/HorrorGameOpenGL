Readme File

Name of the game: 
  Free the Truth. Escape the maze

Team Members:
•	Comeagă Ana-Maria, 1231B
•	El-Ghoul Layla, 1231B
•	Petre George-Alexandru, 1231B
•	Urîtu Andra-Ioana, 1231B

Description:
	In Free the Truth: Escape the Maze, players step into the shoes of a young girl who awakens in a hauntingly strange and desolate world, with no memories of her past or how she ended up in such a horrifying place. She finds herself trapped in an abandoned barn, deep within a fog-covered, eerie forest. Her first task is to escape this unsettling barn and make her way back to the outside world. The only thing she can remember is being with her friend before everything went dark.
Upon her escape, she discovers that the forest is teeming with monstrous creatures, all bent on her destruction. Armed with the ability to defend herself, she must fight off these terrifying monsters as she navigates the deadly terrain. As she ventures deeper into the forest, the girl is led into a perilous maze—one that seems to go on forever, full of even deadlier enemies, twisted paths, and ominous traps.
The climax of her journey culminates in a final showdown with a monstrous boss, guarding the key to saving her friend. In order to unlock the truth and rescue her friend before it's too late, she must defeat the boss and uncover the dark secrets that bind them both. Time is running out, and failure means her friend’s sacrifice to an unknown evil force.
Will she escape the maze, uncover the truth about her past, and save her friend, or will the dark forces of the forest claim her and her companion forever?

Objective:
	The main objective is to escape the barn, survive the monstrous creatures in the foggy forest, navigate the deadly maze, and defeat the final boss to rescue the girl’s friend. Time is running out, if she fails, her friend will be sacrificed to an evil force, and the truth will remain locked forever.


Code explanations:
	This game demonstrates a simple 2D interactive scene using OpenGL, where:
•	The player navigates a square (squarePosition) to achieve goals like reaching a house or dodging obstacles.
•	Gameplay elements like flames, monsters, and textures add variety and challenges.
•	The game resets upon loss (youLost) or by pressing the restart key.

Dependencies and Libraries:
o	GLFW: Manages the game window and input handling.
o	GLEW: Handles OpenGL extensions.
o	GLM: Provides mathematical operations for transformations (e.g., positions and rotations).
o	stb_image.h: Used to load textures from image files.
Game Elements:
o	Player Square: Represented as a moving object controlled by the player with keyboard inputs (A/D for left/right).
o	Backgrounds: Textures managed by the backgroundTextures array, switching dynamically based on game state.
o	Flames: A projectile system that activates when the player clicks the mouse.
o	Monsters: Obstacles that move or remain static. Their positions reset after each game reset.
o	House: Represents the player's starting point and includes visual details like windows, roof, and a door.
Controls:
o	A/D: Move the player square left or right.
o	SPACE: Reset the game after a loss.
o	Mouse Click: Fires a flame towards a clicked position.
Rendering:
o	Background: Dynamically renders different scenes using textures.
o	Square and House: Rendered using vertices and indices, with hardcoded positions and colors.
o	Game Over: Displays a "Game Over" screen using a separate texture.
Game Logic:
o	Collision detection (not shown in this snippet) could determine whether the player interacts with flames, monsters, or the house.
o	Lives (counter) and game status (youLost) track progress.
Restart Logic:
o	The resetGame() function reinitializes all game variables, such as the player's position, monster positions, and backgrounds.
• loadTexture(const char* filepath): Loads and binds a texture to be used for rendering backgrounds or other game objects.
• resetGame(): Resets all key game variables for a fresh start after the player loses.
• Rendering Functions:
•	renderBackground(GLuint programID): Draws the current background texture.
•	renderSquare(GLuint programID): Draws the movable square controlled by the player.
•	renderHouse(GLuint programID): Draws the goal, including a detailed house with windows, a door, and a roof.
• Input Handling:
•	key_callback: Handles keyboard inputs for movement and restarting the game.
•	mouse_button_callback: Tracks mouse clicks for flame activation and determines its trajectory.
• Flame Activation: A simple projectile system where flames are activated, aimed, and moved in the direction of the mouse click.



