// This file is part of XWord
// Copyright (C) 2011 Mike Richards ( mrichards42@gmx.com )
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either
// version 3 of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

#ifndef PUZ_FORMAT_XPF_H
#define PUZ_FORMAT_XPF_H

#include "Puzzle.hpp"
#include <string>
#include "parse/xml.hpp"

namespace puz {

class XPFData : public Puzzle::FormatData
{
public:
    XPFData(xml::document * root = NULL) : root(root) {}
    ~XPFData() { delete root; }
    xml::document * root;
};

void LoadXPF(Puzzle * puz, const std::string & filename, void * /* dummy */);
void SaveXPF(Puzzle * puz, const std::string & filename, void * /* dummy */);

} // namespace puz

#endif // PUZ_FORMAT_XPF_H
