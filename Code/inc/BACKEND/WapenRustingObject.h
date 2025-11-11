#ifndef WAPENRUSTINGOBJECT_H
#define WAPENRUSTINGOBJECT_H

#include "Spelobject.h"
#include "Voorwerp.h"

class WapenRustingObject : public Voorwerp
{
public:
	WapenRustingObject(const char* naam, int bescherming);
	virtual ~WapenRustingObject() = default;

	Spelobject* clone() const override;

	int getBescherming() const;

private:
	int mBescherming;
};

#endif // WAPENRUSTINGOBJECT_H
