#ifndef GOUDKLOMP_H
#define GOUDKLOMP_H

#include "Spelobject.h"
#include "Voorwerp.h"

class Goudklomp : public Voorwerp
{
public:
	Goudklomp(const char* naam);
	virtual ~Goudklomp() = default;

	// Cloning for database + inventory duplication
	Spelobject* clone() const override;
};

#endif // GOUDKLOMP_H
