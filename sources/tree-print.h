//
// $Source$
// $Date$
// $Revision$
//
// DESCRIPTION:
// Declaration of printout for extensive form tree
//
// This file is part of Gambit
// Copyright (c) 2004, The Gambit Project
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

#ifndef TREE_PRINT_H
#define TREE_PRINT_H

#include <wx/print.h>

#include "game-document.h"

class gbtTreeLayout;

class gbtTreePrintout : public wxPrintout {
private:
  const gbtTreeLayout &m_layout;

public:
  gbtTreePrintout(const gbtTreeLayout &, const wxString &);

  bool OnPrintPage(int page);
  bool HasPage(int page);
  bool OnBeginDocument(int startPage, int endPage);
  void GetPageInfo(int *minPage, int *maxPage,
		   int *selPageFrom, int *selPageTo);
};

#endif  // TREE_PRINT_H

