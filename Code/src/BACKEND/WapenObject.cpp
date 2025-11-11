#include "WapenObject.h"
#include <cstring>

WapenObject::WapenObject(const char* aNaam, int aMinimumSchade, int aMaximumSchade)
	: Voorwerp(aNaam), mMinimumSchade(aMinimumSchade), mMaximumSchade(aMaximumSchade)
{
}

int WapenObject::getMinimumSchade() const { return mMinimumSchade; }

int WapenObject::getMaximumSchade() const { return mMaximumSchade; }

Spelobject* WapenObject::clone() const
{
	WapenObject* copy = new WapenObject(mNaam, mMinimumSchade, mMaximumSchade);
	copyBeschrijvingInto(copy);
	copy->setKnutselVereisten(mKnutselGrondstof, mAantalKnutselGrondstof);
	return copy;
}