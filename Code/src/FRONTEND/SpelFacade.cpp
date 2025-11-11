#include "SpelFacade.h"
#include "BACKEND/DatabaseLoader.h"
#include "Logger.h"
#include <cstring>
#include <utility>

SpelFacade::SpelFacade(std::unique_ptr<Spelwereld> world) : mWorld(std::move(world)) {}

void SpelFacade::loadDatabase(const char* databasePath)
{
	DatabaseLoader loader;
	int count = 0;
	mAvailableObjects = loader.laadSpelobjecten(databasePath, count);
	mAvailableEnemies = loader.laadVijanden(databasePath, count);

	// Pass read-only prototypes to factory (pointer-to-list, not ownership)
	mSpelobjectFactory.setTemplates(mAvailableObjects);
	mSpelobjectFactory.addDefaultGrondstoffen();

	mVijandFactory.setTemplates(mAvailableEnemies);
}

// --- Room management ---
Ruimte* SpelFacade::getOrCreateRuimte(int x, int y, int width, int height, Direction doorDir)
{
	return mWorld ? mWorld->getOrCreateRuimte(x, y, width, height, doorDir) : nullptr;
}

Ruimte* SpelFacade::getRuimte(int x, int y) { return mWorld ? mWorld->getRuimte(x, y) : nullptr; }

Ruimte* SpelFacade::createStartRuimte(int x, int y, int width, int height)
{
	return mWorld ? mWorld->createStartRuimte(x, y, width, height) : nullptr;
}

Ruimte* SpelFacade::getStartRuimte(int width, int height)
{
	return mWorld ? mWorld->getStartRuimte(width, height) : nullptr;
}

const Ruimte* SpelFacade::getCurrentRuimte() const { return mWorld ? mWorld->getCurrentRuimte() : nullptr; }

// --- Object handling ---
std::unique_ptr<Spelobject> SpelFacade::pickUpObject(int roomX, int roomY, int localX, int localY)
{
	if (!mWorld)
		return nullptr;
	Spelobject* obj = mWorld->pickUpObject(roomX, roomY, localX, localY);
	return std::unique_ptr<Spelobject>(obj);
}

bool SpelFacade::placeObject(int roomX, int roomY, int localX, int localY, std::unique_ptr<Spelobject>&& obj)
{
	return (mWorld && obj) ? mWorld->placeObject(roomX, roomY, localX, localY, obj.release()) : false;
}

// --- Factory creation ---
std::unique_ptr<Spelobject> SpelFacade::createObjectByName(const char* naam)
{
	return std::unique_ptr<Spelobject>(mSpelobjectFactory.createByName(naam));
}

std::unique_ptr<Spelobject> SpelFacade::createGrondstofByCode(char code)
{
	return std::unique_ptr<Spelobject>(mSpelobjectFactory.createByCode(code));
}

// --- Player and movement ---
bool SpelFacade::kanLopen(Direction richting) const { return mWorld && mWorld->kanLopen(richting); }

bool SpelFacade::ga(Direction richting)
{
	try
	{
		if (!mWorld)
			return false;

		bool newRoom = false;
		Direction enteredFrom;
		int entryDoorX = 0, entryDoorY = 0;

		// Determine if we entered a new room
		bool moved = mWorld->verplaatsSpeler(richting, newRoom, enteredFrom, entryDoorX, entryDoorY);
		if (!moved)
			return false;

		// Only run this when we actually entered through a door (new or existing)
		if (enteredFrom != Direction::Default || newRoom)
		{
			Direction opposite;
			switch (enteredFrom)
			{
			case Direction::Noord:
				opposite = Direction::Zuid;
				break;
			case Direction::Zuid:
				opposite = Direction::Noord;
				break;
			case Direction::Oost:
				opposite = Direction::West;
				break;
			case Direction::West:
				opposite = Direction::Oost;
				break;
			}

			Ruimte* current = mWorld->getCurrentRuimte();
			if (current)
			{
				const int W = current->getWidth();
				const int H = current->getHeight();

				auto clamp = [](int v, int lo, int hi) { return v < lo ? lo : (v > hi ? hi : v); };

				int bx = 0, by = 0;
				switch (opposite)
				{
				case Direction::Noord:
					by = 0;
					bx = clamp(entryDoorX, 1, W - 2);
					break;
				case Direction::Zuid:
					by = H - 1;
					bx = clamp(entryDoorX, 1, W - 2);
					break;
				case Direction::Oost:
					bx = W - 1;
					by = clamp(entryDoorY, 1, H - 2);
					break;
				case Direction::West:
					bx = 0;
					by = clamp(entryDoorY, 1, H - 2);
					break;
				}

				// Create the door if it doesn't exist
				Spelobject* existing = current->getAt(bx, by);
				if (!existing || !dynamic_cast<Deur*>(existing))
				{
					std::unique_ptr<Spelobject> deur(mSpelobjectFactory.createByName("Deur"));
					if (auto* d = deur ? dynamic_cast<Deur*>(deur.get()) : nullptr)
					{
						d->setDirection(opposite);
						current->setAt(bx, by, d);
						deur.release(); // Release ownership to the backend
					}
				}
			}

			// Move the player in the new room and clear the area around them
			mWorld->getCurrentRuimte()->setTile(mWorld->getPlayerX(), mWorld->getPlayerY(), '.');
			mWorld->getCurrentRuimte()->clearFiveAround(mWorld->getPlayerX(), mWorld->getPlayerY());
		}

		return true;
	}
	catch (const std::exception& e)
	{
		Logger::getInstance().logError(std::string("SpelFacade::ga fout: ") + e.what());
		return false;
	}
	catch (...)
	{
		Logger::getInstance().logError("Onbekende fout in SpelFacade::ga");
		return false;
	}
}

bool SpelFacade::hak(Direction richting) { return mWorld && mWorld->hak(richting, mVijandFactory, mSpelobjectFactory); }

int SpelFacade::getPlayerX() const { return mWorld ? mWorld->getPlayerX() : 0; }

int SpelFacade::getPlayerY() const { return mWorld ? mWorld->getPlayerY() : 0; }

void SpelFacade::syncPlayerFromWorld(Player& p) const
{
	if (!mWorld)
		return;
	p.setPositie(getPlayerX(), getPlayerY(), getRoomX(), getRoomY());
}

// --- Interaction ---
std::unique_ptr<Spelobject> SpelFacade::pak(Direction richting)
{
	return std::unique_ptr<Spelobject>(mWorld ? mWorld->pak(richting) : nullptr);
}

bool SpelFacade::leg(Direction richting, std::unique_ptr<Spelobject>& item)
{
	if (!mWorld)
		return false;

	Spelobject* raw = item.get();
	bool success = mWorld->leg(richting, raw);
	if (success)
	{
		(void)item.release(); // release to backend
	}
	return success;
}

// --- Combat ---
int SpelFacade::aanval(int& schadeUit) { return mWorld ? mWorld->aanval(schadeUit, mSpelobjectFactory) : -1; }

int SpelFacade::vijandBeurt()
{
	if (mWorld && mWorld->getCurrentRuimte()->getVijanden().size() > 0)
	{
		return mWorld->vijandenBewegenEnAanvallen();
	}
	else
	{
		return -1;
	}
}

// --- Utility ---
void SpelFacade::resetWereld()
{
	if (mWorld)
		mWorld->destroy();
}

void SpelFacade::setDefaultRoomSize(int width, int height)
{
	if (mWorld)
		mWorld->setDefaultRoomSize(width, height);
}

int SpelFacade::getRoomX() const { return mWorld ? mWorld->getRoomX() : 0; }

int SpelFacade::getRoomY() const { return mWorld ? mWorld->getRoomY() : 0; }

// --- Access to factorys ---
const SpelobjectFactory& SpelFacade::getSpelobjectFactory() const { return mSpelobjectFactory; }

// --- Kist interaction ---
std::size_t SpelFacade::getKistItemCount(Direction richting) const
{
	Kist* kist = getKistAt(richting);

	return kist ? kist->getItemCount() : 0;
}

const char* SpelFacade::getKistItemName(Direction richting, std::size_t index) const
{
	Kist* kist = getKistAt(richting);

	if (!kist)
		return nullptr;

	const Spelobject* item = kist->getItemAt(index);
	return item ? item->getNaam() : nullptr;
}

std::unique_ptr<Spelobject> SpelFacade::takeFromKist(Direction richting, int index)
{
	Kist* kist = getKistAt(richting);

	if (!kist)
		return nullptr;

	return std::unique_ptr<Spelobject>(kist->takeItem(index));
}

Kist* SpelFacade::getKistAt(Direction richting) const
{
	if (!mWorld)
		return nullptr;

	Ruimte* room = mWorld->getCurrentRuimte();
	if (!room)
		return nullptr;

	int dx = 0, dy = 0;
	switch (richting)
	{
	case Direction::Noord:
		dy = -1;
		break;
	case Direction::Zuid:
		dy = 1;
		break;
	case Direction::Oost:
		dx = 1;
		break;
	case Direction::West:
		dx = -1;
		break;
	default:
		return nullptr;
	}

	int px = mWorld->getPlayerX();
	int py = mWorld->getPlayerY();

	Spelobject* obj = room->getAt(px + dx, py + dy);
	Kist* kist = dynamic_cast<Kist*>(obj);

	return kist;
}
