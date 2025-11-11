#ifndef VIJANDFACTORY_H
#define VIJANDFACTORY_H

#include "Vijand.h"
#include <sqlite3.h>

class VijandFactory
{
public:
	VijandFactory() = default;
	virtual ~VijandFactory() = default;

	// Create a Vijand* from a prepared stmt (SELECT on Vijanden)
	static Vijand* createVijand(sqlite3_stmt* stmt);

	// Register templates (non-owning)
	void setTemplates(CustomVector<Vijand*>& vijanden);

	// Create clones from templates
	Vijand* createByName(const char* naam) const;
	Vijand* createRandomVijand() const;

private:
	CustomVector<Vijand*>* mTemplates = nullptr; // Doesnt own, purely for prototype/clone pattern
};

#endif // VIJANDFACTORY_H
