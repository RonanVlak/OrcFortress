#ifndef GAME_H
#define GAME_H

#include "DatabaseLoader.h"
#include "FSConverter.h"
#include "SpelFacade.h"
#include "Player.h"
#include "Direction.h"
#include <filesystem>
#include <iostream>
#include <memory>
#include <string>

class Game
{
public:
	Game();
	~Game() = default;
	void start();

private:
	enum class State
	{
		MainMenu,
		GameMenu,
		Gameplay,
		DeathMenu,
		Quit
	};

	void mainMenu();
	void gameMenu();
	void gameplay();
	void deathMenu();
	
	void setupNewRun();

	// Command handlers
	bool handleCommand(const std::string& cmd);
	void showHelp();
	void showPlayer();

	std::string mPlayerName;
    std::string mDbPath;   
	SpelFacade mFacade;
	Player mPlayer;
	State mCurrentState;
	int mRoomWidth;
	int mRoomHeight;
};

#endif // GAME_H