#ifndef SPELOBJECT_H
#define SPELOBJECT_H

class Spelobject
{
public:
	Spelobject() = default;
	Spelobject(const char* naam);
	virtual ~Spelobject();

	Spelobject(const Spelobject& other);
	Spelobject& operator=(const Spelobject& other);
	Spelobject(Spelobject&& other) noexcept;
	Spelobject& operator=(Spelobject&& other) noexcept;

	virtual Spelobject* clone() const = 0;
	virtual char getKaartChar() const = 0;

	const char* getNaam() const;
	const char* getBeschrijving() const;
	int getKnutselGrondstof() const;
	int getAantalKnutselGrondstof() const;
	void setKnutselVereisten(int grondstofCode, int aantal);
	virtual bool isMeubilair() const;
	virtual bool isGrondstof() const;
	virtual bool isVoorwerp() const;

protected:
	void copyBeschrijvingInto(Spelobject* target) const;
	const char* mNaam{nullptr};
	const char* mBeschrijving{nullptr};
	char mKnutselGrondstof = '\0';
	int mAantalKnutselGrondstof{0};
};

#endif // SPELOBJECT_H
