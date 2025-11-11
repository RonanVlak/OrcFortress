#include "Ruimte.h"
#include "Deur.h"
#include "Direction.h"
#include <cstring>
#include <utility>

Ruimte::Ruimte(int width, int height) : mWidth(width), mHeight(height), mTiles(nullptr), mGrid(nullptr), mVijanden()
{
	const int size = mWidth * mHeight;

	// allocate both buffers first
	mTiles = new char[size];
	mGrid = new Spelobject*[size];

	// init terrain to floor ('.')
	for (int i = 0; i < size; ++i)
		mTiles[i] = '.';

	// init objects grid to nullptr
	for (int i = 0; i < size; ++i)
		mGrid[i] = nullptr;
}

Ruimte::~Ruimte()
{
	if (mGrid)
	{
		int size = mWidth * mHeight;
		for (int i = 0; i < size; ++i)
		{
			delete mGrid[i];
		}
		delete[] mGrid;
		mGrid = nullptr;
	}
	delete[] mTiles;
	mTiles = nullptr;
}

static Spelobject** duplicateGrid(const Spelobject* const* src, int width, int height)
{
	if (!src)
		return nullptr;
	int size = width * height;
	Spelobject** newGrid = new Spelobject*[size];
	for (int i = 0; i < size; ++i)
		newGrid[i] = nullptr;
	try
	{
		for (int i = 0; i < size; ++i)
		{
			if (src[i])
				newGrid[i] = src[i]->clone();
		}
	}
	catch (...)
	{
		for (int j = 0; j < size; ++j)
			delete newGrid[j];
		delete[] newGrid;
		throw;
	}
	return newGrid;
}

Ruimte::Ruimte(const Ruimte& other)
	: mWidth(other.mWidth), mHeight(other.mHeight), mTiles(nullptr), mGrid(nullptr), mVijanden(other.mVijanden)
{
	if (other.mTiles)
	{
		mTiles = new char[mWidth * mHeight];
		std::memcpy(mTiles, other.mTiles, mWidth * mHeight);
	}
	mGrid = duplicateGrid(other.mGrid, mWidth, mHeight);
}

Ruimte::Ruimte(Ruimte&& other) noexcept
	: mWidth(other.mWidth), mHeight(other.mHeight), mTiles(other.mTiles), mGrid(other.mGrid),
	  mVijanden(std::move(other.mVijanden))
{
	other.mTiles = nullptr;
	other.mGrid = nullptr;
	other.mWidth = 0;
	other.mHeight = 0;
}

Ruimte& Ruimte::operator=(const Ruimte& other)
{
	if (this == &other)
		return *this;
	Ruimte tmp(other);
	swap(tmp);
	return *this;
}

Ruimte& Ruimte::operator=(Ruimte&& other) noexcept
{
	if (this == &other)
		return *this;

	if (mGrid)
	{
		int oldSize = mWidth * mHeight;
		for (int i = 0; i < oldSize; ++i)
			delete mGrid[i];
		delete[] mGrid;
	}
	delete[] mTiles;

	mWidth = other.mWidth;
	mHeight = other.mHeight;
	mTiles = other.mTiles;
	mGrid = other.mGrid;
	mVijanden = std::move(other.mVijanden);

	other.mTiles = nullptr;
	other.mGrid = nullptr;
	other.mWidth = 0;
	other.mHeight = 0;
	return *this;
}

void Ruimte::swap(Ruimte& other) noexcept
{
	std::swap(mWidth, other.mWidth);
	std::swap(mHeight, other.mHeight);
	std::swap(mTiles, other.mTiles);
	std::swap(mGrid, other.mGrid);
	std::swap(mVijanden, other.mVijanden);
}

int Ruimte::getWidth() const { return mWidth; }

int Ruimte::getHeight() const { return mHeight; }

// ---------- Terrain ----------
char Ruimte::getTile(int x, int y) const
{
	if (x < 0 || x >= mWidth || y < 0 || y >= mHeight)
		return '.';
	return mTiles[y * mWidth + x];
}

void Ruimte::setTile(int x, int y, char c)
{
	if (x < 0 || x >= mWidth || y < 0 || y >= mHeight)
		return;
	mTiles[y * mWidth + x] = c;
}

bool Ruimte::isRock(int x, int y) const
{
	if (x < 0 || y < 0 || x >= mWidth || y >= mHeight)
		return false;
	return mTiles[y * mWidth + x] == '0';
}

bool Ruimte::isWalkable(int x, int y) const
{
	if (x < 0 || y < 0 || x >= mWidth || y >= mHeight)
		return false;

	// 1) Rock or wall blocks movement
	if (mTiles[y * mWidth + x] == '0')
		return false;

	// 2) Objects: door is OK, everything else blocks
	Spelobject* obj = mGrid[y * mWidth + x];
	if (obj && dynamic_cast<Deur*>(obj) == nullptr)
		return false;

	// 3) Enemies: cannot walk onto them
	const CustomVector<Vijand*>& vijanden = mVijanden;
	for (std::size_t i = 0; i < vijanden.size(); ++i)
	{
		const Vijand* v = vijanden[i];
		if (v && v->getX() == x && v->getY() == y)
			return false;
	}

	// If we reach here, the tile is free to walk onto
	return true;
}

// ---------- Objects ----------
Spelobject* Ruimte::getAt(int x, int y) const
{
	if (x < 0 || x >= mWidth || y < 0 || y >= mHeight)
		return nullptr;
	return mGrid[y * mWidth + x];
}

void Ruimte::setAt(int x, int y, Spelobject* obj)
{
	if (x < 0 || x >= mWidth || y < 0 || y >= mHeight)
		return;
	int idx = y * mWidth + x;
	delete mGrid[idx];
	mGrid[idx] = obj;
}

Spelobject* Ruimte::takeAt(int x, int y)
{
	if (x < 0 || x >= mWidth || y < 0 || y >= mHeight)
		return nullptr;
	int idx = y * mWidth + x;
	Spelobject* obj = mGrid[idx];
	mGrid[idx] = nullptr;
	return obj;
}

bool Ruimte::isEmpty(int x, int y) const { return getAt(x, y) == nullptr; }

// ---------- Doors ----------
bool Ruimte::isDoor(int x, int y) const { return dynamic_cast<Deur*>(getAt(x, y)) != nullptr; }

Direction Ruimte::getDoorDirection(int x, int y) const
{
	Deur* d = dynamic_cast<Deur*>(getAt(x, y));
	// caller should check isDoor first
	return d ? d->getDirection() : Direction::Noord;
}

// ---------- Enemies ----------
CustomVector<Vijand*>& Ruimte::getVijanden() { return mVijanden; }

const CustomVector<Vijand*>& Ruimte::getVijanden() const { return mVijanden; }

void Ruimte::plaatsVijand(int x, int y, Vijand* v)
{
	if (!v)
		return;
	v->setPos(x, y);
	mVijanden.push_back(v);
}

int Ruimte::enemyTurn(int playerX, int playerY)
{
	int totalDamage = -1;

	// Copy to avoid interference while iterating
	for (std::size_t i = 0; i < mVijanden.size(); ++i)
	{
		Vijand* v = mVijanden[i];
		if (!v)
			continue;

		int vx = v->getX();
		int vy = v->getY();

		// Check adjacency (4 directions)
		const bool adjacent = (std::abs(vx - playerX) + std::abs(vy - playerY)) == 1;

		if (adjacent)
		{
			// Attack the player
			int damage = v->attack();
			totalDamage += damage;
			continue; // enemy stays in place after attack
		}

		// Movement towards player
		// --- Compute distance and direction ---
		int dx = playerX - vx;
		int dy = playerY - vy;
		int stepX = 0, stepY = 0;

		// Choose major axis first (no diagonal)
		if (std::abs(dx) > std::abs(dy))
		{
			stepX = (dx > 0) ? 1 : -1;
		}
		else if (dy != 0)
		{
			stepY = (dy > 0) ? 1 : -1;
		}

		int newX = vx + stepX;
		int newY = vy + stepY;

		// --- Prevent moving onto player ---
		if (newX == playerX && newY == playerY)
		{
			continue; // stay in place instead
		}

		// --- Fallback if blocked ---
		if (!isInside(newX, newY) || !isWalkable(newX, newY))
		{
			if (stepX != 0 && dy != 0)
			{
				newX = vx;
				newY = vy + ((dy > 0) ? 1 : -1);
			}
			else if (stepY != 0 && dx != 0)
			{
				newX = vx + ((dx > 0) ? 1 : -1);
				newY = vy;
			}

			// Still prevent stepping on player
			if (newX == playerX && newY == playerY)
			{
				continue;
			}
		}

		// --- Move if valid ---
		if (isInside(newX, newY) && isWalkable(newX, newY) && !(isDoor(newX, newY)))
		{
			v->move(newX, newY);
		}
	}

	return totalDamage;
}

// ---------- Utility ----------
void Ruimte::clearFiveAround(int cx, int cy)
{
	int cleared = 0;
	for (int r = 0; r <= 2 && cleared < 5; ++r)
	{
		for (int dy = -r; dy <= r && cleared < 5; ++dy)
		{
			for (int dx = -r; dx <= r && cleared < 5; ++dx)
			{
				int x = cx + dx, y = cy + dy;
				if (x < 0 || y < 0 || x >= mWidth || y >= mHeight)
					continue;

				// Only clear rock tiles
				if (isRock(x, y))
				{
					setTile(x, y, '.'); // Clear the rock tile
					++cleared;
				}
			}
		}
	}
}

bool Ruimte::isInside(int x, int y) const { return x >= 0 && y >= 0 && x < getWidth() && y < getHeight(); }
