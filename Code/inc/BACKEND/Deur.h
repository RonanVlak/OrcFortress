#ifndef DEUR_H
#define DEUR_H

#include "Direction.h"
#include "Meubilair.h"

class Deur : public Meubilair
{
public:
	Deur(const char* naam, Direction dir);
	virtual ~Deur() = default;

	Spelobject* clone() const override;
	char getKaartChar() const override;

	Direction getDirection() const;
	void setDirection(Direction dir);

private:
	Direction mDirection;
};

#endif // DEUR_H
