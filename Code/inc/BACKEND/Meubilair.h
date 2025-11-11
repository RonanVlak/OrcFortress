#ifndef MEUBILAIR_H
#define MEUBILAIR_H

#include "CustomVector.h"
#include "Spelobject.h"

class Meubilair : public Spelobject
{
public:
	Meubilair(const char* naam);
	virtual ~Meubilair() = default;

	bool isMeubilair() const override;
	Spelobject* clone() const override;
	char getKaartChar() const override;
};

#endif // MEUBILAIR_H
