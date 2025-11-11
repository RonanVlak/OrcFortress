#include "Goudklomp.h"
#include <cstring>

Goudklomp::Goudklomp(const char* naam) : Voorwerp(naam) {}

Spelobject* Goudklomp::clone() const
{
	Goudklomp* copy = new Goudklomp(getNaam());
	copyBeschrijvingInto(copy);
	copy->setKnutselVereisten(mKnutselGrondstof, mAantalKnutselGrondstof);
	return copy;
}
