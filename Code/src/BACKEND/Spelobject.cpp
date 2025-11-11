#include "Spelobject.h"
#include <cstring>
#include <utility>

Spelobject::Spelobject(const char* naam)
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

	const char* defaultDesc = "Dit is een spelobject.";
	std::size_t len = std::strlen(defaultDesc);
	mBeschrijving = new char[len + 1];
	std::strcpy(const_cast<char*>(mBeschrijving), defaultDesc);
}

Spelobject::~Spelobject()
{
	delete[] mNaam;
	delete[] mBeschrijving;
}

// Copy constructor	
Spelobject::Spelobject(const Spelobject& other)
	: mKnutselGrondstof(other.mKnutselGrondstof), mAantalKnutselGrondstof(other.mAantalKnutselGrondstof)
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
}

// Copy assignment operator
Spelobject& Spelobject::operator=(const Spelobject& other)
{
	if (this != &other)
	{
		delete[] mNaam;
		delete[] mBeschrijving;

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

		mKnutselGrondstof = other.mKnutselGrondstof;
		mAantalKnutselGrondstof = other.mAantalKnutselGrondstof;
	}
	return *this;
}

// Move constructor
Spelobject::Spelobject(Spelobject&& other) noexcept
	: mNaam(other.mNaam), mBeschrijving(other.mBeschrijving), mKnutselGrondstof(other.mKnutselGrondstof),
	  mAantalKnutselGrondstof(other.mAantalKnutselGrondstof)
{
	other.mNaam = nullptr;
	other.mBeschrijving = nullptr;
}

// Move assignment operator
Spelobject& Spelobject::operator=(Spelobject&& other) noexcept
{
	if (this != &other)
	{
		delete[] mNaam;
		delete[] mBeschrijving;

		mNaam = other.mNaam;
		mBeschrijving = other.mBeschrijving;
		mKnutselGrondstof = other.mKnutselGrondstof;
		mAantalKnutselGrondstof = other.mAantalKnutselGrondstof;

		other.mNaam = nullptr;
		other.mBeschrijving = nullptr;
	}
	return *this;
}

const char* Spelobject::getNaam() const { return mNaam; }

const char* Spelobject::getBeschrijving() const { return mBeschrijving; }

int Spelobject::getKnutselGrondstof() const { return mKnutselGrondstof; }

int Spelobject::getAantalKnutselGrondstof() const { return mAantalKnutselGrondstof; }

void Spelobject::setKnutselVereisten(int grondstofCode, int aantal)
{
	mKnutselGrondstof = grondstofCode;
	mAantalKnutselGrondstof = aantal;
}

bool Spelobject::isMeubilair() const { return false; }

bool Spelobject::isGrondstof() const { return false; }

bool Spelobject::isVoorwerp() const { return false; }

void Spelobject::copyBeschrijvingInto(Spelobject* target) const
{
	if (!target)
		return;

	// Free the target's default description (allocated by its constructor)
	delete[] target->mBeschrijving;
	target->mBeschrijving = nullptr;

	// Deep copy the current description
	if (mBeschrijving)
	{
		std::size_t len = std::strlen(mBeschrijving);
		target->mBeschrijving = new char[len + 1];
		std::strcpy(const_cast<char*>(target->mBeschrijving), mBeschrijving);
	}
}
