#include <stdio.h>
#include <nds.h>
#include <nf_lib.h>
#include <time.h>

#define click() (keysDown() & KEY_TOUCH)
#define release() (keysUp() & KEY_TOUCH)
#define hold() (keysHeld() & KEY_TOUCH)
#define lidClosed() (keysDown() & KEY_LID)

class NumberSlice
{
public:
	int number;
	float x;
	float y;
	int gfxId;
	int spriteId;
};

// Number slices
NumberSlice numberSlices[10];
int selectedNumberSlice = -1;

// Current level details
int level = 0;
int currentTime = 0;
int currentQuota = 0;

int getQuota(int level)
{
	return 4 + ((level)*2);
}

int getTimeLimit(int level)
{
	return ((30 - (level * 1.5)) + (getQuota(level) * 4));
}

void updateText()
{
	char statusFormat[] = "QUOTA: %i/%i TIME: %i LEVEL: %i";
	char statusText[1000];
	sprintf(statusText, statusFormat, currentQuota, getQuota(level), currentTime, (level + 1));

	NF_ClearTextLayer(0, 0);
	NF_WriteText(0, 0, 1, 22, statusText);
}

void generateNumbers()
{
	// Generate random numbers
	int numbers[10];
	numbers[0] = 1;
	numbers[1] = 2;
	numbers[2] = 1;
	numbers[3] = 6;

	// Fill the rest of the array with random numbers
	for (int i = 4; i < 10; i++)
	{
		numbers[i] = rand() % 9 + 1;
	}

	// Shuffle the numbers
	for (int i = 0; i < 10; i++)
	{
		int j = rand() % 10;
		int temp = numbers[i];
		numbers[i] = numbers[j];
		numbers[j] = temp;
	}

	for (int i = 0; i < 10; i++)
	{
		numberSlices[i].number = numbers[i];
		numberSlices[i].x = (i * 25) + 16;
		numberSlices[i].y = 174 - (rand() % 10 - 5);
		numberSlices[i].spriteId = 10 + i;
		numberSlices[i].gfxId = 10 + numbers[i];
		NF_CreateSprite(1, numberSlices[i].spriteId, numberSlices[i].gfxId, 10, numberSlices[i].x - 16, numberSlices[i].y - 32);
	}
}

void setTitle(float x, float y)
{
	// Create logo sprite
	NF_CreateSprite(0, 1, 1, 1, x - 32, y);
	NF_CreateSprite(0, 2, 2, 2, x + 32, y);

	// Create saul sprite
	NF_CreateSprite(0, 0, 0, 0, 256 / 2 - 64, 192 / 2 - 32);
	NF_EnableSpriteRotScale(0, 0, 0, true);

	// Write text
	NF_CreateTextLayer(1, 1, 0, "font");
	NF_DefineTextColor(1, 1, 1, 31, 31, 0);
	NF_SetTextColor(1, 1, 1); // Set text to yellow
	NF_WriteText(1, 1, 11.5, 10, "KID NAMED");
	NF_WriteText(1, 1, 7.5, 12, "* TOUCH TO START *");

	// Play title theme
	soundKill(0);
	NF_PlayRawSound(0, 64, 64, true, 1);
}

void setGameOver(const char text[])
{
	// Unload letters
	for (int i = 0; i < 10; i++)
	{
		NF_DeleteSprite(1, numberSlices[i].spriteId);
	}
	for (int i = 1; i < 10; i++)
	{
		NF_UnloadSpriteGfx(10 + i);
		NF_FreeSpriteGfx(1, 10 + i);
	}

	// Update backgrounds
	NF_DeleteTiledBg(1, 1);
	NF_CreateTiledBg(1, 1, "rules_bg");

	// Delete text layer, create new text layer
	NF_CreateTextLayer(1, 1, 0, "font");
	NF_DefineTextColor(1, 1, 1, 31, 31, 0);
	NF_SetTextColor(1, 1, 1); // Set text to yellow

	// Write text
	NF_ClearTextLayer(0, 0);
	NF_WriteText(1, 1, 11, 8, text);
	NF_WriteText(1, 1, 5, 9.5, "* TOUCH TO CONTINUE *");
	NF_WriteText(0, 0, 1, 1, "");
}

int main()
{
	// Seed randomizer
	srand(time(NULL));

	// Ready the graphics system
	consoleDemoInit();
	consoleClear();

	// Set both screens to graphics mode 0
	NF_Set2D(0, 0);
	NF_Set2D(1, 0);

	// Set the root assets folder to /nitrofiles
	NF_SetRootFolder("NITROFS");

	// Prepare sound engine
	soundEnable();
	NF_InitRawSoundBuffers();

	// Prepare top and bottom screen sprite/bg/text systems
	NF_InitSpriteBuffers();
	NF_InitTiledBgBuffers();
	NF_InitSpriteSys(0);
	NF_InitSpriteSys(1);
	NF_InitTiledBgSys(0);
	NF_InitTiledBgSys(1);
	NF_InitTextSys(0);
	NF_InitTextSys(1);

	// Load the saul sprite (id: 0, ram/vram pal/gfx slot: 0)
	NF_LoadSpriteGfx("sprites/saul", 0, 64, 64); // Tempy!
	NF_LoadSpritePal("sprites/saul", 0);
	NF_VramSpriteGfx(0, 0, 0, false);
	NF_VramSpritePal(0, 0, 0);

	// Load the top screen background
	NF_LoadTiledBg("backgrounds/titlebg", "title_bg", 256, 256);
	NF_CreateTiledBg(0, 1, "title_bg");

	// Load the font
	NF_LoadTextFont("font/font", "font", 256, 256, 0);
	NF_CreateTextLayer(0, 0, 0, "font");

	// Load sounds
	NF_LoadRawSound("sounds/title", 0, 22050, 0);
	NF_LoadRawSound("sounds/byewaltuh", 1, 22050, 0);
	NF_LoadRawSound("sounds/game", 2, 22050, 0);
	NF_LoadRawSound("sounds/bettercallsaul", 3, 22050, 0);
	NF_LoadRawSound("sounds/notourjimmy", 4, 22050, 0);
	NF_LoadRawSound("sounds/preciousjimmy", 5, 22050, 0);

	// Load the logo sprites
	NF_LoadSpriteGfx("sprites/logo_l", 1, 64, 64);
	NF_LoadSpritePal("sprites/logo_l", 1);
	NF_LoadSpriteGfx("sprites/logo_r", 2, 64, 64);
	NF_LoadSpritePal("sprites/logo_r", 2);
	NF_VramSpriteGfx(0, 1, 1, false);
	NF_VramSpritePal(0, 1, 1);
	NF_VramSpriteGfx(0, 2, 2, false);
	NF_VramSpritePal(0, 2, 2);

	// Load letter sprite palette
	NF_LoadSpritePal("sprites/letters/letters", 10);
	NF_VramSpritePal(1, 10, 10);

	// Variables for saul scalage
	int logoX = 256 / 2 - 32;
	int logoY = 4;

	int saulScale = 256;
	int saulScaleFactor = 1.2;

	// Frame counter
	long frame = 0;
	bool showLogo = false;

	// Touch screen position control
	touchPosition touch;

	// Game mode control
	int mode = 0;

	// Set the title screen
	setTitle(logoX, logoY);

	// Game loop
	while (1)
	{
		scanKeys();
		touchRead(&touch);

		// "Goodbye, Waltuh!" when you close the lid
		if (lidClosed())
		{
			NF_PlayRawSound(1, 64, 64, false, 0);
			break;
		}

		switch (mode)
		{
		// Title screen logic
		case 0:
		{
			// Kid named scale
			NF_SpriteRotScale(0, 0, 0, saulScale, saulScale);
			saulScale += saulScaleFactor;

			// Invert scale factor when limits reached
			if (saulScale >= 420 || saulScale <= 100)
			{
				saulScaleFactor *= -1;
			}

			if (frame % 60 == 0)
			{
				showLogo = !showLogo;
				NF_ShowSprite(0, 1, showLogo);
				NF_ShowSprite(0, 2, showLogo);
			}

			if (click())
			{
				mode = 1;
				frame = 0;
				// Clear text layers, prep new one
				NF_ClearTextLayer(1, 1);
				NF_CreateTextLayer(1, 0, 0, "font");
				NF_DefineTextColor(1, 0, 1, 31, 31, 0);
				NF_SetTextColor(1, 0, 1); // Set text to yellow

				// Remove saul
				NF_DeleteSprite(0, 0);

				// Show the title
				NF_ShowSprite(0, 1, true);
				NF_ShowSprite(0, 2, true);

				// Update background
				NF_LoadTiledBg("backgrounds/rulesbg", "rules_bg", 256, 256);
				NF_CreateTiledBg(1, 1, "rules_bg");

				// Write text
				NF_WriteText(1, 0, 11.5, 4, "HOW TO PLAY");
				NF_WriteText(1, 0, 2.5, 6, "HELP SLIPPIN' JIMMY SPLICE");
				NF_WriteText(1, 0, 2.5, 7.5, "THE NUMBERS! DRAG THE CUT UP");
				NF_WriteText(1, 0, 2.5, 8.5, "NUMBERS TO WIN BACK M.VERDE!");
				NF_WriteText(1, 0, 2.5, 9.5, "HURRY! CHUCK IS ON THE MOVE!");
				NF_WriteText(1, 0, 2.5, 13.5, "1216 - ONE AFTER MAGNA CARTA!");
				NF_WriteText(1, 0, 7, 16.5, "= TOUCH TO START =");
			}
			break;
		}
		case 1:
		{
			// Transition / rules explanation screen logic

			if (click() && frame > 30)
			{
				mode = 2;
				NF_ClearTextLayer(1, 1);
				NF_DeleteTextLayer(1, 0);

				// Load game bg
				NF_DeleteTiledBg(1, 1);
				NF_LoadTiledBg("backgrounds/gamebg", "game_bg", 256, 256);
				NF_CreateTiledBg(1, 1, "game_bg");

				// Play game theme
				soundKill(0);
				NF_PlayRawSound(2, 64, 64, true, 0);

				// Load the letter sprites
				for (int i = 1; i <= 9; i++)
				{
					char path[] = "sprites/letters/%d";
					char formattedPath[1000];
					sprintf(formattedPath, path, i);
					NF_LoadSpriteGfx(formattedPath, (i + 10), 32, 64);
					NF_VramSpriteGfx(1, (i + 10), (i + 10), true);
				}

				generateNumbers();
				currentTime = getTimeLimit(level);
				level = 0;
				currentQuota = 0;
				selectedNumberSlice = -1;
			}
			break;
		}
		case 2:
		{
			// Game logic

			// Pausing
			if (keysDown() & KEY_START)
			{
				break;
			}

			// Update timer display
			if (frame % 60 == 0 && currentTime > 0)
			{
				currentTime--;
			}
			updateText();
			if (currentTime <= 0)
			{
				currentTime = 0;
				mode = 3;
				setGameOver("GAME OVER");
				frame = 0;
				NF_PlayRawSound(1, 127, 64, false, 0);
				break;
			}

			// Update current stylus or finger (HOLY SHIT) position
			int touchX = 0;
			int touchY = 0;
			if (hold())
			{
				touchX = touch.px;
				touchY = touch.py;
			}

			if (click())
			{
				for (int i = 0; i < 10; i++)
				{
					if (touchX > numberSlices[i].x - 16 && touchX < numberSlices[i].x + 16 && touchY > numberSlices[i].y - 32 && touchY < numberSlices[i].y + 32)
					{
						selectedNumberSlice = i;
						break;
					}
				}
			}

			if (hold() && selectedNumberSlice != -1)
			{
				numberSlices[selectedNumberSlice].x = touchX;
				numberSlices[selectedNumberSlice].y = touchY;
				NF_MoveSprite(1, numberSlices[selectedNumberSlice].spriteId, touchX - 16, touchY - 32);
			}

			if (release() && selectedNumberSlice != -1)
			{
				selectedNumberSlice = -1;

				// Target coords (letters correspond to 1 2 1 6)
				int targetY = 40;
				int targetX[] = {40, 88, 136, 184};
				int targetNums[] = {1, 2, 1, 6};
				bool satisfied[] = {false, false, false, false};
				bool slotsOccupied[] = {false, false, false, false};

				// Iterate through each targetX
				for (int i = 0; i < 4; i++)
				{
					// Iterate through each number slice
					for (int j = 0; j < 10; j++)
					{
						// If the number slice is in the target x coord
						slotsOccupied[i] = false;
						if (numberSlices[j].x >= targetX[i] && numberSlices[j].x <= targetX[i] + 32)
						{
							// If the number slice is in the target y coord
							if (numberSlices[j].y >= targetY && numberSlices[j].y <= targetY + 64)
							{
								if (!slotsOccupied[i])
								{
									numberSlices[j].x = targetX[i];
									numberSlices[j].y = targetY;
									NF_MoveSprite(1, numberSlices[j].spriteId, targetX[i], targetY);
									slotsOccupied[i] = true;
								}

								// If the number slice is the correct number
								if (numberSlices[j].number == targetNums[i])
								{
									satisfied[i] = true;

									bool allSatisfied = true;
									// If all numbers are satisfied, end the game
									for (int k = 0; k < 4; k++)
									{
										if (!satisfied[k])
										{
											allSatisfied = false;
											break;
										}
									}

									// Detect if all numbers are satisfied and play sound test
									if (allSatisfied)
									{

										for (int i = 0; i < 10; i++)
										{
											NF_DeleteSprite(1, numberSlices[i].spriteId);
										}

										currentQuota++;
										generateNumbers();
										selectedNumberSlice = -1;

										if (currentQuota >= getQuota(level))
										{
											// Better call saul!
											NF_PlayRawSound(3, 94, 64, false, 0);

											level++;
											if (level > 9)
											{
												setGameOver("YOU WIN!");
												frame = 0;
												break;
											}
											currentQuota = 0;
											currentTime = getTimeLimit(level);
										}
										else
										{
											// Not our precious jimmy!
											if (frame % 2 == 0)
											{
												NF_PlayRawSound(4, 127, 127, false, 1);
											}
											else
											{
												NF_PlayRawSound(5, 127, 127, false, 1);
											}
										}
										break;
									}
								}
								else
								{
									satisfied[i] = false;
									break;
								}
							}
						}
					}
				}
			}

			break;
		}
		case 3:
		{
			if (click() && frame > 30)
			{
				frame = 0;
				mode = 0;
				NF_ClearTextLayer(0, 0);
				NF_ClearTextLayer(1, 1);
				NF_DeleteTiledBg(1, 1);
				setTitle(logoX, logoY);
			}
			break;
		}
		}

		// Update sprites, wait for next frame
		NF_SpriteOamSet(0);
		oamUpdate(&oamMain);
		NF_SpriteOamSet(1);
		oamUpdate(&oamSub);
		NF_UpdateTextLayers();
		swiWaitForVBlank();

		// Increment frame conter
		frame++;
		if (frame > 2147483646)
		{
			frame = 0;
		}
	}
	return 0;
}