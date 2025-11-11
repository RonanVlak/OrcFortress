#include "Deur.h"

Deur::Deur(const char* naam, Direction dir) : Meubilair(naam), mDirection(dir) {}

Spelobject* Deur::clone() const
{
	Deur* copy = new Deur(getNaam(), mDirection);
	copyBeschrijvingInto(copy);
	copy->setKnutselVereisten(mKnutselGrondstof, mAantalKnutselGrondstof);
	return copy;
}

char Deur::getKaartChar() const { return '='; }

Direction Deur::getDirection() const { return mDirection; }

void Deur::setDirection(Direction dir) { mDirection = dir; }