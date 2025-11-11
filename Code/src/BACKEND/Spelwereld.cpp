#include "Spelwereld.h"
#include "Afvalmand.h"
#include "Kist.h"
#include "Logger.h"
#include "RandomEngine.h"
#include "RuimteRenderer.h"
#include "SpelobjectFactory.h"
#include "Vijand.h"
#include "VijandFactory.h"

// Helper: deep-copy one room (tiles, objects, enemies)
static Ruimte* cloneRuimteFrom(const Ruimte* src, const RuimteFactory& factory)
{
	if (!src)
		return nullptr;
	Ruimte* dst = factory.createRuimte(src->getWidth(), src->getHeight());

	// copy tiles / objects: clone objects where present,
	for (int y = 0; y < src->getHeight(); ++y)
	{
		for (int x = 0; x < src->getWidth(); ++x)
		{
			Spelobject* o = src->getAt(x, y);
			if (o)
			{
				Spelobject* clone = o->clone();
				dst->setAt(x, y, clone);
			}
			else if (src->isRock(x, y))
			{
				dst->setTile(x, y, '0');
			}
			else
			{
				dst->setTile(x, y, '.');
			}
		}
	}

	// copy enemies (preserve position)
	const CustomVector<Vijand*>& vij = src->getVijanden();
	for (std::size_t i = 0; i < vij.size(); ++i)
	{
		if (!vij[i])
			continue;
		Vijand* vclone = vij[i]->clone();
		dst->plaatsVijand(vclone->getX(), vclone->getY(), vclone);
	}

	return dst;
}

// Default ctor
Spelwereld::Spelwereld() : mRooms(), mRuimteFactory() {}

// Destructor
Spelwereld::~Spelwereld() { destroy(); }

// Copy ctor (deep copy)
Spelwereld::Spelwereld(const Spelwereld& other)
	: mRooms(), mRuimteFactory(other.mRuimteFactory), mCurrentRoom(nullptr), mPlayerX(other.mPlayerX),
	  mPlayerY(other.mPlayerY), mRoomX(other.mRoomX), mRoomY(other.mRoomY)
{
	for (std::size_t i = 0; i < other.mRooms.size(); ++i)
	{
		RoomEntry e = other.mRooms[i];
		Ruimte* copy = cloneRuimteFrom(e.room, mRuimteFactory);
		RoomEntry ne{e.x, e.y, copy};
		mRooms.push_back(ne);
		if (other.mCurrentRoom == e.room)
		{
			mCurrentRoom = copy;
		}
	}
}

// Copy assignment (deep copy)
Spelwereld& Spelwereld::operator=(const Spelwereld& other)
{
	if (this == &other)
		return *this;

	// delete current rooms
	destroy();

	// copy factory and positional state
	mRuimteFactory = other.mRuimteFactory;
	mPlayerX = other.mPlayerX;
	mPlayerY = other.mPlayerY;
	mRoomX = other.mRoomX;
	mRoomY = other.mRoomY;
	mCurrentRoom = nullptr;

	// deep-copy rooms
	for (std::size_t i = 0; i < other.mRooms.size(); ++i)
	{
		RoomEntry e = other.mRooms[i];
		Ruimte* copy = cloneRuimteFrom(e.room, mRuimteFactory);
		RoomEntry ne{e.x, e.y, copy};
		mRooms.push_back(ne);
		if (other.mCurrentRoom == e.room)
		{
			mCurrentRoom = copy;
		}
	}

	return *this;
}

// Move ctor (steal pointers)
Spelwereld::Spelwereld(Spelwereld&& other) noexcept
	: mRooms(), mRuimteFactory(std::move(other.mRuimteFactory)), mCurrentRoom(other.mCurrentRoom),
	  mPlayerX(other.mPlayerX), mPlayerY(other.mPlayerY), mRoomX(other.mRoomX), mRoomY(other.mRoomY)
{
	// steal room entries (shallow copy of RoomEntry structs that contain pointers)
	for (std::size_t i = 0; i < other.mRooms.size(); ++i)
	{
		mRooms.push_back(other.mRooms[i]);
	}
	other.mRooms.clear();
	other.mCurrentRoom = nullptr;
	other.mPlayerX = other.mPlayerY = other.mRoomX = other.mRoomY = 0;
}

// Move assignment (steal)
Spelwereld& Spelwereld::operator=(Spelwereld&& other) noexcept
{
	if (this == &other)
		return *this;

	// destroy current
	destroy();

	// steal factory and state
	mRuimteFactory = std::move(other.mRuimteFactory);
	mPlayerX = other.mPlayerX;
	mPlayerY = other.mPlayerY;
	mRoomX = other.mRoomX;
	mRoomY = other.mRoomY;
	mCurrentRoom = other.mCurrentRoom;

	// steal room entries
	for (std::size_t i = 0; i < other.mRooms.size(); ++i)
	{
		mRooms.push_back(other.mRooms[i]);
	}
	other.mRooms.clear();
	other.mCurrentRoom = nullptr;
	other.mPlayerX = other.mPlayerY = other.mRoomX = other.mRoomY = 0;

	return *this;
}

// Spelwereld::getOrCreateRuimte
// doorDir: indicates from which side the player entered. Used by the caller
// (SpelFacade) to place the matching return door. The RuimteFactory itself
// does not use this parameter.
Ruimte* Spelwereld::getOrCreateRuimte(int x, int y, int width, int height, Direction doorDir)
{
	try
	{
		// Find by coordinates
		for (std::size_t i = 0; i < mRooms.size(); ++i)
		{
			RoomEntry e = mRooms[i];
			if (e.x == x && e.y == y)
				return e.room;
		}

		// Use factory to create a new room with door in specified direction
		Ruimte* nieuw = mRuimteFactory.createRuimte(width, height);
		RoomEntry ne{x, y, nieuw};
		mRooms.push_back(ne);
		return nieuw;
	}
	catch (const std::exception& e)
	{
		char buf[512];
		std::snprintf(buf, sizeof(buf), "Spelwereld fout: %s", e.what());
		Logger::getInstance().logError(buf);
		return nullptr;
	}
	catch (...)
	{
		Logger::getInstance().logError("Onbekende fout in Spelwereld::getOrCreateRuimte");
		return nullptr;
	}
}

Ruimte* Spelwereld::getRuimte(int x, int y) const
{
	for (std::size_t i = 0; i < mRooms.size(); ++i)
	{
		RoomEntry e = mRooms[i];
		if (e.x == x && e.y == y)
			return e.room;
	}
	return nullptr;
}

Ruimte* Spelwereld::createStartRuimte(int x, int y, int width, int height)
{
	// If a room already exists at the coords, delete it and replace it.
	for (std::size_t i = 0; i < mRooms.size(); ++i)
	{
		if (mRooms[i].x == x && mRooms[i].y == y)
		{
			delete mRooms[i].room;
			Ruimte* nieuw = mRuimteFactory.createStartRuimte(width, height);
			mRooms[i].room = nieuw;
			return nieuw;
		}
	}

	// otherwise create and append
	Ruimte* nieuw = mRuimteFactory.createStartRuimte(width, height);
	RoomEntry ne{x, y, nieuw};
	mRooms.push_back(ne);
	return nieuw;
}

// Single canonical start room accessor (creates room at (0,0) on first call)
Ruimte* Spelwereld::getStartRuimte(int width, int height)
{
	const int sx = 0, sy = 0;
	Ruimte* r = getRuimte(sx, sy);
	if (!r)
		r = createStartRuimte(sx, sy, width, height);
	// ensure world knows where we are
	mCurrentRoom = r;
	mRoomX = sx;
	mRoomY = sy;

	// put player in center
	mPlayerX = r->getWidth() / 2;
	mPlayerY = r->getHeight() / 2;
	return r;
}

bool Spelwereld::removeRuimte(int x, int y)
{
	for (std::size_t i = 0; i < mRooms.size(); ++i)
	{
		RoomEntry e = mRooms[i];
		if (e.x == x && e.y == y)
		{
			delete e.room;
			// remove entry by swapping with last and popping
			std::size_t last = mRooms.size() - 1;
			if (i != last)
				mRooms[i] = mRooms[last];
			mRooms.pop_back();
			return true;
		}
	}
	return false;
}

Ruimte* Spelwereld::getCurrentRuimte() const { return mCurrentRoom; }

Spelobject* Spelwereld::pickUpObject(int roomX, int roomY, int localX, int localY)
{
	Ruimte* r = getRuimte(roomX, roomY);
	if (!r)
		return nullptr;
	return r->takeAt(localX, localY);
}

bool Spelwereld::placeObject(int roomX, int roomY, int localX, int localY, Spelobject* obj)
{
	if (!obj)
		return false;
	Ruimte* r = getRuimte(roomX, roomY);
	if (!r)
		return false; // Room must exist
	if (localX < 0 || localY < 0 || localX >= r->getWidth() || localY >= r->getHeight())
		return false;
	Spelobject* existing = r->getAt(localX, localY);
	if (existing)
		return false; // Position occupied
	r->setAt(localX, localY, obj);
	return true;
}

// --- Helper ---
static void richtingOffset(Direction dir, int& dx, int& dy)
{
	dx = 0;
	dy = 0;
	switch (dir)
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
	}
}

// --- Movement / navigation ---
bool Spelwereld::kanLopen(Direction richting) const
{
	if (!mCurrentRoom)
		return false;

	int dx, dy;
	richtingOffset(richting, dx, dy);

	const int nx = mPlayerX + dx;
	const int ny = mPlayerY + dy;

	return mCurrentRoom->isWalkable(nx, ny);
}

bool Spelwereld::verplaatsSpeler(Direction richting, bool& newRoomCreated, Direction& enteredFrom, int& entryDoorX,
								 int& entryDoorY)
{
	try
	{
		newRoomCreated = false;
		enteredFrom = Direction::Default;
		entryDoorX = entryDoorY = 0;

		//Create start room if needed
		if (!mCurrentRoom)
		{
			mCurrentRoom = getStartRuimte(mDefaultWidth, mDefaultHeight);
			mPlayerX = mCurrentRoom->getWidth() / 2;
			mPlayerY = mCurrentRoom->getHeight() / 2;
			mCurrentRoom->clearFiveAround(mPlayerX, mPlayerY);
			return true;
		}


		if (!kanLopen(richting))
			return false;

		// Calculate new player position
		int dx = 0, dy = 0;
		richtingOffset(richting, dx, dy);
		int nx = mPlayerX + dx;
		int ny = mPlayerY + dy;


		// Enter door to next room in case we walked onto a door
		if (mCurrentRoom->isDoor(nx, ny))
		{
			Direction doorDir = mCurrentRoom->getDoorDirection(nx, ny);
			enteredFrom = doorDir;
			entryDoorX = nx;
			entryDoorY = ny;

			int rx = 0, ry = 0;
			switch (doorDir)
			{
			case Direction::Noord:
				ry = -1;
				break;
			case Direction::Zuid:
				ry = 1;
				break;
			case Direction::Oost:
				rx = 1;
				break;
			case Direction::West:
				rx = -1;
				break;
			}

			mRoomX += rx;
			mRoomY += ry;

			//Check if the room exists already or we are going to create it
			Ruimte* existing = getRuimte(mRoomX, mRoomY);
			newRoomCreated = (existing == nullptr);

			Ruimte* next =
				getOrCreateRuimte(mRoomX, mRoomY, mCurrentRoom->getWidth(), mCurrentRoom->getHeight(), doorDir);
			mCurrentRoom = next;

			auto clamp = [](int v, int lo, int hi) { return v < lo ? lo : (v > hi ? hi : v); };

			// Determine player spawn just inside the new room based on entry direction
			const int W = next->getWidth();
			const int H = next->getHeight();

			// Default spawn inside bounds
			mPlayerX = W / 2;
			mPlayerY = H / 2;

			switch (doorDir)
			{
			case Direction::Noord:
				// Came from north → door was on the top wall in previous room
				mPlayerX = clamp(entryDoorX, 1, W - 2);
				mPlayerY = H - 2; // just inside bottom wall
				break;
			case Direction::Zuid:
				// Came from south → door was on the bottom wall in previous room
				mPlayerX = clamp(entryDoorX, 1, W - 2);
				mPlayerY = 1; // just inside top wall
				break;
			case Direction::Oost:
				// Came from east → door was on the right wall in previous room
				mPlayerX = 1; // just inside left wall
				mPlayerY = clamp(entryDoorY, 1, H - 2);
				break;
			case Direction::West:
				// Came from west → door was on the left wall in previous room
				mPlayerX = W - 2; // just inside right wall
				mPlayerY = clamp(entryDoorY, 1, H - 2);
				break;
			}

			//Check if there is an enemy on the tile we are supposed to be placed on
			CustomVector<Vijand*>& vijanden = mCurrentRoom->getVijanden();
			for (std::size_t i = 0; i < vijanden.size(); ++i)
			{
				Vijand* v = vijanden[i];
				if (v && v->getX() == mPlayerX && v->getY() == mPlayerY)
				{
					//Move the player one tile in another direction if possible
					if (mCurrentRoom->isWalkable(mPlayerX + 1, mPlayerY) && !(mCurrentRoom->isDoor(mPlayerX + 1, mPlayerY)) && mCurrentRoom->isInside(mPlayerX + 1, mPlayerY))
					{
						mPlayerX += 1;
					}
					else if (mCurrentRoom->isWalkable(mPlayerX - 1, mPlayerY) && !(mCurrentRoom->isDoor(mPlayerX - 1, mPlayerY)) && mCurrentRoom->isInside(mPlayerX - 1, mPlayerY))
					{
						mPlayerX -= 1;
					}
					else if (mCurrentRoom->isWalkable(mPlayerX, mPlayerY + 1) && !(mCurrentRoom->isDoor(mPlayerX, mPlayerY + 1)) && mCurrentRoom->isInside(mPlayerX, mPlayerY + 1))
					{
						mPlayerY += 1;
					}
					else if (mCurrentRoom->isWalkable(mPlayerX, mPlayerY - 1) && !(mCurrentRoom->isDoor(mPlayerX, mPlayerY - 1)) && mCurrentRoom->isInside(mPlayerX, mPlayerY - 1))
					{
						mPlayerY -= 1;
					}

					//Otherwise just move on the enemy for now, could fix later
					break;
				}
			}
			return true;
		}

		// === Normal movement ===
		mPlayerX = nx;
		mPlayerY = ny;
		return true;
	}
	catch (const std::exception& e)
	{
		char buf[512];
		std::snprintf(buf, sizeof(buf), "Spelwereld fout: %s", e.what());
		Logger::getInstance().logError(buf);
		return false;
	}
	catch (...)
	{
		Logger::getInstance().logError("Onbekende fout in Spelwereld::verplaatsSpeler");
		return false;
	}
}

// --- Resource gathering / item placement ---
bool Spelwereld::hak(Direction richting, VijandFactory& vf, SpelobjectFactory& of)
{
	if (!mCurrentRoom)
		return false;

	int dx = 0, dy = 0;
	richtingOffset(richting, dx, dy);
	int tx = mPlayerX + dx;
	int ty = mPlayerY + dy;

	if (!mCurrentRoom->isInside(tx, ty))
		return false;

	// --- Case 1: hak steen / rots ---
	if (mCurrentRoom->isRock(tx, ty))
	{
		mCurrentRoom->setTile(tx, ty, '.');

		const int roll = RandomEngine::getRandomInt(1, 10); // 1..10
		if (roll == 1)
		{
			Vijand* vij = vf.createRandomVijand();
			mCurrentRoom->plaatsVijand(tx, ty, vij);
		}
		else
		{
			Spelobject* grondstof = of.createRandomGrondstof();
			if (grondstof && !mCurrentRoom->getAt(tx, ty))
				mCurrentRoom->setAt(tx, ty, grondstof);
			else
				delete grondstof;
		}
		return true;
	}

	// --- Case 2: hak meubilair ---
	Spelobject* obj = mCurrentRoom->getAt(tx, ty);
	if (!obj)
		return false;

	// Take it out without auto-delete (avoids double free)
	obj = mCurrentRoom->takeAt(tx, ty);

	Meubilair* meub = dynamic_cast<Meubilair*>(obj);
	if (!meub)
	{
		// not a furniture item, put it back
		mCurrentRoom->setAt(tx, ty, obj);
		return false;
	}

	// Record crafting requirements before deleting
	const int grondstofCode = meub->getKnutselGrondstof();
	const int aantal = meub->getAantalKnutselGrondstof();

	// Delete the meubilair safely (only once)
	delete meub;

	// Try to drop grondstoffen around
	static const int OFFSETS[9][2] = {{0, 0}, {1, 0}, {-1, 0}, {0, 1}, {0, -1}, {1, 1}, {-1, 1}, {1, -1}, {-1, -1}};

	for (int i = 0; i < aantal; ++i)
	{
		if (grondstofCode == 0)
			break;

		Spelobject* drop = of.createByCode(static_cast<char>(grondstofCode));
		if (!drop)
			continue;

		bool placed = false;
		for (auto& off : OFFSETS)
		{
			int nx = tx + off[0];
			int ny = ty + off[1];
			if (!mCurrentRoom->isInside(nx, ny))
				continue;
			if (mCurrentRoom->isRock(nx, ny))
				continue;
			if (mCurrentRoom->getAt(nx, ny))
				continue;

			mCurrentRoom->setAt(nx, ny, drop);
			placed = true;
			break;
		}

		if (!placed)
			delete drop; // nowhere to place
	}

	return true;
}

bool Spelwereld::legInKist(Direction richting, Spelobject* obj)
{
	if (!mCurrentRoom || !obj)
		return false;
	int dx, dy;
	richtingOffset(richting, dx, dy);
	Kist* k = dynamic_cast<Kist*>(mCurrentRoom->getAt(mPlayerX + dx, mPlayerY + dy));
	if (k)
	{
		k->addItem(obj);
		return true;
	}
	return false;
}

bool Spelwereld::legInAfvalmand(Direction richting, Spelobject* obj)
{
	if (!mCurrentRoom || !obj)
		return false;
	int dx, dy;
	richtingOffset(richting, dx, dy);
	Afvalmand* a = dynamic_cast<Afvalmand*>(mCurrentRoom->getAt(mPlayerX + dx, mPlayerY + dy));
	if (a)
	{
		if (a->acceptItem(obj)) {
			return true;
		}
	}
	return false;
}

bool Spelwereld::leg(Direction richting, Spelobject* obj)
{
	if (!mCurrentRoom || !obj)
		return false;

	int dx, dy;
	richtingOffset(richting, dx, dy);
	int tx = mPlayerX + dx, ty = mPlayerY + dy;

	// Check boundaries
	if (tx < 0 || ty < 0 || tx >= mCurrentRoom->getWidth() || ty >= mCurrentRoom->getHeight())
		return false;

	// Try placing in Kist or Afvalmand first
	if (legInKist(richting, obj))
		return true;
	if (legInAfvalmand(richting, obj))
		return true;

	// Normal placement on the floor
	if (mCurrentRoom->getAt(tx, ty) != nullptr)
		return false; // still occupied

	if (mCurrentRoom->isRock(tx, ty))
		return false; // cannot place on rock

	if (auto* deur = dynamic_cast<Deur*>(obj))
	{
		const int W = mCurrentRoom->getWidth();
		const int H = mCurrentRoom->getHeight();
		bool onEdge = (ty == 0 && richting == Direction::Noord) || (ty == H - 1 && richting == Direction::Zuid) ||
					  (tx == 0 && richting == Direction::West) || (tx == W - 1 && richting == Direction::Oost);

		if (!onEdge)
			return false;

		deur->setDirection(richting);
		mCurrentRoom->setAt(tx, ty, deur); // takes ownership
		return true;
	}

	// Default placement
	mCurrentRoom->setAt(tx, ty, obj); // takes ownership
	return true;
}

Spelobject* Spelwereld::pak(Direction richting)
{
	if (!mCurrentRoom)
		return nullptr;

	int dx = 0, dy = 0;
	richtingOffset(richting, dx, dy);
	const int tx = mPlayerX + dx;
	const int ty = mPlayerY + dy;

	// Bounds
	if (tx < 0 || ty < 0 || tx >= mCurrentRoom->getWidth() || ty >= mCurrentRoom->getHeight())
		return nullptr;


	Spelobject* obj = mCurrentRoom->getAt(tx, ty);
	if (!obj)
		return nullptr;

	// Dont allow picking up meubilair
	if (dynamic_cast<Meubilair*>(obj) != nullptr)
		return nullptr;

	// Dont allow picking up enemies
	if (dynamic_cast<Vijand*>(obj) != nullptr)
		return nullptr;

	// Transfer ownership to caller (Ruimte::takeAt removes without deleting)
	return mCurrentRoom->takeAt(tx, ty);
}

// --- Combat ---

int Spelwereld::aanval(int& schadeUit, SpelobjectFactory& of)
{
	if (!mCurrentRoom)
		return -1;

	CustomVector<Vijand*>& vijanden = mCurrentRoom->getVijanden();

	Vijand* nabij[4];
	int nabijCount = 0;

	const int px = mPlayerX;
	const int py = mPlayerY;

	for (std::size_t i = 0; i < vijanden.size(); ++i)
	{
		Vijand* v = vijanden[i];
		if (!v)
			continue;

		int vx = v->getX();
		int vy = v->getY();

		if ((vx == px + 1 && vy == py) || (vx == px - 1 && vy == py) || (vx == px && vy == py + 1) ||
			(vx == px && vy == py - 1))
		{
			if (nabijCount < 4)
				nabij[nabijCount++] = v;
		}
	}

	if (nabijCount == 0)
	{
		schadeUit = 0;
		return -1;
	}

	Vijand* target = nabij[RandomEngine::getRandomInt(0, nabijCount - 1)];

	int effectieveSchade = target->takeDamage(schadeUit);

	if (target->isVerslagen())
	{
		char loot = target->getLoot();
		if (loot != '\0')
		{
			Spelobject* lootObject = of.createByCode(loot);
			if (lootObject)
			{
				mCurrentRoom->setAt(target->getX(), target->getY(), lootObject);
			}
		}

		for (std::size_t i = 0; i < vijanden.size(); ++i)
		{
			if (vijanden[i] == target)
			{
				vijanden.erase(i);
				break;
			}
		}
	}

	schadeUit = effectieveSchade;
	return effectieveSchade;
}

int Spelwereld::vijandenBewegenEnAanvallen()
{
	if (!mCurrentRoom)
		return -1;
	return mCurrentRoom->enemyTurn(mPlayerX, mPlayerY);
}

int Spelwereld::getPlayerX() const { return mPlayerX; }

int Spelwereld::getPlayerY() const { return mPlayerY; }

int Spelwereld::getRoomX() const { return mRoomX; }

int Spelwereld::getRoomY() const { return mRoomY; }

void Spelwereld::setDefaultRoomSize(int width, int height)
{
	mDefaultWidth = width;
	mDefaultHeight = height;
}

void Spelwereld::destroy()
{
	// mRooms contains RoomEntry values; delete stored Ruimte* pointers and reset state
	for (std::size_t i = 0; i < mRooms.size(); ++i)
	{
		RoomEntry e = mRooms[i];
		if (e.room)
		{
			delete e.room;
			mRooms[i].room = nullptr;
		}
	}
	mRooms.clear();
	mCurrentRoom = nullptr;
	mPlayerX = mPlayerY = 0;
	mRoomX = mRoomY = 0;
}