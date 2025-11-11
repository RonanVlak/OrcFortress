#include "GrondstofObject.h"

GrondstofObject::GrondstofObject(const char* naam, char code) : Voorwerp(naam), mCode(code) {}

Spelobject* GrondstofObject::clone() const
{
	GrondstofObject* copy = new GrondstofObject(getNaam(), mCode);
	copyBeschrijvingInto(copy);
	copy->setKnutselVereisten(mKnutselGrondstof, mAantalKnutselGrondstof);
	return copy;
}

char GrondstofObject::getKaartChar() const { return mCode; }

char GrondstofObject::getCode() const { return mCode; }

bool GrondstofObject::isGrondstof() const { return true; }
