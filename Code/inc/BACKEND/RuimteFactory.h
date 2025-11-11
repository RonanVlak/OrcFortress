#ifndef RUIMTEFACTORY_H
#define RUIMTEFACTORY_H

#include "Deur.h"
#include "Ruimte.h"

class RuimteFactory
{
public:
	RuimteFactory() = default;
	virtual ~RuimteFactory() = default;

	static Ruimte* createStartRuimte(int width, int height);
	static Ruimte* createRuimte(int width, int height);
};

#endif // RUIMTEFACTORY_H
