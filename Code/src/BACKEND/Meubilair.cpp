#include "Meubilair.h"

Meubilair::Meubilair(const char* naam) : Spelobject(naam) {}

bool Meubilair::isMeubilair() const { return true; }

Spelobject* Meubilair::clone() const
{
	Meubilair* copy = new Meubilair(mNaam);
	copyBeschrijvingInto(copy);
	copy->setKnutselVereisten(mKnutselGrondstof, mAantalKnutselGrondstof);
	return copy;
}

char Meubilair::getKaartChar() const
{
	return 'o'; // Generic furniture character
}
