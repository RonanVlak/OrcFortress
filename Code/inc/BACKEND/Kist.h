#ifndef KIST_H
#define KIST_H

#include "Meubilair.h"

class Kist : public Meubilair
{
public:
	Kist(const char* naam);
	virtual ~Kist() = default;

	Spelobject* clone() const override;
	char getKaartChar() const override;

	void addItem(Spelobject* item);
	CustomVector<Spelobject*>& getItems();
	std::size_t getItemCount() const;
    const Spelobject* getItemAt(std::size_t index) const;
	Spelobject* takeItem(int index = 0);

private:
	CustomVector<Spelobject*> mItems;
};

#endif // KIST_H
