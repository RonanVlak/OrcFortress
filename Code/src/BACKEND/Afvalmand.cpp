#include "Afvalmand.h"
#include <cstring>

Afvalmand::Afvalmand(const char* naam) : Meubilair(naam) {}

Spelobject* Afvalmand::clone() const
{
	Afvalmand* copy = new Afvalmand(getNaam());
	copyBeschrijvingInto(copy);
	copy->setKnutselVereisten(mKnutselGrondstof, mAantalKnutselGrondstof);
	return copy;
}

char Afvalmand::getKaartChar() const { return '#'; }

bool Afvalmand::acceptItem(Spelobject* item)
{
	if (!item)
		return false;

	if (item->isGrondstof() && std::strcmp(item->getNaam(), "Rotzooi") == 0)
	{
		// Delete the item and accept it
		delete item;
		return true;
	}
	return false;
}
