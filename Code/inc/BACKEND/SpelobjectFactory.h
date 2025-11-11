#ifndef SPELOBJECTFACTORY_H
#define SPELOBJECTFACTORY_H

#include "CustomVector.h"
#include "Spelobject.h"
#include <sqlite3.h>

class SpelobjectFactory
{
public:
	SpelobjectFactory() = default;
	~SpelobjectFactory() = default;

	// Create a concrete Spelobject from a prepared sqlite3_stmt (SELECT on SpelObjecten)
	static Spelobject* createSpelobject(sqlite3_stmt* stmt);

	void setTemplates(CustomVector<Spelobject*>& templates);

	Spelobject* createByName(const char* naam) const;

	Spelobject* createByCode(char code) const;

	void addDefaultGrondstoffen();

	Spelobject* createRandomGrondstof() const;

	Spelobject* findPrototypeByName(const char* naam) const;

private:
	CustomVector<Spelobject*>* mTemplates = nullptr; // Doesnt own, purely for Prototype/Clone pattern
};

#endif // SPELOBJECTFACTORY_H
