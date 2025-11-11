#ifndef VIJAND_H
#define VIJAND_H

#include "CustomVector.h"
#include "Spelobject.h"
#include <cstddef>

class Vijand
{
public:
	// loot is a single char code per DB (or '\0' when none)
	Vijand(const char* naam, int levenspunten, int minSchade, int maxSchade, int wapenRusting, char loot);
	~Vijand();

	Vijand(const Vijand& other);
	Vijand& operator=(const Vijand& other);
	Vijand(Vijand&& other) noexcept;
	Vijand& operator=(Vijand&& other) noexcept;

	Vijand* clone() const;

	int getLevenspunten() const;
	void setLevenspunten(int hp);
	int takeDamage(int damage);
	bool isVerslagen() const;
	int attack() const;

	void move(int newX, int newY);
	void setPos(int x, int y);
	int getX() const;
	int getY() const;

	int getMinSchade() const;
	int getMaxSchade() const;

	int getBescherming() const;
	char getLoot() const;
	const char* getNaam() const;
	const char* getBeschrijving() const;

	int attackRoll() const; // random between min and max

	void voegSpelobjectToe(Spelobject* object);
	CustomVector<Spelobject*>& getSpelobjecten();
	const CustomVector<Spelobject*>& getSpelobjecten() const; // const-correct overload

private:
	const char* mNaam{nullptr};
	const char* mBeschrijving{nullptr};
	int mLevenspunten{0};
	int mMinSchade{0};
	int mMaxSchade{0};
	int mWapenRusting{0};
	char mLoot{'\0'}; // single char loot code
	int mX{0};		  // grid position
	int mY{0};

	CustomVector<Spelobject*> mSpelobjecten;
};

#endif // VIJAND_H