//
// $Source$
// $Date$
// $Revision$
//
// DESCRIPTION:
// Interface to player representation classes
//
// This file is part of Gambit
// Copyright (c) 2002, The Gambit Project
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
//

#ifndef GAME_PLAYER_H
#define GAME_PLAYER_H

#include "game.h"

class gbtGameInfosetRep;
typedef gbtGameObjectHandle<gbtGameInfosetRep> gbtGameInfoset;

class gbtGameStrategyRep;
typedef gbtGameObjectHandle<gbtGameStrategyRep> gbtGameStrategy;

class gbtGamePlayerRep : public gbtGameObject {
  friend class gbtGameObjectHandle<gbtGamePlayerRep>;

public:
  //!
  //! @name General information about the player
  //!
  //@{
  /// Returns the ID number of the player (unique within a game)
  virtual int GetId(void) const = 0;
  /// Returns true if the player represents the chance or nature player
  virtual bool IsChance(void) const = 0;
  /// Set the text label associated with the player
  virtual void SetLabel(const std::string &) = 0;
  /// Get the text label associated with the player
  virtual std::string GetLabel(void) const = 0;

  /// Returns true if the player has been deleted from its game
  virtual bool IsDeleted(void) const = 0;
  //@}

  //!
  //! @name Accessing information sets of the player
  //!
  //@{
  /// Returns the number of information sets at which the player moves
  virtual int NumInfosets(void) const = 0;
  /// Gets the p_index'th information set belonging to the player
  virtual gbtGameInfoset GetInfoset(int p_index) const = 0;
  /// Creates a new information set with p_actions actions for the player.
  virtual gbtGameInfoset NewInfoset(int p_actions) = 0;
  //@}

  //!
  //! @name Accessing the sequences of the player
  //!
  //@{
  /// Returns the number of sequences generated by a player's actions
  virtual int NumSequences(void) const = 0;
  //@}

  //!
  //! @name Accessing the strategies of the player
  //!
  //@{
  /// Returns the number of strategies the player has
  virtual int NumStrategies(void) const = 0;
  /// Returns the ith strategy belonging to the player
  virtual gbtGameStrategy GetStrategy(int) const = 0;
  //@}
};

typedef gbtGameObjectHandle<gbtGamePlayerRep> gbtGamePlayer;

class gbtGamePlayerIterator {
private:
  int m_index;
  gbtGame m_efg;

public:
  gbtGamePlayerIterator(const gbtGame &p_efg);
  
  gbtGamePlayer operator*(void) const;
  gbtGamePlayerIterator &operator++(int);

  bool Begin(void);
  bool End(void) const;
};

#endif  // GAME_PLAYER_H