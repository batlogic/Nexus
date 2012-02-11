
#include "Dialogs.h"
#include "Widgets.h"
#include "ModuleBox.h"
#include "ModulePort.h"
#include "ImagePool.h"
#include "FontResources.h"
#include "PatchView.h"



//--------------------------------------------------------------
// class PortDialog
//--------------------------------------------------------------

PortDialog::PortDialog( PatchView* owner, ModulePort* port ) : 
    ModalDialog( owner ),
    port_( port ),
    patchView_( owner )
{
	setTransparency( true );

    CBitmap* imgBkgnd = imagePool.get( ImagePool::bkgndPortDialog );
    setBackground( imgBkgnd );
    size( 0, 0, imgBkgnd->getWidth(), imgBkgnd->getHeight() );

    CBitmap* imgBtnCancel = imagePool.get( ImagePool::btnCancel );
    CBitmap* imgBtnRemove = imagePool.get( ImagePool::btnRemoveLink );

    CRect rcSelect( 20, 38, 140, 56 );
    CRect rcBtnRemove( 155, 35, 155+imgBtnRemove->getWidth(), 35+imgBtnRemove->getHeight()/2 );
    CRect rcBtnClose( 220, 75, 220+imgBtnCancel->getWidth(), 75+imgBtnCancel->getHeight()/2 );

    SelectBox* selector = new SelectBox( rcSelect, this, CmdPortSelect );
    selector->setFont( fontArial9 );
    selector->setTextColor( kBlackCColor );
    selector->setBackgroundColor( colBkgndBrightest );
    selector->setFrameColor( colBkgndBright );
    selector->setSelectionColor( colOrange );
    addView( selector );

    SelectBoxList portLabels;
    buildPortLabels( portLabels );
    selector->setList( portLabels, 3 );
    
	CKickButton* btnRemove = new CKickButton( rcBtnRemove, this, CmdRemove, imgBtnRemove->getHeight()/2, imgBtnRemove );
    btnRemove->setWantsFocus( true );
	addView( btnRemove );
	
    CKickButton* btnClose = new CKickButton( rcBtnClose, this, CmdCommit, imgBtnCancel->getHeight()/2, imgBtnCancel );
    btnClose->setWantsFocus( true );
	addView( btnClose );
}


void PortDialog::buildPortLabels( SelectBoxList& list )
{
    for( UINT16 i=0; i<port_->links_.size(); i++ )
    {
        LinkInfo& linkInfo = port_->links_.at( i );
        ModuleBox* box     = patchView_->getBox( linkInfo.sourceId_ );
        ASSERT( box );

        if( box ) {
            ModuleData* data = box->data_;
            ModulePort* port = box->getPort( linkInfo.outputId_, OUT_PORT );
            ASSERT( port );

            if( port ) {
                string label = ModuleInfo::getModuleLabel( data->catalog_ );
                list.push_back( label + ":" + port->info_->label_ );
            }
        }
    }
}


void PortDialog::valueChanged( CControl *pControl )
{
    UINT16 cmd = (Command)pControl->getTag();

    switch( cmd ) {
    case CmdCommit:     if( pControl->getValue() == 0.0f ) close( CmdCommit ); break;
    case CmdPortSelect: TRACE( "PortDialog::valueChanged selected=%d\n", (int)pControl->getValue() ); break;
    case CmdRemove:     removeLink(); break;
    }
}


void PortDialog::removeLink()
{
}



