#ifndef SPEL_FACADE_H
#define SPEL_FACADE_H

#include "BACKEND/CustomVector.h"
#include "BACKEND/Ruimte.h"
#include "BACKEND/RuimteFactory.h"
#include "BACKEND/Spelobject.h"
#include "BACKEND/SpelobjectFactory.h"
#include "BACKEND/Spelwereld.h"
#include "BACKEND/VijandFactory.h"
#include "BACKEND/Kist.h"
#include "Direction.h"
#include "FRONTEND/Player.h"
#include <memory>

class SpelFacade
{
public:
	SpelFacade(std::unique_ptr<Spelwereld> world);
	~SpelFacade() = default;

	// --- Initialization ---
	void loadDatabase(const char* databasePath);
	void resetWereld();

	// --- World / Room Management ---
	Ruimte* getOrCreateRuimte(int x, int y, int width, int height, Direction doorDir);
	Ruimte* getRuimte(int x, int y);
	Ruimte* createStartRuimte(int x, int y, int width, int height);
	Ruimte* getStartRuimte(int width, int height);
	const Ruimte* getCurrentRuimte() const;

	// --- Object Handling ---
	std::unique_ptr<Spelobject> pickUpObject(int roomX, int roomY, int localX, int localY);
	bool placeObject(int roomX, int roomY, int localX, int localY, std::unique_ptr<Spelobject>&& obj);

	// Factory-based creation
	std::unique_ptr<Spelobject> createObjectByName(const char* naam);
	std::unique_ptr<Spelobject> createGrondstofByCode(char code);

	// --- Player / Movement ---
	bool kanLopen(Direction richting) const;
	bool ga(Direction richting);
	bool hak(Direction richting);
	int getPlayerX() const;
	int getPlayerY() const;
	void syncPlayerFromWorld(Player& p) const;

	// --- Interaction ---
	std::unique_ptr<Spelobject> pak(Direction richting);
	bool leg(Direction richting, std::unique_ptr<Spelobject>& item);

	// --- Combat ---
	int aanval(int& schadeUit);
	int vijandBeurt();

	// --- Room tracking ---
	int getRoomX() const;
	int getRoomY() const;

	// --- Access to factorys ---
	const SpelobjectFactory& getSpelobjectFactory() const;

	// --- Utility ---
	void setDefaultRoomSize(int width, int height);

	// --- Kist interaction ---
	Kist* getKistAt(Direction richting) const;
	std::size_t getKistItemCount(Direction richting) const;
    const char* getKistItemName(Direction richting, std::size_t index) const;
    std::unique_ptr<Spelobject> takeFromKist(Direction richting, int index);

private:
	SpelobjectFactory mSpelobjectFactory;
	VijandFactory mVijandFactory;

	std::unique_ptr<Spelwereld> mWorld;
	CustomVector<Spelobject*> mAvailableObjects; // Database prototypes
	CustomVector<Vijand*> mAvailableEnemies;	 // Database prototypes

	int mRoomWidth = 0;
	int mRoomHeight = 0;
};

#endif // SPEL_FACADE_H
