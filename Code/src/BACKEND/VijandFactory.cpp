#include "VijandFactory.h"
#include "Logger.h"
#include "RandomEngine.h"
#include <cstring>
#include <sqlite3.h>

Vijand* VijandFactory::createVijand(sqlite3_stmt* stmt)
{
	try
	{
		if (!stmt)
			return nullptr;
		const char* naam = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
		int levenspunten = sqlite3_column_int(stmt, 1);
		int minSchade = sqlite3_column_int(stmt, 2);
		int maxSchade = sqlite3_column_int(stmt, 3);
		int wapenrusting = sqlite3_column_int(stmt, 4);

		// determine loot robustly: handle NULL, TEXT and INTEGER column types
		char lootChar = '\0';
		int colType = sqlite3_column_type(stmt, 5);
		if (colType == SQLITE_TEXT)
		{
			const unsigned char* lootText = sqlite3_column_text(stmt, 5);
			if (lootText && lootText[0] != '\0')
			{
				lootChar = static_cast<char>(lootText[0]);
			}
		}
		else
		{
			lootChar = '\0'; // NULL or other types => no loot
		}

		return new Vijand(naam, levenspunten, minSchade, maxSchade, wapenrusting, lootChar);
	}
	catch (const std::exception& e)
	{
		char buf[512];
		std::snprintf(buf, sizeof(buf), "VijandFactory fout: %s", e.what());
		Logger::getInstance().logError(buf);
		return nullptr;
	}
	catch (...)
	{
		Logger::getInstance().logError("Onbekende fout in VijandFactory::createVijand");
		return nullptr;
	}
}

void VijandFactory::setTemplates(CustomVector<Vijand*>& vijanden)
{
	mTemplates = &vijanden; // store reference only — no copying
}

Vijand* VijandFactory::createByName(const char* naam) const
{
	try
	{
		if (!mTemplates)
			return nullptr;
		for (std::size_t i = 0; i < mTemplates->size(); ++i)
		{
			if (std::strcmp((*mTemplates)[i]->getNaam(), naam) == 0)
			{
				return (*mTemplates)[i]->clone();
			}
		}
		return nullptr;
	}
	catch (const std::exception& e)
	{
		char buf[512];
		std::snprintf(buf, sizeof(buf), "VijandFactory fout: %s", e.what());
		Logger::getInstance().logError(buf);
		return nullptr;
	}
	catch (...)
	{
		Logger::getInstance().logError("Onbekende fout in VijandFactory::createByName");
		return nullptr;
	}
}

Vijand* VijandFactory::createRandomVijand() const
{
	try
	{
		if (!mTemplates || mTemplates->isEmpty())
			return nullptr;
		int maxIndex = static_cast<int>(mTemplates->size()) - 1;
		int idx = RandomEngine::getRandomInt(0, maxIndex);
		return (*mTemplates)[idx]->clone();
	}
	catch (const std::exception& e)
	{
		char buf[512];
		std::snprintf(buf, sizeof(buf), "VijandFactory fout: %s", e.what());
		Logger::getInstance().logError(buf);
		return nullptr;
	}
	catch (...)
	{
		Logger::getInstance().logError("Onbekende fout in VijandFactory::createRandomVijand");
		return nullptr;
	}
}
