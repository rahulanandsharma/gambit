//
// FILE: nfg.h  -- Declaration of normal form representation
//
// $Id$
//

#ifndef NFG_H
#define NFG_H

#include "garray.h"
#include "gtext.h"
#include "grblock.h"
#include "gnumber.h"

class NFOutcome;


#include "gblock.h"

class NFPlayer;
class Strategy;
class StrategyProfile;

#ifndef NFG_ONLY
class Lexicon;
class Efg;
#endif   // NFG_ONLY

class Nfg  {
#ifndef NFG_ONLY
friend class Lexicon;
friend void SetEfg(Nfg *, const Efg *);
#endif   // NFG_ONLY
protected:
  gText title, comment;
  gArray<int> dimensions;

  gArray<NFPlayer *> players;
  gBlock<NFOutcome *> outcomes;

  gArray<NFOutcome *> results;

#ifndef NFG_ONLY
  const Efg *efg;
#endif  // NFG_ONLY

  // PRIVATE AUXILIARY MEMBER FUNCTIONS
  void IndexStrategies(void);
  int Product(const gArray<int> &);

  void BreakLink(void);


public:
  // CONSTRUCTORS, DESTRUCTORS, CONSTRUCTIVE OPERATORS
  Nfg(const gArray<int> &dim);
  Nfg(const Nfg &b);
  ~Nfg();
    
  // GENERAL DATA ACCESS AND MANIPULATION  
  void SetTitle(const gText &s);
  const gText &GetTitle(void) const;

  void SetComment(const gText &);
  const gText &GetComment(void) const;

  void WriteNfgFile(gOutput &) const;

  // PLAYERS AND STRATEGIES
  int NumPlayers(void) const;
  const gArray<NFPlayer *> &Players(void) const;

  int NumStrats(int pl) const;
  const gArray<int> &NumStrats(void) const  { return dimensions; }
  const gArray<Strategy *> &Strategies(int p) const;

  int ProfileLength(void) const;

  // OUTCOMES
  NFOutcome *NewOutcome(void);
  void DeleteOutcome(NFOutcome *);

  const gArray<NFOutcome *> &Outcomes(void) const  { return outcomes; }
  int NumOutcomes(void) const   { return outcomes.Length(); }

  void SetOutcome(const gArray<int> &profile, NFOutcome *outcome);
  NFOutcome *GetOutcome(const gArray<int> &profile) const;
  void SetOutcome(const StrategyProfile &p, NFOutcome *outcome);
  NFOutcome *GetOutcome(const StrategyProfile &p) const;

  void SetOutcome(int index, NFOutcome *outcome)  { results[index] = outcome; }
  NFOutcome *GetOutcome(int index) const   { return results[index]; }

  void SetPayoff(NFOutcome *, int pl, const gNumber &value);
  gNumber Payoff(NFOutcome *, int pl) const;

    // defined in nfgutils.cc
  friend void RandomNfg(Nfg &);
  friend bool IsConstSum(const Nfg &);
  friend gNumber MinPayoff(const Nfg &, int pl = 0);
  friend gNumber MaxPayoff(const Nfg &, int pl = 0);

#ifndef NFG_ONLY
  const Efg *AssociatedEfg(void) const   { return efg; }
#endif   // NFG_ONLY
};

int ReadNfgFile(gInput &, Nfg *&);

class NFOutcome   {
  friend class Nfg;
  private:
    int number;
    Nfg *nfg;
    gText name;
    gArray<gNumber> payoffs;

    NFOutcome(int n, Nfg *N)
      : number(n), nfg(N), payoffs(N->NumPlayers())  { }
    NFOutcome(int n, const NFOutcome &c)
      : number(n), nfg(c.nfg),name(c.name), payoffs(c.payoffs) { }
    ~NFOutcome() { }

  public:
    int GetNumber(void) const    { return number; }

    const gText &GetName(void) const   { return name; }
    void SetName(const gText &s)   { name = s; }

    Nfg *Game(void) const   { return nfg; }
};

#endif    // NFG_H
