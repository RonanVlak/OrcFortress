#include "DatabaseLoader.h"
#include "Logger.h"
#include "RandomEngine.h"
#include "RuimteFactory.h"
#include <iostream>
#include <stdexcept>

// RAII wrapper for sqlite3 database connection
class SQLiteDB
{
public:
	SQLiteDB(const char* filename)
	{
		if (sqlite3_open(filename, &db) != SQLITE_OK)
		{
			throw std::runtime_error("Could not open database");
		}
	}

	~SQLiteDB()
	{
		if (db)
		{
			sqlite3_close(db);
		}
	}

	sqlite3* get() const { return db; }

private:
	sqlite3* db = nullptr;

	SQLiteDB(const SQLiteDB&)
	{
		// Empty, do not copy
	}

	// Copy assignment, do not copy
	SQLiteDB& operator=(const SQLiteDB& other)
	{
		// Self-assignment guard, do nothing
		if (this != &other)
		{ 
			/* nothing */
		}
		return *this;
	}

	SQLiteDB(SQLiteDB&&) noexcept
	{
		// Empty, do not move
	}

	// Move assignment, do not move
	SQLiteDB& operator=(SQLiteDB&&) noexcept { return *this; }
};

// RAII wrapper for sqlite3 statement
class SQLiteStmt
{
public:
	SQLiteStmt(sqlite3* db, const char* sql)
	{
		if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK)
		{
			throw std::runtime_error(std::string("Failed to prepare statement: ") + sqlite3_errmsg(db));
		}
	}

	~SQLiteStmt()
	{
		if (stmt)
		{
			sqlite3_finalize(stmt);
		}
	}

	sqlite3_stmt* get() const { return stmt; }

private:
	sqlite3_stmt* stmt = nullptr;

	SQLiteStmt(const SQLiteStmt&)
	{
		// Empty, do not copy
	}

	// Copy assignment, do not copy
	SQLiteStmt& operator=(const SQLiteStmt& other)
	{
		// Self-assignment guard, do nothing
		if (this != &other)
		{ 
			/* nothing */
		}
		return *this;
	}

	SQLiteStmt(SQLiteStmt&&) noexcept
	{
		// Empty, do not move
	}

	// Move assignment, do not move
	SQLiteStmt& operator=(SQLiteStmt&&) noexcept { return *this; }
};

CustomVector<Vijand*> DatabaseLoader::laadVijanden(const char* databaseBestand, int& count)
{
	CustomVector<Vijand*> vijandenVector;

	try
	{
		SQLiteDB db(databaseBestand);
		const char* sql = "SELECT * FROM Vijanden";
		SQLiteStmt stmt(db.get(), sql);

		VijandFactory factory;

		while (sqlite3_step(stmt.get()) == SQLITE_ROW)
		{
			Vijand* vijand = factory.createVijand(stmt.get());
			vijandenVector.push_back(vijand);
		}

		count = vijandenVector.size();
	}
	catch (const std::exception& e)
	{
		std::cerr << e.what() << std::endl;
		count = 0;
	}

	return vijandenVector;
}

CustomVector<Spelobject*> DatabaseLoader::laadSpelobjecten(const char* databaseBestand, int& count)
{
	CustomVector<Spelobject*> spelobjectVector;

	try
	{
		SQLiteDB db(databaseBestand);
		const char* sql = "SELECT * FROM SpelObjecten";
		SQLiteStmt stmt(db.get(), sql);

		SpelobjectFactory factory;

		while (sqlite3_step(stmt.get()) == SQLITE_ROW)
		{
			Spelobject* object = factory.createSpelobject(stmt.get());
			if (object)
			{
				spelobjectVector.push_back(object);
			}
		}

		count = spelobjectVector.size();
	}
	catch (const std::exception& e)
	{
		std::cerr << e.what() << std::endl;
		count = 0;
	}

	return spelobjectVector;
}
