#include "WindowManager/WindowManager.h"
#include "MyGame/MyGame.h"

#include "Utility/FrameCounter.h"

#include <iostream>

WindowManager mWindow;
MyGame* mGame = nullptr;

int main()
{
	FrameCounter frameCounter(1'000);
	float timePassed = 0;

	mWindow.create();

	mGame = new MyGame(mWindow);

	while (mWindow.isRunning() && !mWindow.isCloseRequested())
	{
		frameCounter.frame();
		const float delta = frameCounter.deltaTime();

		timePassed += delta;
		if (timePassed > 1.0f)
		{
			std::cout << frameCounter.fps() << "\n";
			timePassed = 0.0f;
		}

		if (mWindow.getInput().isKeyPressed(GLFW_KEY_ESCAPE))
		{
			break;
		}

		mGame->run(delta);
	}

	mGame->cleanup();
	mWindow.destroy();

	delete mGame;

	return 0;
}
