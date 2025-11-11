#ifndef SPELWERELD_H
#define SPELWERELD_H

#include "CustomVector.h"
#include "Direction.h"
#include "Ruimte.h"
#include "RuimteFactory.h"
#include "Spelobject.h"
#include "SpelobjectFactory.h"
#include "Vijand.h"
#include "VijandFactory.h"

// Simple world that stores pointers to Ruimte created on demand
struct RoomEntry
{
	int x;
	int y;
	Ruimte* room;
};

#include <memory>

class Spelwereld
{
public:
	Spelwereld();
	~Spelwereld();

	// Rule of 5: provide copy + move semantics and assignments
	Spelwereld(const Spelwereld& other);
	Spelwereld& operator=(const Spelwereld& other);

	Spelwereld(Spelwereld&& other) noexcept;
	Spelwereld& operator=(Spelwereld&& other) noexcept;

	Ruimte* getOrCreateRuimte(int x, int y, int width, int height, Direction doorDir);
	Ruimte* getRuimte(int x, int y) const;
	Ruimte* createStartRuimte(int x, int y, int width, int height);
	Ruimte* getStartRuimte(int width, int height);
	bool removeRuimte(int x, int y);
	Ruimte* getCurrentRuimte() const;

	// Object management - transfer ownership between room and caller
	Spelobject* pickUpObject(int roomX, int roomY, int localX, int localY);
	bool placeObject(int roomX, int roomY, int localX, int localY, Spelobject* obj);

	bool kanLopen(Direction richting) const;
	bool verplaatsSpeler(Direction richting, bool& newRoomCreated, Direction& enteredFrom, int& entryDoorX,
						 int& entryDoorY);
	bool hak(Direction richting, VijandFactory& vf, SpelobjectFactory& of);
	Spelobject* pak(Direction richting);
	bool leg(Direction richting, Spelobject* obj);
	bool legInKist(Direction richting, Spelobject* obj);
	bool legInAfvalmand(Direction richting, Spelobject* obj);
	int aanval(int& schadeUit, SpelobjectFactory& of);
	int vijandenBewegenEnAanvallen();
	void toonKaart() const;
	void destroy();

	// Player X/Y getters for frontend
	int getPlayerX() const;
	int getPlayerY() const;

	// Room tracking
	int getRoomX() const;
	int getRoomY() const;

	void setDefaultRoomSize(int width, int height);

private:
	// store rooms using CustomVector of entries mapping coords -> room
	CustomVector<RoomEntry> mRooms;
	RuimteFactory mRuimteFactory;

	Ruimte* mCurrentRoom = nullptr;
	int mPlayerX = 0;
	int mPlayerY = 0;
	int mRoomX = 0;
	int mRoomY = 0;

	int mDefaultWidth = 10;
	int mDefaultHeight = 10;
};

#endif // SPELWERELD_H
