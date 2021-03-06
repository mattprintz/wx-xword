// This file is part of XWord
// Copyright (C) 2012 Mike Richards ( mrichards42@gmx.com )
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

#ifndef METADATA_CTRL_H
#define METADATA_CTRL_H

#include "config.hpp"
#include "widgets/HtmlText.hpp"
#include "puz/Puzzle.hpp"
class MyFrame;

enum
{
    META_USE_LUA = 1 << 4
};

class MetadataCtrl
    : public HtmlText
{
public:
    MetadataCtrl() : m_frame(NULL) {}

    MetadataCtrl(wxWindow * parent,
               wxWindowID id,
               const wxString & displayFormat = wxEmptyString,
               const wxPoint & position = wxDefaultPosition,
               const wxSize & size = wxDefaultSize,
               long style = wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL | META_USE_LUA,
               const wxString & name = _T("MetadataCtrl"))
        : m_frame(NULL)
    {
        Create(parent, id, displayFormat, position, size, style, name);
    }

    virtual ~MetadataCtrl();

    bool Create(wxWindow * parent,
                wxWindowID id,
                const wxString & displayFormat = wxEmptyString,
                const wxPoint & position = wxDefaultPosition,
                const wxSize & size = wxDefaultSize,
                long style = wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL,
                const wxString & name = _T("MetadataCtrl"));

    const wxString & GetDisplayFormat() const { return m_displayFormat; }
    void SetDisplayFormat(const wxString & format)
    {
        m_displayFormat = format;
        UpdateLabel();
    }
    void UpdateLabel() { SetLabel(FormatLabel()); }

    bool HasaLuaFormat() const { return m_useLua; }
    void SetLuaFormat(bool lua) { m_useLua = lua; UpdateLabel(); }

    // Format a label
    static wxString FormatLabel(const wxString & str, MyFrame * frame, bool useLua = false);

    // Get a metadata value
    static wxString GetMeta(const wxString & str, MyFrame * frame);

    // Assign the config key for this control
    // This automatically adds config callbacks, and an alignment contenxt menu.
    void SetConfig(ConfigManager::Metadata_t * cfg);

protected:
    wxString FormatLabel() { return FormatLabel(m_displayFormat, m_frame, m_useLua); }
    void OnContextMenu(wxContextMenuEvent & evt);

    wxString m_displayFormat;
    MyFrame * m_frame;
    bool m_useLua;
    ConfigManager::Metadata_t * m_cfg;

    DECLARE_NO_COPY_CLASS(MetadataCtrl)
    DECLARE_DYNAMIC_CLASS(MetadataCtrl)
};

#endif // METADATA_CTRL_H
