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

#include "jpz.hpp"

#include "Puzzle.hpp"
#include "Clue.hpp"
#include "puzstring.hpp"
#include "parse/xml.hpp"
#include "utils/minizip.hpp"
#include <sstream>

namespace puz {

class jpzParser : public xml::Parser
{
public:
    virtual bool DoLoadPuzzle(Puzzle * puz, xml::document & doc);

    // Return the square from this element
    Square * GetSquare(Puzzle * puz, xml::node &);
    Square * RequireSquare(Puzzle * puz, xml::node &);
};


void LoadJpz(Puzzle * puz, const std::string & filename, void * /* dummy */)
{
    jpzParser parser;

    // Open a zip archive
    unzip::Archive zip(filename);
    if (! zip)
    {
        parser.LoadFromFilename(puz, filename);
        return;
    }
    // Browse the archive
    unzip::File f = zip.First();
    int n_files = zip.GetFileCount();
    while (f)
    {
        // Read the file
        f.Open();
        std::ostringstream stream;
        for (;;)
        {
            char buf[1024];
            int chars_read = f.Read(buf, 1024);
            stream.write(buf, chars_read);
            if (chars_read <= 0)
                break;
        }
        if (n_files == 1)
        {
            parser.LoadFromString(puz, stream.str().c_str());
            return;
        }
        else
        {
            try {
                parser.LoadFromString(puz, stream.str().c_str());
                return;
            }
            catch (...) {
                // Try the next file
            }
        }
        f.Next();
    }
    throw FatalFileError("Could not load any files in the archive");
}

Square *
jpzParser::RequireSquare(Puzzle * puz, xml::node & node)
{
    string_t x = GetAttribute(node, "x");
    string_t y = GetAttribute(node, "y");
    if (x.find(puzT("-")) != string_t::npos || y.find(puzT("-")) != string_t::npos)
        throw InvalidGridCell("Spanned cells are not supported.");
    if (x.empty() || x.empty())
        throw InvalidGridCell("Cell missing x or y");
    try
    {
        // x and y are 1-based
        return &puz->GetGrid().At(ToInt(x) - 1, ToInt(y) - 1);
    }
    catch (...)
    {
        throw InvalidGridCell("Cell is out of range");
    }
}

Square *
jpzParser::GetSquare(Puzzle * puz, xml::node & node)
{
    try {
        return RequireSquare(puz, node);
    }
    catch(...) {
        return NULL;
    }
}


void parse_range(const string_t & str, int * val1, int * val2)
{
    int index = str.find(puzT("-"));
    if (index == string_t::npos)
    {
        *val1 = ToInt(str);
        *val2 = *val1;
    }
    else
    {
        *val1 = ToInt(str.substr(0, index));
        *val2 = ToInt(str.substr(index+1));
    }
}

Word make_word(Puzzle * puz, const string_t & x, const string_t & y)
{
    int x1, x2;
    parse_range(x, &x1, &x2);
    if (x1 == -1 || x2 == -1)
        throw InvalidGridCell("Invalid cell range in word.");
    int y1, y2;
    parse_range(y, &y1, &y2);
    if (y1 == -1 || y2 == -1)
        throw InvalidGridCell("Invalid cell range in word.");
    return puz->MakeWord(x1-1, y1-1, x2-1, y2-1); // offsets are from 1
}



bool jpzParser::DoLoadPuzzle(Puzzle * puz, xml::document & doc)
{
    Grid & grid = puz->GetGrid();

    xml::node applet = RequireChild(doc, "crossword-compiler-applet");
    xml::node puzzle = RequireChild(applet, "rectangular-puzzle");
    xml::node crossword = RequireChild(puzzle, "crossword");

    // Metadata
    xml::node meta = puzzle.child("metadata");
    puz->SetTitle(GetInnerXML(meta, "title"));
    puz->SetAuthor(GetInnerXML(meta, "creator"));
    puz->SetCopyright(GetInnerXML(meta, "copyright"));

    if (puz->GetTitle().empty())
        puz->SetTitle(GetInnerXML(applet, "title"));
    if (puz->GetCopyright().empty())
        puz->SetCopyright(GetInnerXML(applet, "copyright"));

    puz->SetNotes(GetInnerXML(puzzle, "instructions"));

    // Grid
    {
        xml::node grid_node = RequireChild(crossword, "grid");

        int width = grid_node.attribute("width").as_int();
        int height = grid_node.attribute("height").as_int();
        if (width < 1 || height < 1)
            throw FatalFileError("Invalid grid size");
        grid.SetSize(width, height);

        // Default all squares to missing, because missing cells don't have to
        // be specified.
        {
            Square * square;
            for (square = grid.First(); square != NULL; square = square->Next())
                square->SetMissing();
        }

        // Grid cells
        xml::node cell = RequireChild(grid_node, "cell");
        for (; cell; cell = cell.next_sibling("cell"))
        {
            Square * square = RequireSquare(puz, cell);
            // Find the square's type
            string_t type = GetAttribute(cell, "type");
            if (type == puzT("void"))
            {
                square->SetMissing();
            }
            else if (type == puzT("block"))
            {
                square->SetMissing(false);
                square->SetBlack();
            }
            else if (type == puzT("clue"))
            {
                throw FatalFileError("Clues inside squares are not supported.");
            }
            else // type == puzT("letter")
            {
                square->SetMissing(false);
                square->SetSolution(GetAttribute(cell, "solution"));
                square->SetText(GetAttribute(cell, "solve-state"));
                string_t status = GetAttribute(cell, "solve-status");
                if (status == puzT("pencil"))
                    square->AddFlag(FLAG_PENCIL);
                if (status == puzT("revealed"))
                {
                    square->AddFlag(FLAG_REVEALED);
                    square->SetText(square->GetSolution());
                }
                if (GetAttribute(cell, "hint") == puzT("true"))
                    square->SetText(square->GetSolution());
                square->SetNumber(GetAttribute(cell, "number"));
                if (GetAttribute(cell, "background-shape") == puzT("circle"))
                    square->SetCircle();

                square->SetColor(GetAttribute(cell, "background-color"));
            }
        }
    }

    // Words
    std::map<string_t, Word *> words;
    {
        xml::node word = RequireChild(crossword, "word");
        for (; word; word = word.next_sibling("word"))
        {
           // Look for a cell range
            string_t x = GetAttribute(word, "x");
            string_t y = GetAttribute(word, "y");
            // Ignore the delaration if X or Y is provided, but not both.
            if (! x.empty() && ! y.empty())
                puz->AddWord(make_word(puz, x, y));
            else
                puz->AddWord(Word());
            string_t id = GetAttribute(word, "id");
            if (id.empty())
                throw FatalFileError("Each word must have an id");
            // Add this word to the internal map of words.
            Word & thisword = puz->GetWords().back();
            words[id] = &thisword;
            // TODO: hidden

            // Word cells
            xml::node cells = word.child("cells");
            for (; cells; cells = cells.next_sibling("cells"))
            {
                // Make a word from this cell.
                Word cell_word = make_word(puz, GetAttribute(cells, "x"),
                                                GetAttribute(cells, "y"));
                // Add it to the current word;
                Word::iterator it;
                for (it = cell_word.begin(); it != cell_word.end(); ++it)
                    thisword.push_back(*it);
            }
        }
    }

    // Clues
    {
        xml::node clues = crossword.child("clues");
        for (; clues; clues = clues.next_sibling("clues"))
        {
            string_t key = GetText(clues, "title");
            if (key.empty())
                throw FatalFileError("Each clue list must have a title");
            ClueList list(GetInnerXML(RequireChild(clues, "title")));
            xml::node clue = RequireChild(clues, "clue");
            for (; clue; clue = clue.next_sibling("clue"))
            {
                std::map<string_t, Word *>::iterator it;
                it = words.find(GetAttribute(clue, "word"));
                if (it == words.end())
                    throw FatalFileError("Each clue must have a word");
                string_t text = GetInnerXML(clue);
                string_t format = GetAttribute(clue, "format");
                string_t number = GetAttribute(clue, "number");
                if (! format.empty())
                    text.append(puzT(" (")).append(format).append(puzT(")"));
                list.push_back(Clue(number, text, it->second));
            }
            puz->SetClueList(key, list);
        }
    }

    // Timer
    {
        xml::node timer = applet.child("timer");
        puz->SetTime(timer.attribute("initial-value").as_int());
        puz->SetTimerRunning(
            GetAttribute(timer, "start-on-load") == puzT("true")
        );
    }

    return false; // Delete the doc
}

} // namespace puz
