#ifndef GRONDSTOFOBJECT_H
#define GRONDSTOFOBJECT_H

#include "Spelobject.h"
#include "Voorwerp.h"

class GrondstofObject : public Voorwerp
{
public:
	GrondstofObject(const char* naam, char code);
	virtual ~GrondstofObject() = default;

	Spelobject* clone() const override;
	char getKaartChar() const override;
	bool isGrondstof() const override;

	char getCode() const;

private:
	char mCode; // I, G, L, R, H, M
};

#endif // GRONDSTOFOBJECT_H
