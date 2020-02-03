#pragma once
#include <memory>

class Printer {
    struct PImpl;
    PImpl *pimpl;
    const unsigned int NoOfPlayers;
    unsigned int NoOfCards;
    std::unique_ptr<bool*> written;
    std::unique_ptr<int*> taken;
    std::unique_ptr<int*> remainingCards;
    std::unique_ptr<int*> remainingPlayers;
    enum
    {
        DEATH_DECK_DIVISOR = 7
    };
    void clearBuffer();

  public:
    Printer( const unsigned int NoOfPlayers, const unsigned int NoOfCards );
    ~Printer();
    void prt( unsigned int id, int took, int RemainingPlayers );
}; // Printer

// Local Variables: //
// mode: c++ //
// compile-command: "make cardgame" //
// End: //
