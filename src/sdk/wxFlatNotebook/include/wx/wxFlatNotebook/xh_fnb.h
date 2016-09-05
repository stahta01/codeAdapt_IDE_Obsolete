/////////////////////////////////////////////////////////////////////////////
// Name:        xh_fnb.h
// Purpose:     XRC resource for wxFlatNotebook
// Author:      Armel Asselin
// Created:     2006/10/30
// RCS-ID:      $Id: xh_fnb.h 7947 2012-04-28 10:01:45Z mortenmacfly $
// Copyright:   (c) 2006 Armel Asselin
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_XH_FLATNOTBK_H_
#define _WX_XH_FLATNOTBK_H_


#include "wx/xrc/xmlres.h"

#if wxUSE_NOTEBOOK

#include "wx/wxFlatNotebook/wxFlatNotebook.h"

class WXDLLIMPEXP_FNB wxFlatNotebook;

class WXDLLIMPEXP_FNB wxFlatNotebookXmlHandler : public wxXmlResourceHandler
{
DECLARE_DYNAMIC_CLASS(wxFlatNotebookXmlHandler)
public:
    wxFlatNotebookXmlHandler();
    virtual wxObject *DoCreateResource();
    virtual bool CanHandle(wxXmlNode *node);

private:
    bool m_isInside;
    wxFlatNotebook *m_notebook;
};

#endif

#endif // _WX_XH_NOTBK_H_
