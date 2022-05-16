#include <SFML/Graphics.hpp>
#include <sstream>
#include <SFML/Audio.hpp>

using namespace sf;

// Function declaration
void updateBranches(int seed);
const int NUM_BRANCHES = 6;
Sprite branches[NUM_BRANCHES];
// Where is the player/branch?
// Left or right
enum class side { LEFT, RIGHT, NONE };
side branchPositions[NUM_BRANCHES];

int main() {
	// Create a video mode object
	VideoMode vm(1920, 1080);
	// Create and open a window for the game
	RenderWindow window(vm, "Timber!!!", Style::Fullscreen);

	// Create a texture to hold a graphic on the GPU
	Texture textureBackground;
	// Load a graphic into the texture
	textureBackground.loadFromFile("graphics/background.png");
	// Create a sprite
	Sprite spriteBackground;
	// Attach the texture to the sprite
	spriteBackground.setTexture(textureBackground);
	// Set the spriteBackground to cover the screen
	spriteBackground.setPosition(0, 0);

	// Make a tree sprite
	Texture textureTree;
	textureTree.loadFromFile("graphics/tree.png");
	Sprite spriteTree;
	spriteTree.setTexture(textureTree);
	spriteTree.setPosition(810, 0);

	// Prepare the bee
	Texture textureBee;
	textureBee.loadFromFile("graphics/my_axe.png");
	Sprite spriteBee;
	spriteBee.setTexture(textureBee);
	spriteBee.setPosition(0, 800);
	spriteBee.setScale(10, 10);
	// Is the bee currently moving?
	bool beeActive = false;
	// How fast can the bee fly
	float beeSpeed = 0.0f;

	// Make 3 cloud sprites from 1 texture
	Texture textureCloud;
	textureCloud.loadFromFile("graphics/cloud.png");

	// refactor to store all clouds in an array instead
	Sprite Clouds[3];
	bool cloudActive[3];
	float cloudSpeed[3];

	for (int i = 0; i < 3; i++) {
		Clouds[i].setTexture(textureCloud);
		Clouds[i].setPosition(0, i * 250);
		cloudActive[i] = false;
		cloudSpeed[i] = 0.0f;
	}

	// Variables to control time itself
	Clock clock;
	// Time bar
	RectangleShape timeBar;
	float timeBarStartWidth = 400;
	float timeBarHeight = 80;
	timeBar.setSize(Vector2f(timeBarStartWidth, timeBarHeight));
	timeBar.setFillColor(Color::Red);
	timeBar.setPosition((1920 / 2) - timeBarStartWidth / 2, 980);
	Time gameTimeTotal;
	float timeRemaining = 6.0f;
	float timeBarWidthPerSecond = timeBarStartWidth / timeRemaining;
	// Track whether the game is running
	bool paused = true;

	// draw some text
	int score = 0;
	int fps = 0;
	float oneSecond = 0.0f;
	Text messageText;
	Text scoreText;
	Text fpsText;
	// We need to choose a font
	Font font;
	font.loadFromFile("fonts/KOMIKAP_.ttf");
	// Set the font to our message
	messageText.setFont(font);
	scoreText.setFont(font);
	fpsText.setFont(font);
	// Assign the actual message
	messageText.setString("Press Enter to start!");
	scoreText.setString("Score = 0");
	fpsText.setString("FPS: 9999");
	// Make it really big
	messageText.setCharacterSize(75);
	scoreText.setCharacterSize(100);
	fpsText.setCharacterSize(75);
	// Choose a colour
	messageText.setFillColor(Color::White);
	scoreText.setFillColor(Color::White);
	fpsText.setFillColor(Color::White);
	// Position the text
	FloatRect textRect = messageText.getLocalBounds();
	messageText.setOrigin(
		textRect.left + textRect.width / 2.0f,
		textRect.top + textRect.height / 2.0f
	);
	messageText.setPosition(1920 / 2.0f, 1080 / 2.0f);
	scoreText.setPosition(20, 20);
	// Set fpsText origin to top right
	FloatRect fpsRect = fpsText.getLocalBounds();
	fpsText.setOrigin(
		fpsRect.width, 0
	);
	fpsText.setPosition(1850, 20);
	fpsText.setString("FPS : ");
	// Add some background to the text
	RectangleShape scoreBackground;
	RectangleShape fpsBackground;
	fpsBackground.setSize(Vector2f(
		fpsRect.width + 40.0f,
		fpsRect.height + 40.0f
	));
	fpsBackground.setFillColor(Color(0, 0, 0, 128));
	fpsBackground.setOrigin(Vector2f(
		fpsRect.width + 20.f,
		0.0f
	));
	fpsBackground.setPosition(Vector2f(
		fpsText.getPosition().x,
		fpsText.getPosition().y
	));
	// Set up the score background
	FloatRect scoreRect = scoreText.getLocalBounds();
	scoreBackground.setSize(Vector2f(
		scoreRect.width + 40.0f,
		scoreRect.height + 40.0f
	));
	scoreBackground.setFillColor(Color(0, 0, 0, 128));
	/*scoreBackground.setOrigin(Vector2f(
		scoreRect.width + 20.f,
		0.0f
	));*/
	scoreBackground.setPosition(Vector2f(
		scoreText.getPosition().x,
		scoreText.getPosition().y
	));


	// Prepare 6 branches
	Texture textureBranch;
	textureBranch.loadFromFile("graphics/branch.png");
	// Set the texture for each branch sprite
	for (int i = 0; i < NUM_BRANCHES; i++) {
		branches[i].setTexture(textureBranch);
		branches[i].setPosition(-2000, -2000);
		// Set the sprites origin to the dead centre
		// We can then spin it round without changing its position
		branches[i].setOrigin(220, 20);
	}

	// Prepare the player
	Texture texturePlayer;
	texturePlayer.loadFromFile("graphics/player.png");
	Sprite spritePlayer;
	spritePlayer.setTexture(texturePlayer);
	spritePlayer.setPosition(580, 720);
	// The player starts on the left
	side playerSide = side::LEFT;
	// Prepare the gravestone
	Texture textureRIP;
	textureRIP.loadFromFile("graphics/rip.png");
	Sprite spriteRIP;
	spriteRIP.setTexture(textureRIP);
	spriteRIP.setPosition(600, 800);
	// Prepare the axe
	Texture textureAxe;
	textureAxe.loadFromFile("graphics/axe.png");
	Sprite spriteAxe;
	spriteAxe.setTexture(textureAxe);
	spriteAxe.setPosition(700, 830);
	// Line the axe up with the tree
	const float AXE_POSITION_LEFT = 700;
	const float AXE_POSITION_RIGHT = 1075;
	// Prepare the flying log
	Texture textureLog;
	textureLog.loadFromFile("graphics/log.png");
	Sprite spriteLog;
	spriteLog.setTexture(textureLog);
	spriteLog.setPosition(810, 720);
	// some other useful log related variables
	bool logActive = false;
	float logSpeedX = 1000;
	float logSpeedY = -1500;
	// Control the player input
	bool acceptInput = false;
	// Prepare the sounds
	// The player chopping sound
	SoundBuffer chopBuffer;
	chopBuffer.loadFromFile("sound/chop.wav");
	Sound chop;
	chop.setBuffer(chopBuffer);
	// The player has met his end under a branch
	SoundBuffer deathBuffer;
	deathBuffer.loadFromFile("sound/death.wav");
	Sound death;
	death.setBuffer(deathBuffer);
	// Out of time
	SoundBuffer ootBuffer;
	ootBuffer.loadFromFile("sound/out_of_time.wav");
	Sound outOftime;
	outOftime.setBuffer(ootBuffer);


	while (window.isOpen()) {
		/*
		**********************************
		Handle the players input
		**********************************
		*/
		Event event;
		while (window.pollEvent(event)) {
			if (event.type == Event::KeyReleased && !paused) {
				// Listen for key pressed again
				acceptInput = true;
				// hide the axe
				spriteAxe.setPosition(2000, spriteAxe.getPosition().y);
			}
		}
		if (Keyboard::isKeyPressed(Keyboard::Escape)) {
			window.close();
		}
		if (Keyboard::isKeyPressed(Keyboard::Enter)) {
			paused = false;
			// Reset the time and score;
			score = 0;
			timeRemaining = 6;
			// Make all the branches disappear -
			// starting in the second position
			for (int i = 1; i < NUM_BRANCHES; i++) {
				branchPositions[i] = side::NONE;
			}
			// Make sure the gravestone is hidden
			spriteRIP.setPosition(675, 2000);
			// Move the player into position
			spritePlayer.setPosition(580, 720);
			acceptInput = true;
		}
		if (acceptInput) {
			// First handle pressing the right cursor key
			if (Keyboard::isKeyPressed(Keyboard::Right)) {
				// Make sure the player is on the right side
				playerSide = side::RIGHT;
				score++;
				// Add to the amount of time remaining
				timeRemaining += (2.0 / score) + 0.15f ;
				spriteAxe.setPosition(AXE_POSITION_RIGHT, spriteAxe.getPosition().y);
				spritePlayer.setPosition(1200, 720);
				// update the branches
				updateBranches(score);

				// Set the log flying to the left
				spriteLog.setPosition(810, 720);
				logSpeedX = -5000;
				logActive = true;
				acceptInput = false;
				// Play a chop sound 
				chop.play();
			}
			// Handle the left cursor key
			if (Keyboard::isKeyPressed(Keyboard::Left)) {
				playerSide = side::LEFT;
				score++;
				// add to the amount of time remaining
				timeRemaining += (2.0 / score) + 0.15f;
				spriteAxe.setPosition(AXE_POSITION_LEFT, spriteAxe.getPosition().y);
				spritePlayer.setPosition(580, 720);
				// update the branches
				updateBranches(score);
				// set the log flying
				spriteLog.setPosition(810, 720);
				logSpeedX = 5000;
				logActive = true;
				acceptInput = false;
				chop.play();
			}
		}
		/*
		**********************************
		Update the scene
		**********************************
		*/
		if (!paused) {
			// Measure time
			Time dt = clock.restart();
			//Subtract from the amount of time remaining
			timeRemaining -= dt.asSeconds();
			// size up the time bar
			timeBar.setSize(Vector2f(timeBarWidthPerSecond * timeRemaining, timeBarHeight));
			if (timeRemaining <= 0.0f) {
				// Pause the game
				paused = true;
				// Change the message shown to the player
				messageText.setString("Out of time!!");
				// Reposition the text based on its new size
				FloatRect textRect = messageText.getLocalBounds();
				messageText.setOrigin(
					textRect.left + textRect.width / 2.0f,
					textRect.top + textRect.height / 2.0f
				);
				messageText.setPosition(1920 / 2.0f, 1080 / 2.0f);
				outOftime.play();
			}
			// Setup the bee
			if (!beeActive) {
				// How fast is the bee
				srand((int)time(0));
				beeSpeed = (rand() % 200) + 200;
				// How high is the bee
				srand((int)time(0) * 10);
				float height = (rand() % 500) + 500;
				spriteBee.setPosition(2000, height);
				beeActive = true;
			}
			else {
				// Move the bee
				spriteBee.setPosition(
					spriteBee.getPosition().x - (beeSpeed * dt.asSeconds()),
					spriteBee.getPosition().y
				);
				// Has the bee reached the left-hand edge of the screen?
				if (spriteBee.getPosition().x < -100) {
					beeActive = false;
				}
			}
			// Manage the clouds

			// refactor to use loops and arrays instead of manually handling each cloud object
			for (int i = 0; i < 3; i++) {
				int k = i + 1;
				if (!cloudActive[i]) {
					// How fast is the cloud
					srand((int)time(0) * (k * 10));
					cloudSpeed[i] = (rand() % 200);
					// How high is the cloud
					srand((int)time(0) * (k * 10));
					float height;
					if (i == 0) {
						height = (rand() % (k * 150));
					}
					else {
						height = (rand() % (k * 150) - 150);
					}
					Clouds[i].setPosition(-200, height);
					cloudActive[i] = true;
				}
				else {
					Clouds[i].setPosition(
						Clouds[i].getPosition().x + (cloudSpeed[i] * dt.asSeconds()),
						Clouds[i].getPosition().y
					);
					// Has the cloud reached the right hand edge of the screen?
					if (Clouds[i].getPosition().x > 1920) {
						//  Set it up ready to be a whole new cloud next frame
						cloudActive[i] = false;
					}
				}
			}

			// Calculate FPS
			// Add the length of time this frame has taken
			oneSecond += dt.asSeconds();
			fps++;
			// If oneSecond has passed, reset the oneSecond timer and update the FPS + score
			if (oneSecond >= 1.0f) {
				// Update the score text
				std::stringstream ss;
				ss << "Score = " << score;
				scoreText.setString(ss.str());

				std::stringstream  sf;
				// Update the FPS Counter
				sf << "FPS: " << fps;
				fpsText.setString(sf.str());

				// -1.0f to leave any remainder over one second for the next update
				oneSecond -= 1.0f;
				fps = 0;
			}


			// Update the branch sprites
			for (int i = 0; i < NUM_BRANCHES; i++) {
				float height = i * 150;
				if (branchPositions[i] == side::LEFT) {
					// Move the sprite to the left side
					branches[i].setPosition(610, height);
					// Flip the sprite the other way around
					branches[i].setRotation(180);
				}
				else if (branchPositions[i] == side::RIGHT) {
					// Move the sprite to the right side
					branches[i].setPosition(1330, height);
					// Set the sprite rotation to normal
					branches[i].setRotation(0);
				}
				else {
					// Hide the branch
					branches[i].setPosition(3000, height);
				}
			}
			// Handle a flying log
			if (logActive) {
				spriteLog.setPosition(
					spriteLog.getPosition().x + (logSpeedX * dt.asSeconds()),
					spriteLog.getPosition().y + (logSpeedY * dt.asSeconds())
				);
				// Has the log reached the right hande edge?
				if (spriteLog.getPosition().x < -100 || spriteLog.getPosition().x > 2000) {
					// Set it up ready to be a whole new log next frame
					logActive = false;
					spriteLog.setPosition(810, 720);
				}
			}

			// has the player been squished by a branch?
			if (branchPositions[5] == playerSide) {
				//death
				paused = true;
				acceptInput = false;
				// Draw the gamestone
				spriteRIP.setPosition(525, 760);
				// hide the player
				spritePlayer.setPosition(2000, 660);
				// Change the text of the message
				messageText.setString("You died!");
				// center it on screen
				FloatRect textRect = messageText.getLocalBounds();
				messageText.setOrigin(
					textRect.left + textRect.width / 2.0f,
					textRect.top + textRect.height / 2.0f
				);
				death.play();
				fps = 0;

			}
		} // end if(!paused)

		/*
		**********************************
		Draw the scene
		**********************************
		*/
		// Clear everything from the last frame
		window.clear();
		// Draw our game scene here
		window.draw(spriteBackground);
		// Draw the clouds
		// refactor to use loops and arrays instead of manually handling each object
		for (int i = 0; i < sizeof(Clouds) / sizeof(Clouds[0]); i++) {
			window.draw(Clouds[i]);
		}
		// Draw the branches
		for (int i = 0; i < NUM_BRANCHES; i++) {
			window.draw(branches[i]);
		}
		// Draw the tree
		window.draw(spriteTree);
		// Draw the player
		window.draw(spritePlayer);
		// Draw the axe
		window.draw(spriteAxe);
		// Draw the flying log
		window.draw(spriteLog);
		// Draw the gravestone
		window.draw(spriteRIP);
		// Draw the insect
		window.draw(spriteBee);
		// Draw the score
		window.draw(scoreBackground);
		window.draw(scoreText);
		if (paused) {
			window.draw(messageText);
		}
		// Draw the timebar
		window.draw(timeBar);
		//Draw the FPS
		window.draw(fpsBackground);
		window.draw(fpsText);
		// Show everything we just drew
		window.display();
	}

	return 0;

}

// Function definition
void updateBranches(int seed) {
	// Move all the branches down one place
	for (int j = NUM_BRANCHES - 1; j > 0; j--) {
		branchPositions[j] = branchPositions[j - 1];
	}
	// Spawn a new branch at position 0
	// LEFT RIGHT OR NONE
	srand((int)time(0) + seed);
	int r = (rand() % 3);
	switch (r) {
	case 0:
		branchPositions[0] = side::LEFT;
		break;
	case 1:
		branchPositions[0] = side::RIGHT;
		break;
	default:
		branchPositions[0] = side::NONE;
		break;
	}
}