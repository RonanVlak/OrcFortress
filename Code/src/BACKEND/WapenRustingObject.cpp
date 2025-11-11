#include "WapenRustingObject.h"

WapenRustingObject::WapenRustingObject(const char* naam, int bescherming) : Voorwerp(naam), mBescherming(bescherming) {}

Spelobject* WapenRustingObject::clone() const
{
	WapenRustingObject* copy = new WapenRustingObject(getNaam(), mBescherming);
	copyBeschrijvingInto(copy);
	copy->setKnutselVereisten(mKnutselGrondstof, mAantalKnutselGrondstof);
	return copy;
}

int WapenRustingObject::getBescherming() const { return mBescherming; }
