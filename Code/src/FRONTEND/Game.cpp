#include "FRONTEND/Game.h"
#include "BACKEND/DatabaseLoader.h"
#include "BACKEND/RuimteFactory.h"
#include "BACKEND/WapenObject.h"
#include "BACKEND/WapenRustingObject.h"
#include "Direction.h"
#include "FRONTEND/Logger.h"
#include "FRONTEND/RuimteRenderer.h"
#include "FRONTEND/SpelFacade.h"
#include "FSConverter.h"

#include <iostream>
#include <limits>
#include <map>
#include <string>

Game::Game()
	: mCurrentState(State::MainMenu), mFacade(std::make_unique<Spelwereld>()), mPlayer(), mRoomWidth(10),
	  mRoomHeight(10)
{
}

void Game::start()
{
	try
	{
		FSConverter fsConverter;
		std::string dbFileName = "OrcFortress.db";
		mDbPath = fsConverter.getResourcePath(dbFileName);

		mFacade.loadDatabase(mDbPath.c_str());

		while (mCurrentState != State::Quit)
		{

			if (mCurrentState == State::MainMenu)
			{
				// show main menu
				Logger::getInstance().logOutput("\n=== Hoofdmenu ===\n1. Nieuw spel\n2. Quit\n");

				int choice = 0;
				if (!(std::cin >> choice))
				{
					std::cin.clear();
					std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
					Logger::getInstance().logOutput("Ongeldige invoer.\n");
					continue;
				}
				std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
				Logger::getInstance().logInput(std::to_string(choice) + "\n");

				if (choice == 1)
				{
					setupNewRun(); // ask name + room size, reset world/player
					mCurrentState = State::Gameplay;
				}
				else if (choice == 2)
				{
					mCurrentState = State::Quit;
				}
				else
				{
					Logger::getInstance().logOutput("Ongeldige keuze.\n");
				}
			}
			else if (mCurrentState == State::Gameplay)
			{
				gameplay(); // this will change state to DeathMenu or Quit
			}
			else if (mCurrentState == State::DeathMenu)
			{
				deathMenu(); // sets Gameplay or Quit
			}
		}
	}
	catch (const std::exception& e)
	{
		Logger::getInstance().logError(std::string("Fatale fout in Game::start: ") + e.what());
	}
	catch (...)
	{
		Logger::getInstance().logError("Onbekende fatale fout in Game::start");
	}
}

void Game::setupNewRun()
{
	try
	{
		// ask name
		Logger::getInstance().logOutput("Voer je naam in: ");
		std::string naam;
		std::getline(std::cin, naam);
		Logger::getInstance().logInput(naam + "\n");

		if (naam.empty())
		{
			naam = "Orc"; // fallback if they just press enter
		}

		mPlayer.setNaam(naam);

		// ask room size
		int breed = 0;
		int hoog = 0;
		while (true)
		{
			Logger::getInstance().logOutput("Ruimte breedte (min 7): ");
			if (!(std::cin >> breed))
			{
				std::cin.clear();
				std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
				Logger::getInstance().logOutput("Ongeldige invoer.\n");
				continue;
			}
			Logger::getInstance().logInput(std::to_string(breed) + "\n");

			Logger::getInstance().logOutput("Ruimte hoogte (min 7): ");
			if (!(std::cin >> hoog))
			{
				std::cin.clear();
				std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
				Logger::getInstance().logOutput("Ongeldige invoer.\n");
				continue;
			}
			Logger::getInstance().logInput(std::to_string(hoog) + "\n");

			std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

			if (breed >= 7 && hoog >= 7)
				break;
			Logger::getInstance().logOutput("Beide waardes moeten minstens 7 zijn.\n");
		}

		// store chosen size
		mRoomWidth = breed;
		mRoomHeight = hoog;

		// reset backend world
		mFacade.resetWereld();

		mFacade.setDefaultRoomSize(mRoomWidth, mRoomHeight);

		// request new start ruimte with these dimensions so Spelwereld knows current room, coords, etc.
		(void)mFacade.getStartRuimte(mRoomWidth, mRoomHeight);

		// reset player stats to spec
		mPlayer.reset(naam, // set name
					  20,	// HP start
					  0		// goud start
		);

		// sync positions
		mFacade.syncPlayerFromWorld(mPlayer);
	}
	catch (const std::exception& e)
	{
		Logger::getInstance().logError(std::string("Fatale fout in Game::start: ") + e.what());
	}
	catch (...)
	{
		Logger::getInstance().logError("Onbekende fatale fout in Game::start");
	}
}

void Game::mainMenu()
{
	try
	{
		while (true)
		{
			Logger::getInstance().logOutput("\n=== Main Menu ===\n1. Start Game\n2. Quit\n");

			int choice = 0;
			if (!(std::cin >> choice))
			{
				std::cin.clear();
				std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
				Logger::getInstance().logOutput("Ongeldige invoer, probeer opnieuw.\n");
				continue;
			}
			std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
			Logger::getInstance().logInput(std::to_string(choice) + "\n");

			if (choice == 1)
			{
				mCurrentState = State::Gameplay;
				return;
			}
			if (choice == 2)
			{
				mCurrentState = State::Quit;
				return;
			}
		}
	}
	catch (const std::exception& e)
	{
		Logger::getInstance().logError(std::string("Fatale fout in Game::mainMenu: ") + e.what());
	}
	catch (...)
	{
		Logger::getInstance().logError("Onbekende fatale fout in Game::mainMenu");
	}
}

void Game::gameplay()
{
	try
	{
		Logger::getInstance().logOutput("\n=== Spel gestart ===\nType 'help' voor commando's.\n");

		// make sure backend has a start room of correct size, and sync positions
		(void)mFacade.getStartRuimte(mRoomWidth, mRoomHeight);
		mFacade.syncPlayerFromWorld(mPlayer);

		while (true)
		{
			Logger::getInstance().logOutput("\n==============================\n");
			// Render current state (map + stats)
			const Ruimte* roomForRender = mFacade.getCurrentRuimte();
			if (!roomForRender)
			{
				roomForRender = mFacade.getStartRuimte(mRoomWidth, mRoomHeight);
			}

			mFacade.syncPlayerFromWorld(mPlayer);
			mPlayer.updateGoudklomp();

			std::string map = RuimteRenderer::render(roomForRender, mPlayer);
			Logger::getInstance().logOutput("Ruimte (" + std::to_string(mPlayer.getRuimteX()) + "," +
											std::to_string(mPlayer.getRuimteY()) + "):\n" + map + "\n");

			Logger::getInstance().logOutput("HP: " + std::to_string(mPlayer.getLevenspunten()) +
											" | Goud: " + std::to_string(mPlayer.getGoud()) +
											(mPlayer.isGodmodeActief() ? " [GODMODE]" : "") + "\n> ");

			// PLAYER ACTION PHASE
			std::string cmd;
			if (!std::getline(std::cin, cmd))
			{
				mCurrentState = State::Quit;
				return;
			}
			Logger::getInstance().logInput(cmd + "\n");

			// run action (one per turn)
			bool actionParsed = handleCommand(cmd);

			// if command was "quit" etc.
			if (mCurrentState == State::Quit)
			{
				return;
			}

			// death check after player's own action
			if (mPlayer.getLevenspunten() <= 0)
			{
				mCurrentState = State::DeathMenu;
				return;
			}

			// ENEMY PHASE
			int vijandDamage = -1;
			vijandDamage = mFacade.vijandBeurt();
			if (vijandDamage != -1)
			{
				int effectieve = mPlayer.takeDamage(vijandDamage);
				Logger::getInstance().logOutput(
					"Je hebt " + std::to_string(effectieve) + " schade ontvangen van vijanden!\n");
			}

			if (mPlayer.getLevenspunten() <= 0)
			{
				mCurrentState = State::DeathMenu;
				return;
			}
		}
	}
	catch (const std::exception& e)
	{
		Logger::getInstance().logError(std::string("Fatale fout in Game::gameplay: ") + e.what());
		mCurrentState = State::Quit;
	}
	catch (...)
	{
		Logger::getInstance().logError("Onbekende fatale fout in Game::gameplay");
		mCurrentState = State::Quit;
	}
}

bool Game::handleCommand(const std::string& cmd)
{
	try
	{
		auto parseDir = [](const std::string& s, Direction& d) -> bool
		{
			if (s == "noord" || s == "n")
			{
				d = Direction::Noord;
				return true;
			}
			if (s == "zuid" || s == "z")
			{
				d = Direction::Zuid;
				return true;
			}
			if (s == "oost" || s == "o")
			{
				d = Direction::Oost;
				return true;
			}
			if (s == "west" || s == "w")
			{
				d = Direction::West;
				return true;
			}
			return false;
		};

		if (cmd == "quit")
		{
			mCurrentState = State::Quit;
			return true;
		}
		if (cmd == "help")
		{
			showHelp();
			return true;
		}
		if (cmd == "bekijk zelf")
		{
			showPlayer();
			return true;
		}
		if (cmd == "godmode")
		{
			mPlayer.toggleGodmode();
			Logger::getInstance().logOutput(mPlayer.isGodmodeActief() ? "Godmode aan.\n" : "Godmode uit.\n");
			return true;
		}
		if (cmd == "wacht")
		{
			Logger::getInstance().logOutput("Je wacht...\n");
			return true;
		}

		// ga <richting>
		if (cmd.rfind("ga ", 0) == 0)
		{
			Direction d;
			if (!parseDir(cmd.substr(3), d))
			{
				Logger::getInstance().logOutput("Gebruik: ga [noord/zuid/oost/west]\n");
				return false;
			}
			if (mFacade.ga(d))
			{
				mFacade.syncPlayerFromWorld(mPlayer);
				Logger::getInstance().logOutput("Je beweegt.\n");
			}
			else
			{
				Logger::getInstance().logOutput("Je kunt daar niet lopen.\n");
			}
			return true;
		}

		// hak <richting>
		if (cmd.rfind("hak ", 0) == 0)
		{
			Direction d;
			if (!parseDir(cmd.substr(4), d))
			{
				Logger::getInstance().logOutput("Gebruik: hak [noord/zuid/oost/west]\n");
				return false;
			}
			if (mFacade.hak(d))
				Logger::getInstance().logOutput("Je hakt de rots.\n");
			else
				Logger::getInstance().logOutput("Daar is geen rots.\n");
			return true;
		}

		// pak <richting>
		if (cmd.rfind("pak ", 0) == 0)
		{
			Direction d;
			if (!parseDir(cmd.substr(4), d))
			{
				Logger::getInstance().logOutput("Gebruik: pak [noord/zuid/oost/west]\n");
				return false;
			}

			// Check if there's a Kist in that direction
			if (mFacade.getKistAt(d))
			{
				std::size_t kistCount = mFacade.getKistItemCount(d);
				if (kistCount > 0)
				{
					// List items
					Logger::getInstance().logOutput("Items in kist:\n");
					for (std::size_t i = 0; i < kistCount; ++i)
					{
						const char* naam = mFacade.getKistItemName(d, i);
						if (naam)
						{
							Logger::getInstance().logOutput(
								"  " + std::to_string(i + 1) + ". " + std::string(naam) + "\n");
						}
					}

					Logger::getInstance().logOutput("Kies een item (nummer) of 0 om te annuleren: ");
					int choice = 0;
					if (!(std::cin >> choice))
					{
						std::cin.clear();
						std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
						Logger::getInstance().logOutput("Ongeldige invoer.\n");
						return false;
					}
					std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
					Logger::getInstance().logInput(std::to_string(choice) + "\n");

					if (choice > 0 && choice <= static_cast<int>(kistCount))
					{
						if (auto item = mFacade.takeFromKist(d, choice - 1))
						{
							mPlayer.voegItemToe(std::move(item));
							Logger::getInstance().logOutput("Je pakt het object uit de kist.\n");
						}
						else
						{
							Logger::getInstance().logOutput("Kon item niet pakken.\n");
						}
					}
					else if (choice != 0)
					{
						Logger::getInstance().logOutput("Ongeldige keuze.\n");
					}
				}
			}

			// Normal pickup sequence
			else
			{
				if (auto item = mFacade.pak(d))
				{
					mPlayer.voegItemToe(std::move(item));
					Logger::getInstance().logOutput("Je pakt het object op.\n");
				}
				else
				{
					Logger::getInstance().logOutput("Daar ligt niets.\n");
				}
			}
			return true;
		}

		// leg <voorwerp> <richting>
		if (cmd.rfind("leg ", 0) == 0)
		{
			std::string rest = cmd.substr(4);
			size_t sp = rest.rfind(' ');
			if (sp == std::string::npos)
			{
				Logger::getInstance().logOutput("Gebruik: leg [voorwerp] [noord/zuid/oost/west]\n");
				return false;
			}
			std::string itemNaam = rest.substr(0, sp);
			std::string dirStr = rest.substr(sp + 1);

			Direction d;
			if (!parseDir(dirStr, d))
			{
				Logger::getInstance().logOutput("Gebruik: leg [voorwerp] [noord/zuid/oost/west]\n");
				return false;
			}

			// Remove from inventory, try to place, put back on failure
			if (auto item = mPlayer.verwijderItem(itemNaam.c_str()))
			{
				if (mFacade.leg(d, item))
				{
					Logger::getInstance().logOutput("Je legt het neer.\n");
				}
				else
				{
					Logger::getInstance().logOutput("Je kunt dat daar niet neerleggen.\n");
					// Voeg het item weer toe aan de speler indien het neerleggen niet lukt
					mPlayer.voegItemToe(std::move(item));
				}
			}
			else
			{
				Logger::getInstance().logOutput("Je hebt dat item niet.\n");
			}
			return true;
		}

		// draag <voorwerp>
		if (cmd.rfind("draag ", 0) == 0)
		{
			std::string itemNaam = cmd.substr(6);

			if (itemNaam.empty())
			{
				Logger::getInstance().logOutput("Gebruik: draag [wapen of wapenrusting]\n");
				return true;
			}

			if (mPlayer.draagItem(itemNaam.c_str()))
			{
				Logger::getInstance().logOutput("Je draagt nu " + itemNaam + ".\n");
			}
			else
			{
				Logger::getInstance().logOutput("Je kunt dat niet dragen of je hebt het niet.\n");
			}

			return true;
		}

		// aanval
		if (cmd == "aanval")
		{
			int playerDamage = mPlayer.aanval();
			int effectiveDamage = mFacade.aanval(playerDamage);
			if (effectiveDamage != -1)
			{
				Logger::getInstance().logOutput("Je valt aan (" + std::to_string(effectiveDamage) + " schade).\n");
			}
			else
			{
				Logger::getInstance().logOutput("Er staat geen vijand naast je.\n");
			}
			return true;
		}

		// knutsel <voorwerpnaam> <richting>
		if (cmd.rfind("knutsel ", 0) == 0)
		{
			std::string rest = cmd.substr(8);
			size_t sp = rest.rfind(' ');
			if (sp == std::string::npos)
			{
				Logger::getInstance().logOutput("Gebruik: knutsel [voorwerpnaam] [noord/zuid/oost/west]\n");
				return false;
			}

			std::string itemNaam = rest.substr(0, sp);
			std::string dirStr = rest.substr(sp + 1);

			Direction d;
			if (!parseDir(dirStr, d))
			{
				Logger::getInstance().logOutput("Gebruik: knutsel [voorwerpnaam] [noord/zuid/oost/west]\n");
				return false;
			}

			const SpelobjectFactory& factory = mFacade.getSpelobjectFactory();
			Spelobject* prototype = factory.findPrototypeByName(itemNaam.c_str());
			if (!prototype)
			{
				Logger::getInstance().logOutput("Onbekend voorwerp: " + itemNaam + "\n");
				return true;
			}

			char grondstofCode = static_cast<char>(prototype->getKnutselGrondstof());
			int nodigAantal = prototype->getAantalKnutselGrondstof();

			if (nodigAantal <= 0)
			{
				Logger::getInstance().logOutput(itemNaam + " kan niet geknutseld worden.\n");
				return true;
			}

			int beschikbaar = mPlayer.telGrondstof(grondstofCode);
			if (beschikbaar < nodigAantal)
			{
				Logger::getInstance().logOutput("Je hebt niet genoeg grondstoffen (" + std::to_string(beschikbaar) +
												"/" + std::to_string(nodigAantal) + " " + grondstofCode + ").\n");
				return true;
			}

			// Try crafting
			std::unique_ptr<Spelobject> nieuw(prototype->clone());

			if (mFacade.leg(d, nieuw))
			{
				// success -> remove resources
				mPlayer.verwijderGrondstoffen(grondstofCode, nodigAantal);
				Logger::getInstance().logOutput("Je hebt succesvol een " + itemNaam + " geknutseld en neergelegd!\n");
			}
			else
			{
				// Automatically deleted crafted item as it could not be placed
				Logger::getInstance().logOutput("Je kunt daar niets knutselen.\n");
			}

			return true;
		}

		// eet
		if (cmd == "eet")
		{
			int oudeHP = mPlayer.getLevenspunten();
			bool success = mPlayer.eetMedicinaleWortel();

			if (success)
			{
				int herstel = mPlayer.getLevenspunten() - oudeHP;
				Logger::getInstance().logOutput(
					"Je eet een Medicinale Wortel en herstelt " + std::to_string(herstel) + " HP.\n");
			}
			else
			{
				Logger::getInstance().logOutput("Je hebt geen medicinale wortel.\n");
			}
			return true;
		}

		Logger::getInstance().logOutput("Onbekend commando. Type 'help'.\n");
		return false;
	}
	catch (const std::exception& e)
	{
		Logger::getInstance().logError(std::string("Fout in Game::handleCommand: ") + e.what());
		return false;
	}
	catch (...)
	{
		Logger::getInstance().logError("Onbekende fout in Game::handleCommand");
		return false;
	}
}

void Game::showHelp()
{
	Logger::getInstance().logOutput("\n=== Beschikbare commando's ===\n"
									"  ga <noord/zuid/oost/west>         - Beweeg in een richting\n"
									"  hak <richting>                    - Hak een rots\n"
									"  pak <richting>                    - Pak een voorwerp op\n"
									"  leg <voorwerp> <richting>         - Leg een voorwerp neer\n"
									"  bekijk zelf                       - Toon speler en inventaris\n"
									"  knutsel <voorwerp> <richting>     - Knutsel een item\n"
									"  aanval                            - Val een willekeurige nabije Vijand aan\n"
									"  draag <voorwerp>                  - Draag een wapen of wapenrusting\n"
									"  eet                               - Eet een Medicinale Wortel\n"
									"  godmode                           - Schakel godmode in/uit\n"
									"  wacht                             - Sla je beurt over\n"
									"  quit                              - Stop het spel\n"
									"===============================\n");
}

void Game::showPlayer()
{
	try
	{
		Logger::getInstance().logOutput("\n=== Speler Info ===\n");
		Logger::getInstance().logOutput("Naam: " + mPlayer.getNaam() + "\n");
		Logger::getInstance().logOutput("Levenspunten: " + std::to_string(mPlayer.getLevenspunten()) + "\n");
		Logger::getInstance().logOutput("Goud: " + std::to_string(mPlayer.getGoud()) + "\n");

		if (mPlayer.getGedragenWapen())
			Logger::getInstance().logOutput(std::string("Wapen: ") + mPlayer.getGedragenWapen()->getNaam() + "\n");
		else
			Logger::getInstance().logOutput("Wapen: (geen)\n");

		if (mPlayer.getGedragenWapenrusting())
			Logger::getInstance().logOutput(
				std::string("Wapenrusting: ") + mPlayer.getGedragenWapenrusting()->getNaam() + "\n");
		else
			Logger::getInstance().logOutput("Wapenrusting: (geen)\n");

		Logger::getInstance().logOutput("\nInventaris:\n");
		const auto& items = mPlayer.getInventaris();

		// aggregate by name
		std::map<std::string, int> counts;
		for (const auto& item : items)
		{
			if (!item)
				continue;
			counts[item->getNaam()] += 1;
		}

		if (counts.empty())
		{
			Logger::getInstance().logOutput("  (leeg)\n");
		}
		else
		{
			for (auto it = counts.begin(); it != counts.end(); ++it)
			{
				std::string line = "  - " + it->first;
				if (it->second > 1)
					line += " x" + std::to_string(it->second);
				line += "\n";
				Logger::getInstance().logOutput(line);
			}
		}
		Logger::getInstance().logOutput("===================\n\n");
	}
	catch (const std::exception& e)
	{
		Logger::getInstance().logError(std::string("Fout in Game::showPlayer: ") + e.what());
	}
	catch (...)
	{
		Logger::getInstance().logError("Onbekende fout in Game::showPlayer");
	}
}

void Game::deathMenu()
{
	try
	{
		// score = goudklompen
		int score = mPlayer.getGoud();

		Logger::getInstance().logOutput("\nJe bent verslagen.\n"
										"Je score (aantal goudklompen): " +
										std::to_string(score) +
										"\n"
										"Wat wil je doen?\n"
										"1. Opnieuw\n"
										"2. Quit\n");

		int c = 0;
		if (!(std::cin >> c))
		{
			std::cin.clear();
			std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
			c = 2;
		}
		std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
		Logger::getInstance().logInput(std::to_string(c) + "\n");

		if (c == 1)
		{
			// full restart flow: choose size + name again
			setupNewRun();
			mCurrentState = State::Gameplay;
		}
		else
		{
			mCurrentState = State::Quit;
		}
	}
	catch (const std::exception& e)
	{
		Logger::getInstance().logError(std::string("Fout in Game::deathMenu: ") + e.what());
		mCurrentState = State::Quit;
	}
	catch (...)
	{
		Logger::getInstance().logError("Onbekende fout in Game::deathMenu");
		mCurrentState = State::Quit;
	}
}
