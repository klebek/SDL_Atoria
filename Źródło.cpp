#include <SDL.h>
#include <SDL_image.h>
#include <stdio.h>
#include <string>
#include <chrono>
#include <thread>
#include <vector>
#include <algorithm>
#include <random>
#include <map>

#include <functional>

const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

bool got = false;
int gold = 0;

using namespace std;
using namespace std::chrono;
duration<double> dt(0.015); // w sekundach
time_point<high_resolution_clock, duration<double> >prevTime = high_resolution_clock::now();


const Uint8 *state = SDL_GetKeyboardState(NULL);

class LTexture
{
public:
	LTexture();

	~LTexture();

	bool loadFromFile(std::string path);

#ifdef _SDL_TTF_H
	bool loadFromRenderedText(std::string textureText, SDL_Color textColor);
#endif

	void free();

	void setColor(Uint8 red, Uint8 green, Uint8 blue);

	void setBlendMode(SDL_BlendMode blending);

	void setAlpha(Uint8 alpha);

	void render(int x, int y, SDL_Rect* clip = NULL, double angle = 0.0, SDL_Point* center = NULL, SDL_RendererFlip flip = SDL_FLIP_NONE);

	int getWidth();
	int getHeight();

private:
	SDL_Texture* mTexture;

	int mWidth;
	int mHeight;
};

class LTimer
{
public:
	LTimer();

	void start();
	void stop();
	void pause();
	void unpause();

	Uint32 getTicks();

	bool isStarted();
	bool isPaused();

private:
	Uint32 mStartTicks;

	Uint32 mPausedTicks;

	bool mPaused;
	bool mStarted;
};

class Dwarf
{
public:
	static const int Dwarf_WIDTH = 30;
	static const int Dwarf_HEIGHT = 30;

	int Dwarf_VEL = 1;
	int mPosX, mPosY;
	int acTime;
	int ac;

	SDL_Rect mCollider;

	Dwarf();

	void handleEvent(SDL_Event& e);

	void move(SDL_Rect& wall1, SDL_Rect& wall2, SDL_Rect& wall3, SDL_Rect& wall4, SDL_Rect& wall5, SDL_Rect& wall6, SDL_Rect& gold1, SDL_Rect& gold2, SDL_Rect& gold3, SDL_Rect& gate1, SDL_Rect& gate2, SDL_Rect& gate3, SDL_Rect& key);

	void render();

private:

	int mVelX, mVelY;

};

class None1
{
public:
	static const int Key_WIDTH = 31;
	static const int Key_HEIGHT = 31;

	None1();
	void render();

private:
	int mPosX, mPosY;
};

class None2
{
public:
	static const int Key_WIDTH = 31;
	static const int Key_HEIGHT = 31;

	None2();
	void render();

private:
	int mPosX, mPosY;
};

class Key
{
public:
	static const int Key_WIDTH = 20;
	static const int Key_HEIGHT = 20;

	Key();
	void render();

private:
	int mPosX, mPosY;
	SDL_Rect mCollider;
};

class Gold1
{
public:
	static const int Gold1_WIDTH = 30;
	static const int Gold1_HEIGHT = 30;

	Gold1();
	void render();

private:
	int mPosX, mPosY;
	SDL_Rect mCollider;
};

class Gold2
{
public:
	static const int Gold2_WIDTH = 30;
	static const int Gold2_HEIGHT = 30;

	Gold2();
	void render();

private:
	int mPosX, mPosY;
	SDL_Rect mCollider;
};

class Gold3
{
public:
	static const int Gold3_WIDTH = 30;
	static const int Gold3_HEIGHT = 30;

	Gold3();
	void render();

private:
	int mPosX, mPosY;
	SDL_Rect mCollider;
};

class Diamond
{
public:
	static const int Diamond_WIDTH = 25;
	static const int Diamond_HEIGHT = 25;

	Diamond();
	void render();

private:
	int mPosX, mPosY;
};

class Gate1
{
public:
	static const int Gate1_WIDTH = 115;
	static const int Gate1_HEIGHT = 20;

	Gate1();
	void render();

private:
	int mPosX, mPosY;
	SDL_Rect mCollider;
};

class Gate2
{
public:
	static const int Gate2_WIDTH = 115;
	static const int Gate2_HEIGHT = 20;

	Gate2();
	void render();

private:
	int mPosX, mPosY;
	SDL_Rect mCollider;
};

class Gate3
{
public:
	static const int Gate3_WIDTH = 135;
	static const int Gate3_HEIGHT = 20;

	Gate3();
	void render();

private:
	int mPosX, mPosY;
	SDL_Rect mCollider;
};

bool init();

bool loadMedia();

void close();

bool checkCollision(SDL_Rect a, SDL_Rect b);

SDL_Window* gWindow = NULL;

SDL_Renderer* gRenderer = NULL;

LTexture gDwarfTexture;

LTexture gGold1Texture;

LTexture gGold2Texture;

LTexture gGold3Texture;

LTexture gGate1Texture;

LTexture gGate2Texture;

LTexture gGate3Texture;

LTexture gKeyTexture;

LTexture gBgTexture;

LTexture gBgLostTexture;

LTexture gNone1Texture;

LTexture gDiamondTexture;

LTexture gNone2Texture;

LTexture::LTexture()
{
	mTexture = NULL;
	mWidth = 0;
	mHeight = 0;
}

LTexture::~LTexture()
{
	free();
}

bool LTexture::loadFromFile(std::string path)
{
	free();

	SDL_Texture* newTexture = NULL;

	SDL_Surface* loadedSurface = IMG_Load(path.c_str());
	if (loadedSurface == NULL)
	{
		printf("Unable to load image %s! SDL_image Error: %s\n", path.c_str(), IMG_GetError());
	}
	else
	{
		SDL_SetColorKey(loadedSurface, SDL_TRUE, SDL_MapRGB(loadedSurface->format, 0, 0xFF, 0xFF));

		newTexture = SDL_CreateTextureFromSurface(gRenderer, loadedSurface);
		if (newTexture == NULL)
		{
			printf("Unable to create texture from %s! SDL Error: %s\n", path.c_str(), SDL_GetError());
		}
		else
		{
			mWidth = loadedSurface->w;
			mHeight = loadedSurface->h;
		}

		SDL_FreeSurface(loadedSurface);
	}

	mTexture = newTexture;
	return mTexture != NULL;
}

#ifdef _SDL_TTF_H
bool LTexture::loadFromRenderedText(std::string textureText, SDL_Color textColor)
{
	free();

	SDL_Surface* textSurface = TTF_RenderText_Solid(gFont, textureText.c_str(), textColor);
	if (textSurface != NULL)
	{
		mTexture = SDL_CreateTextureFromSurface(gRenderer, textSurface);
		if (mTexture == NULL)
		{
			printf("Unable to create texture from rendered text! SDL Error: %s\n", SDL_GetError());
		}
		else
		{
			mWidth = textSurface->w;
			mHeight = textSurface->h;
		}

		SDL_FreeSurface(textSurface);
	}
	else
	{
		printf("Unable to render text surface! SDL_ttf Error: %s\n", TTF_GetError());
	}


	return mTexture != NULL;
}
#endif

void LTexture::free()
{
	if (mTexture != NULL)
	{
		SDL_DestroyTexture(mTexture);
		mTexture = NULL;
		mWidth = 0;
		mHeight = 0;
	}
}

void LTexture::setColor(Uint8 red, Uint8 green, Uint8 blue)
{
	SDL_SetTextureColorMod(mTexture, red, green, blue);
}

void LTexture::setBlendMode(SDL_BlendMode blending)
{
	SDL_SetTextureBlendMode(mTexture, blending);
}

void LTexture::setAlpha(Uint8 alpha)
{
	SDL_SetTextureAlphaMod(mTexture, alpha);
}

void LTexture::render(int x, int y, SDL_Rect* clip, double angle, SDL_Point* center, SDL_RendererFlip flip)
{
	SDL_Rect renderQuad = { x, y, mWidth, mHeight };

	if (clip != NULL)
	{
		renderQuad.w = clip->w;
		renderQuad.h = clip->h;
	}

	SDL_RenderCopyEx(gRenderer, mTexture, clip, &renderQuad, angle, center, flip);
}

int LTexture::getWidth()
{
	return mWidth;
}

int LTexture::getHeight()
{
	return mHeight;
}

Key::Key()
{
	mPosX = 50;
	mPosY = 120;

	mCollider.w = Key_WIDTH;
	mCollider.h = Key_HEIGHT;
}

None1::None1()
{
	mPosX = 165;
	mPosY = 210;

}

Diamond::Diamond()
{
	mPosX = 590;
	mPosY = 440;

}

None2::None2()
{
	mPosX = 605;
	mPosY = 10;

}

Gold1::Gold1()
{
	mPosX = 150;
	mPosY = 210;

	mCollider.w = Gold1_WIDTH;
	mCollider.h = Gold1_HEIGHT;
}

Gold2::Gold2()
{
	mPosX = 590;
	mPosY = 10;

	mCollider.w = Gold2_WIDTH;
	mCollider.h = Gold2_HEIGHT;
}

Gold3::Gold3()
{
	mPosX = 590;
	mPosY = 440;

	mCollider.w = Gold3_WIDTH;
	mCollider.h = Gold3_HEIGHT;
}

Gate1::Gate1()
{
	mPosX = 525;
	mPosY = 60;

	mCollider.w = Gate1_WIDTH;
	mCollider.h = Gate1_HEIGHT;
}

Gate2::Gate2()
{
	mPosX = 525;
	mPosY = 400;

	mCollider.w = Gate2_WIDTH;
	mCollider.h = Gate2_HEIGHT;
}

Gate3::Gate3()
{
	mPosX = 145;
	mPosY = 180;

	mCollider.w = Gate3_WIDTH;
	mCollider.h = Gate3_HEIGHT;
}


Dwarf::Dwarf()
{
	mPosX = 280;
	mPosY = 380;

	mCollider.w = Dwarf_WIDTH;
	mCollider.h = Dwarf_HEIGHT;

	mVelX = 0;
	mVelY = 0;

	acTime = 0;
	ac = 0;
}

void Dwarf::handleEvent(SDL_Event& e)
{
	int a;
	if (e.type == SDL_KEYDOWN && e.key.repeat == 0)
	{
		switch (e.key.keysym.sym)
		{
		case SDLK_UP: mVelY -= Dwarf_VEL; break;
		case SDLK_DOWN: mVelY += Dwarf_VEL; break;
		case SDLK_LEFT: mVelX -= Dwarf_VEL; break;
		case SDLK_RIGHT: mVelX += Dwarf_VEL; break;
		}
	}
	else if (e.type == SDL_KEYUP && e.key.repeat == 0)
	{
		switch (e.key.keysym.sym)
		{
		case SDLK_UP: mVelY += Dwarf_VEL; break;
		case SDLK_DOWN: mVelY -= Dwarf_VEL; break;
		case SDLK_LEFT: mVelX += Dwarf_VEL; break;
		case SDLK_RIGHT: mVelX -= Dwarf_VEL; break;
		}
	}
}

void Dwarf::move(SDL_Rect& wall1, SDL_Rect& wall2, SDL_Rect& wall3, SDL_Rect& wall4, SDL_Rect& wall5, SDL_Rect& wall6, SDL_Rect& gold1, SDL_Rect& gold2, SDL_Rect& gold3, SDL_Rect& gate1, SDL_Rect& gate2, SDL_Rect& gate3, SDL_Rect& key)
{
	acTime += 1;
	mPosX += mVelX;
	mPosY += mVelY;
	mCollider.x = mPosX;
	mCollider.y = mPosY;

	if (mVelX != 0 || mVelY != 0)
	{
		switch (acTime)
		{
		case 10:
			ac = 3;
			break;
		case 20:
			ac = 6;
			break;
		case 30:
			ac = 15;
			break;
		case 40:
			ac = 20;
			break;
		}
	}
	else
	{
		acTime = 0;
		ac = 0;
	}

	if (mVelX > 0)
	{
		mPosX += (mVelX) + ac;
	}
	else if (mVelX < 0)
	{
		mPosX += (mVelX) - ac;
	}
	else if (mVelY < 0)
	{
		mPosY += (mVelY)-ac;
	}
	else if (mVelY > 0)
	{
		mPosY += (mVelY)+ac;
	}
	else
	{
		mPosX += mVelX;
		mPosY += mVelY;
	}

	if (
		(mPosX < 0) ||
		(mPosX + Dwarf_WIDTH > SCREEN_WIDTH) ||
		checkCollision(mCollider, wall1) ||
		checkCollision(mCollider, wall2) ||
		checkCollision(mCollider, wall3) ||
		checkCollision(mCollider, wall4) ||
		checkCollision(mCollider, wall5) ||
		checkCollision(mCollider, wall6)
		)
	{
		mPosX = 280;
		mPosY = 380;
		mCollider.x = mPosX;
	}

	if (checkCollision(mCollider, key))
	{
		//mPosX = 100;
		//mPosY = 120;
		gKeyTexture.free();
		gGate1Texture.free();
		gGate2Texture.free();
		gGate3Texture.free();
		got = true;
	}

	if (
		(mPosY < 0) ||
		(mPosY + Dwarf_HEIGHT > SCREEN_HEIGHT) ||
		checkCollision(mCollider, wall1) ||
		checkCollision(mCollider, wall2) ||
		checkCollision(mCollider, wall3) ||
		checkCollision(mCollider, wall4) ||
		checkCollision(mCollider, wall5) ||
		checkCollision(mCollider, wall6)
		)
	{
		mPosX = 330;
		mPosY = 380;
		mCollider.y = mPosY;
	}

	mCollider.x = mPosX;
	mCollider.y = mPosY;

	if (checkCollision(mCollider, gate1))
	{
		if (got == false)
		{
			mPosX = 560;
			mPosY = 90;
		}
	}

	if (checkCollision(mCollider, gate2))
	{
		if (got == false)
		{
			mPosX = 560;
			mPosY = 360;
		}
	}

	if (checkCollision(mCollider, gate3))
	{
		if (got == false)
		{
			mPosX = 190;
			mPosY = 140;
		}
	}

	if(checkCollision(mCollider, gold1))
	{
		//mPosX = 570;
		//mPosY = 40;
		gGold1Texture.free();

		gold = 1;
	}

	if (checkCollision(mCollider, gold2))
	{
		//mPosX = 570;
		//mPosY = 40;
		gGold2Texture.free();
		gold = 2;
	}

	if (checkCollision(mCollider, gold3))
	{
		//mPosX = 570;
		//mPosY = 40;
		gGold3Texture.free();
		gold = 3;
	}

	if(gold == 3)
	{
		gBgTexture.free();
		gDwarfTexture.free();
		gGold1Texture.free();
		gGold2Texture.free();
		gNone1Texture.free();
		gNone2Texture.free();
	}
}

void Dwarf::render()
{
	gDwarfTexture.render(mPosX, mPosY);
}

void None1::render()
{
	gNone1Texture.render(mPosX, mPosY);
}

void Diamond::render()
{
	gDiamondTexture.render(mPosX, mPosY);
}

void None2::render()
{
	gNone2Texture.render(mPosX, mPosY);
}

void Key::render()
{
	gKeyTexture.render(mPosX, mPosY);
}

void Gold1::render()
{
	gGold1Texture.render(mPosX, mPosY);
}

void Gold2::render()
{
	gGold2Texture.render(mPosX, mPosY);
}

void Gold3::render()
{
	gGold3Texture.render(mPosX, mPosY);
}

void Gate1::render()
{
	gGate1Texture.render(mPosX, mPosY);
}

void Gate2::render()
{
	gGate2Texture.render(mPosX, mPosY);
}

void Gate3::render()
{
	gGate3Texture.render(mPosX, mPosY);
}

bool init()
{
	bool success = true;

	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		printf("SDL could not initialize! SDL Error: %s\n", SDL_GetError());
		success = false;
	}
	else
	{
		if (!SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1"))
		{
			printf("Warning: Linear texture filtering not enabled!");
		}

		gWindow = SDL_CreateWindow("SDL game for SGD lecture.", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
		if (gWindow == NULL)
		{
			printf("Window could not be created! SDL Error: %s\n", SDL_GetError());
			success = false;
		}
		else
		{
			gRenderer = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_ACCELERATED);
			if (gRenderer == NULL)
			{
				printf("Renderer could not be created! SDL Error: %s\n", SDL_GetError());
				success = false;
			}
			else
			{
				SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);

				int imgFlags = IMG_INIT_PNG;
				if (!(IMG_Init(imgFlags) & imgFlags))
				{
					printf("SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError());
					success = false;
				}
			}
		}
	}

	return success;
}

bool loadMedia()
{
	bool success = true;

	if (!gDwarfTexture.loadFromFile("textures/dwarf.png"))
	{
		printf("Failed to load Dwarf texture!\n");
		success = false;
	}

	if (!gKeyTexture.loadFromFile("textures/key.png"))
	{
		printf("Failed to load Key texture!\n");
		success = false;
	}

	if (!gBgTexture.loadFromFile("textures/bg.bmp"))
	{
		printf("Failed to load Bg texture!\n");
		success = false;
	}

	if (!gBgLostTexture.loadFromFile("textures/bg_lost.png"))
	{
		printf("Failed to load BgLost texture!\n");
		success = false;
	}

	if (!gGold1Texture.loadFromFile("textures/box.png"))
	{
		printf("Failed to load Gold1 texture!\n");
		success = false;
	}

	if (!gGold2Texture.loadFromFile("textures/box.png"))
	{
		printf("Failed to load Gold2 texture!\n");
		success = false;
	}

	if (!gGold3Texture.loadFromFile("textures/box.png"))
	{
		printf("Failed to load Gold3 texture!\n");
		success = false;
	}

	if (!gGate1Texture.loadFromFile("textures/gate1.png"))
	{
		printf("Failed to load Gate1 texture!\n");
		success = false;
	}

	if (!gGate2Texture.loadFromFile("textures/gate1.png"))
	{
		printf("Failed to load Gate2 texture!\n");
		success = false;
	}

	if (!gNone1Texture.loadFromFile("textures/none.png"))
	{
		printf("Failed to load None1 texture!\n");
		success = false;
	}

	if (!gDiamondTexture.loadFromFile("textures/diamond.png"))
	{
		printf("Failed to load Diamond texture!\n");
		success = false;
	}

	if (!gNone2Texture.loadFromFile("textures/none.png"))
	{
		printf("Failed to load None2 texture!\n");
		success = false;
	}

	if (!gGate3Texture.loadFromFile("textures/gate3.png"))
	{
		printf("Failed to load Gate3 texture!\n");
		success = false;
	}

	return success;
}

void close()
{
	gDwarfTexture.free();
	gBgTexture.free();
	gBgLostTexture.free();
	gGold1Texture.free();
	gGold2Texture.free();
	gGold3Texture.free();
	gGate1Texture.free();
	gNone1Texture.free();
	gNone2Texture.free();
	gDiamondTexture.free();
	gGate2Texture.free();
	gGate3Texture.free();
	gKeyTexture.free();

	SDL_DestroyRenderer(gRenderer);
	SDL_DestroyWindow(gWindow);
	gWindow = NULL;
	gRenderer = NULL;

	IMG_Quit();
	SDL_Quit();
}

bool checkCollision(SDL_Rect a, SDL_Rect b)
{
	int leftA, leftB;
	int rightA, rightB;
	int topA, topB;
	int bottomA, bottomB;

	leftA = a.x;
	rightA = a.x + a.w;
	topA = a.y;
	bottomA = a.y + a.h;

	leftB = b.x;
	rightB = b.x + b.w;
	topB = b.y;
	bottomB = b.y + b.h;

	if (bottomA <= topB)
	{
		return false;
	}

	if (topA >= bottomB)
	{
		return false;
	}

	if (rightA <= leftB)
	{
		return false;
	}

	if (leftA >= rightB)
	{
		return false;
	}

	return true;
}

int main(int argc, char* args[])
{
	if (!init())
	{
		printf("Failed to initialize!\n");
	}
	else
	{
		duration<double> dt(0.01);
		this_thread::sleep_until(prevTime + dt);
		prevTime = prevTime + dt;
		if (!loadMedia())
		{
			printf("Failed to load media!\n");
		}
		else
		{
			bool quit = false;

			SDL_Event e;

			Dwarf Dwarf;
			Gold1 Gold1;
			Gold2 Gold2;
			Gold3 Gold3;
			Gate1 Gate1;
			Gate2 Gate2;
			Gate3 Gate3;
			None1 None1;
			None2 None2;
			Diamond Diamond;
			Key Key;

			SDL_Rect wall1;
			wall1.x = 0;
			wall1.y = 177;
			wall1.w = 145;
			wall1.h = 330;

			SDL_Rect wall2;
			wall2.x = 145;
			wall2.y = 250;
			wall2.w = 153;
			wall2.h = 240;

			SDL_Rect wall3;
			wall3.x = 417;
			wall3.y = 327;
			wall3.w = 106;
			wall3.h = 155;

			SDL_Rect wall4;
			wall4.x = 280;
			wall4.y = 177;
			wall4.w = 247;
			wall4.h = 80;

			SDL_Rect wall5;
			wall5.x = 280;
			wall5.y = 0;
			wall5.w = 247;
			wall5.h = 88;

			SDL_Rect wall6;
			wall6.x = 0;
			wall6.y = 0;
			wall6.w = 145;
			wall6.h = 88;

			SDL_Rect gate1;
			gate1.x = 525;
			gate1.y = 60;
			gate1.w = 115;
			gate1.h = 20;

			SDL_Rect gate2;
			gate2.x = 525;
			gate2.y = 400;
			gate2.w = 115;
			gate2.h = 20;

			SDL_Rect gate3;
			gate3.x = 145;
			gate3.y = 180;
			gate3.w = 135;
			gate3.h = 20;

			SDL_Rect gold1;
			gold1.x = 150;
			gold1.y = 210;
			gold1.w = 42;
			gold1.h = 30;

			SDL_Rect gold2;
			gold2.x = 590;
			gold2.y = 10;
			gold2.w = 42;
			gold2.h = 30;

			SDL_Rect gold3;
			gold3.x = 590;
			gold3.y = 440;
			gold3.w = 42;
			gold3.h = 30;

			SDL_Rect key;
			key.x = 50;
			key.y = 120;
			key.w = 20;
			key.h = 20;

			while (!quit)
			{
				while (SDL_PollEvent(&e) != 0)
				{
					if (e.type == SDL_QUIT)
					{
						quit = true;
					}

					Dwarf.handleEvent(e);
				}

				Dwarf.move(wall1, wall2, wall3, wall4, wall5, wall6, gold1, gold2, gold3, gate1, gate2, gate3, key);

				SDL_SetRenderDrawColor(gRenderer, 0, 0, 0, 0);
				SDL_RenderClear(gRenderer);

				gBgLostTexture.render(0, 0);
				gBgTexture.render(0, 0);

				//KEY

				SDL_SetRenderDrawColor(gRenderer, 58, 30, 22, 0);
				SDL_RenderDrawRect(gRenderer, &key);

				// GATES

				SDL_SetRenderDrawColor(gRenderer, 58, 30, 22, 0);
				SDL_RenderDrawRect(gRenderer, &gate1);

				SDL_SetRenderDrawColor(gRenderer, 58, 30, 22, 0);
				SDL_RenderDrawRect(gRenderer, &gate2);

				SDL_SetRenderDrawColor(gRenderer, 58, 30, 22, 0);
				SDL_RenderDrawRect(gRenderer, &gate3);

				//GOLDS

				SDL_SetRenderDrawColor(gRenderer, 58, 30, 22, 0);
				SDL_RenderDrawRect(gRenderer, &gold1);

				SDL_SetRenderDrawColor(gRenderer, 58, 30, 22, 0);
				SDL_RenderDrawRect(gRenderer, &gold2);

				SDL_SetRenderDrawColor(gRenderer, 58, 30, 22, 0);
				SDL_RenderDrawRect(gRenderer, &gold3);

				//WALLS

				SDL_SetRenderDrawColor(gRenderer, 58, 30, 22, 0);
				SDL_RenderDrawRect(gRenderer, &wall1);

				SDL_SetRenderDrawColor(gRenderer, 58, 30, 22, 0);
				SDL_RenderDrawRect(gRenderer, &wall2);

				SDL_SetRenderDrawColor(gRenderer, 58, 30, 22, 0);
				SDL_RenderDrawRect(gRenderer, &wall3);

				SDL_SetRenderDrawColor(gRenderer, 58, 30, 22, 0);
				SDL_RenderDrawRect(gRenderer, &wall4);

				SDL_SetRenderDrawColor(gRenderer, 58, 30, 22, 0);
				SDL_RenderDrawRect(gRenderer, &wall5);

				SDL_SetRenderDrawColor(gRenderer, 58, 30, 22, 0);
				SDL_RenderDrawRect(gRenderer, &wall6);

				gBgLostTexture.render(0, 0);
				gBgTexture.render(0, 0);
				None1.render();
				Gold1.render();
				None2.render();
				Gold2.render();
				Diamond.render();
				Gold3.render();
				Key.render();
				Gate1.render();
				Gate2.render();
				Gate3.render();
				Dwarf.render();

				SDL_RenderPresent(gRenderer);
				SDL_Delay(8);

				auto currentTime = std::chrono::high_resolution_clock::now();
				//		std::this_thread::sleep_for( dt );
				//		std::this_thread::sleep_until( prevTime + dt );
				//		prevTime = prevTime + dt;// lub std::chrono::high_resolution_clock::now();
				dt = currentTime - prevTime;
				prevTime = currentTime;

			}
		}
	}

	close();

	return 0;
}