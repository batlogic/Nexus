
#ifndef DIALOGS_H
#define DIALOGS_H

#include "Editor.h"
#include "Widgets.h"

class ModulePort;
class PatchView;

//--------------------------------------------------------------
// class PortDialog
//--------------------------------------------------------------

class PortDialog : public ModalDialog, public CControlListener
{
public:
	PortDialog( PatchView* owner, ModulePort* port );

    void valueChanged( CControl *pControl );

    CLASS_METHODS( PortDialog, ModalDialog )

protected:
    void removeLink();
    void buildPortLabels( SelectBoxList& list );

    enum Command { CmdPortSelect = 2, CmdRemove = 3 };

    ModulePort* port_;
    PatchView* patchView_;
};


#endif //DIALOGS_H