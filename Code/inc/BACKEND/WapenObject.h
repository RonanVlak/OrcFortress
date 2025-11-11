#ifndef WAPENOBJECT_H
#define WAPENOBJECT_H

#include "Spelobject.h"
#include "Voorwerp.h"

class WapenObject : public Voorwerp
{
public:
	WapenObject(const char* aNaam, int aMinimumSchade, int aMaximumSchade);
	virtual ~WapenObject() = default;

public:
	int getMinimumSchade() const;
	int getMaximumSchade() const;
	Spelobject* clone() const override;

private:
	int mMinimumSchade;
	int mMaximumSchade;
};

#endif // WAPENOBJECT_H