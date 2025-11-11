#include "Vijand.h"
#include "GrondstofObject.h"
#include "RandomEngine.h"
#include <cstring>

Vijand::Vijand(const char* naam, int levenspunten, int minSchade, int maxSchade, int wapenRusting, char loot)
	: mLevenspunten(levenspunten), mMinSchade(minSchade), mMaxSchade(maxSchade), mWapenRusting(wapenRusting),
	  mLoot(loot)
{
	if (naam)
	{
		std::size_t len = std::strlen(naam);
		mNaam = new char[len + 1];
		std::strcpy(const_cast<char*>(mNaam), naam);
	}
	else
	{
		mNaam = nullptr;
	}

	// Default description since DB doesn’t provide one
	const char* defaultDesc = "Dit is een vijand.";
	std::size_t len = std::strlen(defaultDesc);
	mBeschrijving = new char[len + 1];
	std::strcpy(const_cast<char*>(mBeschrijving), defaultDesc);
}

Vijand::~Vijand()
{
	delete[] mNaam;
	delete[] mBeschrijving;
}

Vijand* Vijand::clone() const
{
	Vijand* v = new Vijand(mNaam, mLevenspunten, mMinSchade, mMaxSchade, mWapenRusting, mLoot);

	// replace default description allocated by constructor
	delete[] v->mBeschrijving;
	v->mBeschrijving = nullptr;

	// Copy description
	if (mBeschrijving)
	{
		std::size_t len = std::strlen(mBeschrijving);
		v->mBeschrijving = new char[len + 1];
		std::strcpy(const_cast<char*>(v->mBeschrijving), mBeschrijving);
	}

	// Deep copy held spelobjecten
	for (std::size_t i = 0; i < mSpelobjecten.size(); ++i)
	{
		if (mSpelobjecten[i])
			v->mSpelobjecten.push_back(mSpelobjecten[i]->clone());
	}

	return v;
}

// ----------------------------------------------------------------------
// Rule of Five
// ----------------------------------------------------------------------

// Copy constructor
Vijand::Vijand(const Vijand& other)
	: mLevenspunten(other.mLevenspunten), mMinSchade(other.mMinSchade), mMaxSchade(other.mMaxSchade),
	  mWapenRusting(other.mWapenRusting), mLoot(other.mLoot)
{
	if (other.mNaam)
	{
		std::size_t len = std::strlen(other.mNaam);
		mNaam = new char[len + 1];
		std::strcpy(const_cast<char*>(mNaam), other.mNaam);
	}
	else
	{
		mNaam = nullptr;
	}

	if (other.mBeschrijving)
	{
		std::size_t len = std::strlen(other.mBeschrijving);
		mBeschrijving = new char[len + 1];
		std::strcpy(const_cast<char*>(mBeschrijving), other.mBeschrijving);
	}
	else
	{
		mBeschrijving = nullptr;
	}

	// Deep copy spelobjects
	for (auto& obj : other.mSpelobjecten)
	{
		if (obj)
			mSpelobjecten.push_back(obj->clone());
	}
}

// Copy assignment operator
Vijand& Vijand::operator=(const Vijand& other)
{
	if (this == &other)
		return *this;

	delete[] mNaam;
	delete[] mBeschrijving;
	for (auto& obj : mSpelobjecten)
		delete obj;
	mSpelobjecten.clear();

	if (other.mNaam)
	{
		std::size_t len = std::strlen(other.mNaam);
		mNaam = new char[len + 1];
		std::strcpy(const_cast<char*>(mNaam), other.mNaam);
	}
	else
	{
		mNaam = nullptr;
	}

	if (other.mBeschrijving)
	{
		std::size_t len = std::strlen(other.mBeschrijving);
		mBeschrijving = new char[len + 1];
		std::strcpy(const_cast<char*>(mBeschrijving), other.mBeschrijving);
	}
	else
	{
		mBeschrijving = nullptr;
	}

	mLevenspunten = other.mLevenspunten;
	mMinSchade = other.mMinSchade;
	mMaxSchade = other.mMaxSchade;
	mWapenRusting = other.mWapenRusting;
	mLoot = other.mLoot;

	for (auto& obj : other.mSpelobjecten)
	{
		if (obj)
			mSpelobjecten.push_back(obj->clone());
	}

	return *this;
}

// Move constructor
Vijand::Vijand(Vijand&& other) noexcept
	: mNaam(other.mNaam), mBeschrijving(other.mBeschrijving), mLevenspunten(other.mLevenspunten),
	  mMinSchade(other.mMinSchade), mMaxSchade(other.mMaxSchade), mWapenRusting(other.mWapenRusting),
	  mLoot(other.mLoot), mSpelobjecten(std::move(other.mSpelobjecten))
{
	other.mNaam = nullptr;
	other.mBeschrijving = nullptr;
	other.mLoot = '\0';
	other.mLevenspunten = 0;
	other.mMinSchade = 0;
	other.mMaxSchade = 0;
	other.mWapenRusting = 0;
}

// Move assignment operator
Vijand& Vijand::operator=(Vijand&& other) noexcept
{
	if (this == &other)
		return *this;

	delete[] mNaam;
	delete[] mBeschrijving;
	for (auto& obj : mSpelobjecten)
		delete obj;
	mSpelobjecten.clear();

	mNaam = other.mNaam;
	mBeschrijving = other.mBeschrijving;
	mLevenspunten = other.mLevenspunten;
	mMinSchade = other.mMinSchade;
	mMaxSchade = other.mMaxSchade;
	mWapenRusting = other.mWapenRusting;
	mLoot = other.mLoot;
	mSpelobjecten = std::move(other.mSpelobjecten);

	other.mNaam = nullptr;
	other.mBeschrijving = nullptr;
	other.mLoot = '\0';
	other.mLevenspunten = 0;
	other.mMinSchade = 0;
	other.mMaxSchade = 0;
	other.mWapenRusting = 0;

	return *this;
}



int Vijand::getLevenspunten() const { return mLevenspunten; }

void Vijand::setLevenspunten(int hp) { mLevenspunten = hp; }

int Vijand::takeDamage(int damage)
{
	int effective = damage - mWapenRusting;
	if (effective < 0)
		effective = 0;
	mLevenspunten -= effective;
	if (mLevenspunten < 0)
		mLevenspunten = 0;
	return effective;
}

bool Vijand::isVerslagen() const { return mLevenspunten <= 0; }

int Vijand::getMinSchade() const { return mMinSchade; }

int Vijand::getMaxSchade() const { return mMaxSchade; }

int Vijand::getBescherming() const { return mWapenRusting; }

const char* Vijand::getNaam() const { return mNaam; }

const char* Vijand::getBeschrijving() const { return mBeschrijving; }

char Vijand::getLoot() const { return mLoot; }

void Vijand::voegSpelobjectToe(Spelobject* object)
{
	if (object)
		mSpelobjecten.push_back(object);
}

CustomVector<Spelobject*>& Vijand::getSpelobjecten() { return mSpelobjecten; }

const CustomVector<Spelobject*>& Vijand::getSpelobjecten() const { return mSpelobjecten; }

int Vijand::attack() const { return RandomEngine::getRandomInt(mMinSchade, mMaxSchade); }

void Vijand::move(int newX, int newY)
{
	mX = newX;
	mY = newY;
}

int Vijand::getX() const { return mX; }

int Vijand::getY() const { return mY; }

void Vijand::setPos(int x, int y)
{
	mX = x;
	mY = y;
}

int Vijand::attackRoll() const { return RandomEngine::getRandomInt(mMinSchade, mMaxSchade); }