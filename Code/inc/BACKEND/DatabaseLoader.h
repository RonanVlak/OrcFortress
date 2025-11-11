#ifndef DATABASELOADER_H
#define DATABASELOADER_H

#include "CustomVector.h"
#include "Spelobject.h"
#include "SpelobjectFactory.h"
#include "Vijand.h"
#include "VijandFactory.h"
#include <random>
#include <sqlite3.h>

class DatabaseLoader
{
public:
	DatabaseLoader() = default;
	virtual ~DatabaseLoader() = default;
	CustomVector<Vijand*> laadVijanden(const char* databaseBestand, int& count);
	CustomVector<Spelobject*> laadSpelobjecten(const char* databaseBestand, int& count);
};

#endif // DATABASELOADER_H