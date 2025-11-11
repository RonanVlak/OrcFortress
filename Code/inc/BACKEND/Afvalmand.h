#ifndef AFVALMAND_H
#define AFVALMAND_H

#include "Meubilair.h"

class Afvalmand : public Meubilair
{
public:
	Afvalmand(const char* naam);
	virtual ~Afvalmand() = default;

	Spelobject* clone() const override;
	char getKaartChar() const override;

	bool acceptItem(Spelobject* item);
};

#endif // AFVALMAND_H
