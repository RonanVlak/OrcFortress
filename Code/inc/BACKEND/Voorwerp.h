#ifndef VOORWERP_H
#define VOORWERP_H

#include "CustomVector.h"
#include "Spelobject.h"

class Voorwerp : public Spelobject
{
public:
	Voorwerp(const char* naam);
	virtual ~Voorwerp() = default;

	bool isVoorwerp() const override;
	Spelobject* clone() const override;
	char getKaartChar() const override;
};

#endif // VOORWERP_H