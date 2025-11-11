#ifndef RUIMTE_H
#define RUIMTE_H

#include "../BACKEND/CustomVector.h"
#include "../BACKEND/Direction.h"
#include "../BACKEND/Spelobject.h"
#include "../BACKEND/Vijand.h"
#include <string>

class Ruimte
{
public:
	Ruimte(int width, int height);
	~Ruimte();

	// Rule of Five
	Ruimte(const Ruimte& other);
	Ruimte(Ruimte&& other) noexcept;
	Ruimte& operator=(const Ruimte& other);
	Ruimte& operator=(Ruimte&& other) noexcept;
	void swap(Ruimte& other) noexcept;

	int getWidth() const;
	int getHeight() const;

	// --- Terrain (tiles) ---
	// '.' = floor, '0' = rock
	char getTile(int x, int y) const;
	void setTile(int x, int y, char c);
	bool isRock(int x, int y) const;
	bool isWalkable(int x, int y) const; // tile not rock AND (no object OR door)

	// --- Objects per cell ---
	Spelobject* getAt(int x, int y) const;
	void setAt(int x, int y, Spelobject* obj);
	bool isEmpty(int x, int y) const;
	Spelobject* takeAt(int x, int y); // transfer ownership to caller

	// --- Doors ---
	bool isDoor(int x, int y) const;
	Direction getDoorDirection(int x, int y) const;

	// --- Enemies ---
	const CustomVector<Vijand*>& getVijanden() const;
	void plaatsVijand(int x, int y, Vijand* v);
	CustomVector<Vijand*>& getVijanden();
	int enemyTurn(int playerX, int playerY);

	// --- Utility ---
	void clearFiveAround(int cx, int cy);
	bool isInside(int x, int y) const;

private:
	int mWidth = 0;
	int mHeight = 0;
	Spelobject** mGrid{nullptr}; // width*height, owns pointers in cells
	char* mTiles{nullptr};		 // size width*height, object or nullptr
	CustomVector<Vijand*> mVijanden;
};

#endif // RUIMTE_H
