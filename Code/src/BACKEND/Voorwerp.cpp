#include "Voorwerp.h"

Voorwerp::Voorwerp(const char* naam) : Spelobject(naam) {}

bool Voorwerp::isVoorwerp() const { return true; }

Spelobject* Voorwerp::clone() const
{
	Voorwerp* copy = new Voorwerp(mNaam);
	copyBeschrijvingInto(copy);
	copy->setKnutselVereisten(mKnutselGrondstof, mAantalKnutselGrondstof);
	return copy;
}

char Voorwerp::getKaartChar() const
{
	return '*'; // Generic voorwerp character
}