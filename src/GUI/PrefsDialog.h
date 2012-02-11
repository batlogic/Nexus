
#ifndef PREFSDIALOG_H
#define PREFSDIALOG_H

#include "Editor.h"
#include "Widgets.h"

class SelectBox;


class PrefsDialog : public ModalDialog, public CControlListener
{
public:
    PrefsDialog( CView* owner, Editor* editor );
    void valueChanged( CControl *pControl );
    void drawBackgroundRect( CDrawContext* pContext, CRect& rcUpdate );
    const string& getColorScheme();

    CLASS_METHODS( PrefsDialog, ModalDialog )

    CCoord width_, height_;
    INT16 tuning_;
    string colorScheme_;

protected:
    enum Command { CmdTuning=3, CmdSkin=4, CmdWidth=5, CmdHeight=6 };
    CRect rcBtnOk_;
    CRect rcBtnCancel_;
    SelectBox* sbSkin_;
};


#endif //PREFSDIALOG_H



