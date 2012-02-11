
#include "SystemView.h"
#include "Fonts.h"
#include "SelectBox.h"
#include "../SoundDevices.h"
#include "../AppOptions.h"
#include "../Synth.h"



//---------------------------------------------------------------
// class SystemView
//---------------------------------------------------------------

SystemView::SystemView( const CRect& size, Editor* editor) : 
    CViewContainer( size, editor->getChildFrame(), NULL ),
	editor_( editor )
{
    setMouseableArea( size );
	setTransparency( false );
    setWantsFocus( true );
    setAutosizeFlags( kAutosizeAll );

#ifdef _RT_AUDIO
    CRect rcAudioOutPanel( 77, 66, 477, 270 );
    CRect rcMidiInPanel( 487, 66, 687, 270 );

    audioOutPanel_ = new AudioOutPanel( rcAudioOutPanel, editor_ );
    addView( audioOutPanel_ );

    midiInPanel_ = new MidiInPanel( rcMidiInPanel, editor_ );
    addView( midiInPanel_ );

    setColors();
#endif
}


void SystemView::setColors()
{
    setBackgroundColor( colors.fill1 );
#ifdef _RT_AUDIO
    audioOutPanel_->setColors();
    midiInPanel_->setColors();
#endif
}


void SystemView::onEngineRun( bool run )
{
#ifdef _RT_AUDIO
    AudioOutPanel* panel = dynamic_cast< AudioOutPanel* >( getView( 0 ));
    if( panel ) 
        panel->onEngineRun( run );
#endif
}


#ifdef _RT_AUDIO
//-------------------------------------------------------------------------
// class AudioOutPanel
//-------------------------------------------------------------------------

AudioOutPanel::AudioOutPanel( const CRect& rcSize, Editor* editor ) : 
    CViewContainer( rcSize, editor->getChildFrame(), NULL ),
    editor_( editor )
{
    setTransparency( true );
    setWantsFocus( true );

    CRect rcLabelPanel( 5, 0, 150, 15 );
    CRect rcLabelAsio( 302, 38, 340, 52 );
    CRect rcEditAsio( 280, 38, 296, 54 );
    CRect rcEditDevice( 25, 38, 260, 56 );
    CRect rcEditChannel( 25, 70, 260, 88 );
    CRect rcEditBuffer( 25, 102, 105, 120 );
    CRect rcLabelBuffer( 115, 102, 200, 120 );
    CRect rcEditRate( 25, 132, 105, 150 );
    CRect rcLabelRate( 115, 132, 200, 150 );
    CRect rcEditRun( 25, size.bottom-size.top-18, 43, size.bottom-size.top );
    CRect rcLabelRun( 50, size.bottom-size.top-18, 150, size.bottom-size.top );
    CRect rcLabelLatency( 230, size.bottom-size.top-18, 330, size.bottom-size.top-1 );
    
    lblCaption_        = new Label( rcLabelPanel, "Audio Out" );
    lblCaption_->font_ = fontPirulen9;
    addView( lblCaption_ );

    lblAsio_        = new Label( rcLabelAsio, "Asio" );
    lblAsio_->font_ = fontArial9;
    addView( lblAsio_ );

    cbAsio_ = new Checkbox( rcEditAsio, this, CmdAsio, false );
    addView( cbAsio_ );

    lbLatency_            = new Label( rcLabelLatency, "" );
    lbLatency_->font_     = fontArial9;
    lbLatency_->hAlign_   = kCenterText;
    addView( lbLatency_ );

    cbRun_ = new Checkbox( rcEditRun, this, CmdRun, editor_->getOptions()->audioRun_ );
    addView( cbRun_ );

    lblRun_ = new Label( rcLabelRun, "Engine Run" );
    lblRun_->font_ = fontArial9;
    addView( lblRun_ );
    
    sbDevice_ = new SelectBox( rcEditDevice, this, CmdDevice );
    sbDevice_->setFont( fontArial9 );
    
    sbChannel_ = new SelectBox( rcEditChannel, this, CmdChannel );
    sbChannel_->setFont( fontArial9 );

    lbBuffer_        = new Label( rcLabelBuffer, "Buffer size" );
    lbBuffer_->font_ = fontArial9;
    addView( lbBuffer_ );
    
    sbBuffer_ = new SelectBox( rcEditBuffer, this, CmdBuffer );
    sbBuffer_->setFont( fontArial9 );

    lbRate_           = new Label( rcLabelRate, "Sample rate" );
    lbRate_->font_    = fontArial9;
    addView( lbRate_ );
    
    sbRate_        = new SelectBox( rcEditRate, this, CmdRate );
    sbRate_->setFont( fontArial9 );

    addView( sbRate_ );
    addView( sbBuffer_ );
    addView( sbChannel_ );
    addView( sbDevice_ );
    
    setColors();
}


void AudioOutPanel::setColors()
{
    lblCaption_->colText_ = colors.text2;
    lblAsio_->colText_    = colors.text2;
    lbBuffer_->colText_   = colors.text2;
    lbRate_->colText_     = colors.text2;
    lblRun_->colText_     = colors.text2;
    lbLatency_->colText_  = colors.fill2;
    lbLatency_->colBkgnd_ = colors.selection;
        
    cbAsio_->colBkgnd_ = colors.fill2;
    cbAsio_->colFrame_ = colors.fill2;
    cbAsio_->colTick_  = colors.masterFrame3;

    cbRun_->colBkgnd_ = colors.fill2;
    cbRun_->colFrame_ = colors.fill2;
    cbRun_->colTick_  = colors.selection;

    sbDevice_->setTextColor( colors.text2 );
    sbDevice_->setBackgroundColor( colors.fill1 );
    sbDevice_->setFrameColor( colors.fill2 );
    sbDevice_->setSelectionColor( colors.selection );

    sbChannel_->setTextColor( colors.text2 );
    sbChannel_->setBackgroundColor( colors.fill1 );
    sbChannel_->setFrameColor( colors.fill2 );
    sbChannel_->setSelectionColor( colors.selection );

    sbBuffer_->setTextColor( colors.text2 );
    sbBuffer_->setBackgroundColor( colors.fill1 );
    sbBuffer_->setFrameColor( colors.fill2 );
    sbBuffer_->setSelectionColor( colors.selection );

    sbRate_->setTextColor( colors.text2 );
    sbRate_->setBackgroundColor( colors.fill1 );
    sbRate_->setFrameColor( colors.fill2 );
    sbRate_->setSelectionColor( colors.selection );
}


bool AudioOutPanel::attached( CView* parent )
{
    bool result = CViewContainer::attached( parent );
    update();
    return result;
}


void AudioOutPanel::update()
{
    SoundDevices* devices       = editor_->getDevices();
    DeviceInfoList& deviceInfos = devices->deviceInfos_;
    DeviceInfo* deviceInfo      = devices->getAudioOutDevice();
    INT16 deviceId              = deviceInfo ? deviceInfo->id : -1;
    
    // update asio
    cbAsio_->setState( devices->getAudioPort() == RtAudio::WINDOWS_ASIO );
    cbRun_->setState( editor_->getOptions()->audioRun_ );

    // update port
    SelectBoxList deviceList;
    deviceList.push_back( "[not connected]" );

    for( UINT16 i=0; i<deviceInfos.size(); i++ ) {
        deviceList.push_back( deviceInfos.at( i ).name );
    }
    sbDevice_->setList( deviceList, 4 );
    sbDevice_->setIndex( deviceId + 1 );

    // update the rest
    bool visible = deviceInfo != NULL && deviceInfo->outputChannels > 0;

    sbChannel_->setVisible( visible );
    sbBuffer_->setVisible( visible );
    lbBuffer_->setVisible( visible );
    lbRate_->setVisible( visible );
    sbRate_->setVisible( visible );
    lbLatency_->setVisible( visible );

    if( visible )
    {
        // update channel
        if( devices->getAudioPort() == RtAudio::WINDOWS_ASIO )
        {
            SelectBoxList channelList;
            vector<string>& outChannelNames = deviceInfo->outChannelNames;
            for( UINT16 i=0; i<outChannelNames.size()-1 && outChannelNames.size()>0; i+=2 ) 
            {
                string name = outChannelNames[i] + " / " + outChannelNames[i+1];
                channelList.push_back( name );
            }
            sbChannel_->setList( channelList, 4 );
            sbChannel_->setIndex( devices->getAudioOutChannel() );
        }
        else if( devices->getAudioPort() == RtAudio::WINDOWS_DS ) {     // DS: only two channels
            sbChannel_->setVisible( false );
        }

        // update buffersize
        SelectBoxList bufferList;
        UINT16 selectedBuffer = 0;
        vector<unsigned int>& bufferSizes = deviceInfo->bufferSizes;
        for( UINT16 i=0; i<bufferSizes.size(); i++ ) 
        {
            ostringstream os;
            os << bufferSizes[i];
            bufferList.push_back( os.str() );
            if( bufferSizes[i] == devices->getBufferSize() ) 
                selectedBuffer = i;
        }
        sbBuffer_->setList( bufferList, 4 );
        sbBuffer_->setIndex( selectedBuffer );

        // update samplerate
        SelectBoxList rateList;
        UINT16 selectedRate = 0;
        vector<unsigned int>& rates = deviceInfo->sampleRates;
        for( UINT16 i=0; i<rates.size(); i++ ) 
        {
            ostringstream os;
            os << rates[i];
            rateList.push_back( os.str() );
            if( rates[i] == devices->getSampleRate() ) 
                selectedRate = i;
        }
        sbRate_->setList( rateList, 4 );
        sbRate_->setIndex( selectedRate );

        // update latency
        ostringstream os;
        os << setprecision( 2 ) << fixed << devices->getLatency() / 44.1;
        lbLatency_->setText( "Latency: " + os.str() + " ms" );
        lbLatency_->setVisible( devices->getLatency() > 0 && editor_->getOptions()->audioRun_ );
    }
    editor_->checkAppState();
    invalid();
}


void AudioOutPanel::valueChanged( CControl* pControl )
{
    setCursor( CursorWait );
    SoundDevices* devices       = editor_->getDevices();
    DeviceInfoList& deviceInfos = devices->deviceInfos_;
    DeviceInfo* deviceInfo      = devices->getAudioOutDevice();
    INT16 deviceId              = deviceInfo ? deviceInfo->id : -1;

    long tag     = pControl->getTag();
    UINT16 value = (UINT16)pControl->getValue();

    switch( tag )
    {
    case CmdAsio:
    {
        AudioPort port = value == 0 ? RtAudio::WINDOWS_DS : RtAudio::WINDOWS_ASIO;
        if( port != devices->getAudioPort() )
        {
            devices->reset(); 
            devices->openPort( port );
            update();
        }
        break;
    }
    case CmdRun:
    {
        bool checked = value == 0 ? false : true;
        onEngineRun( checked );
        break;
    }
    case CmdDevice:
    {
        INT16 idx = value - 1;
        if( deviceId != idx )
        {
            if( idx >= 0 && idx < (INT16)devices->deviceInfos_.size() ) 
                devices->setBufferSize( devices->deviceInfos_[ idx ].preferBufferSize, false );
            devices->setAudioOutDevice( idx );
            update();
        }
        break;
    }
    case CmdChannel:
    {
        INT16 idx = value * 2;
        if( devices->getAudioOutChannel() != idx )
        {
            devices->setAudioOutChannel( value * 2 );
            update();
        }
        break;
    }
    case CmdBuffer:
    {
        if( deviceId >= 0 )
        {
            vector<unsigned int>& bufferSizes = devices->deviceInfos_[ deviceId ].bufferSizes;
            if( value < bufferSizes.size()) {
                devices->setBufferSize( bufferSizes[ value ], true );
                update();
            }
        }
        break;
    }
    case CmdRate:
    {
        if( deviceId >= 0 )
        {
            vector<unsigned int>& rates = devices->deviceInfos_[ deviceId ].sampleRates;
            if( (UINT16)value < rates.size() ) {
                devices->setSampleRate( rates[ (UINT16)value ] );
                update();
            }
        }
        break;
    }
    }
    setCursor( CursorDefault );
}


void AudioOutPanel::onEngineRun( bool run )
{
    if( run != editor_->getOptions()->audioRun_ )
    {
        editor_->getOptions()->audioRun_ = run;
        if( run == false ) {
            editor_->getDevices()->close();
        }
        else {
            editor_->getDevices()->open();
            editor_->getOptions()->audioRun_ = editor_->getDevices()->getState() == SoundDevices::Ready;
        }
    }
    if( isAttached() ) {
    	update();
    }
}


CMouseEventResult AudioOutPanel::onMouseDown( CPoint& pos, const long& buttons )
{
    CMouseEventResult result = CViewContainer::onMouseDown( pos, buttons );

    CView* view = getViewAt( pos );
    getFrame()->setFocusView( view );

    return result;
}


void AudioOutPanel::drawRect( CDrawContext* pContext, const CRect& updateRect )
{
    pContext->setLineWidth( 1 );
    pContext->setFrameColor( colors.fill2 );
    pContext->setFillColor( colTransparent );

    CRect rcFrame( size.left, size.top+15, size.right, size.bottom );
    pContext->drawRect( rcFrame, kDrawFilledAndStroked );

    drawBackToFront( pContext, updateRect );
    setDirty( false );
}


void AudioOutPanel::setCursor( CursorType type )
{
    ChildFrame* childFrame = dynamic_cast< ChildFrame* >( pParentFrame );
    if( childFrame ) {
        childFrame->setCursor( type );
    }
}



//--------------------------------------------------------------------
// class MidiBoxItem
//--------------------------------------------------------------------

MidiBoxItem::MidiBoxItem( const string& text, bool state ) :
    GridBoxItem()
{
    label_            = new Label( CRect( 0,0,0,0 ), text );
    label_->font_     = fontArial9;
    label_->indent_   = CPoint( 3, 0 );
    addControl( label_ );

    checkbox_            = new Checkbox( CRect( 0,0,0,0 ), this, 0, state );
    checkbox_->tickSize_ = CPoint( 7, 7 );
    addControl( checkbox_ );

    setColors();
}


void MidiBoxItem::setColors()
{
    label_->colText_     = colors.text2;
    checkbox_->colTick_  = colors.selection;
    checkbox_->colBkgnd_ = colors.fill2;
    checkbox_->colFrame_ = colTransparent;
}


//--------------------------------------------------------------------
// class MidiInPanel
//--------------------------------------------------------------------

MidiInPanel::MidiInPanel( const CRect& rcSize, Editor* editor ) : 
    CViewContainer( rcSize, editor->getChildFrame(), NULL ),
    editor_( editor )
{
    setTransparency( true );
    setWantsFocus( true );

    SoundDevices* devices = editor_->getDevices();
    MidiPortMap& inPorts  = devices->midiInInfo_;

    CCoord itemHeight = 18;
    CCoord boxHeight  = min( 6*18+1, inPorts.size()*18+1 );
    CRect rcLabelPanel( 5, 0, 150, 15 );
    CRect rcBox( 25, 38, 185, 38+boxHeight );
    
    lblCaption_        = new Label( rcLabelPanel, "Midi In" );
    lblCaption_->font_ = fontPirulen9;
    addView( lblCaption_ );

    gbPorts_ = new GridBox( rcBox, getFrame(), this );
    gbPorts_->setSelectionStyle( GridBox::NoSel );
    gbPorts_->setItemHeight( itemHeight );
    gbPorts_->addColumn( 86 );
    gbPorts_->addColumn( 14 );

    for( UINT16 i=0; i<inPorts.size(); i++ )
    {
        string portName   = inPorts.getPortName( i );
        bool checked      = devices->midiInPortIsOpen( portName );
        MidiBoxItem* item = new MidiBoxItem( portName, checked );
        gbPorts_->addItem( item );
    }
    setColors();
    gbPorts_->layout();
    addView( gbPorts_ );
}


void MidiInPanel::setColors()
{
    lblCaption_->colText_ = colors.text2;
    gbPorts_->setColors( colors.fill1, colors.fill2, colors.selection, colors.text1 );
}


void MidiInPanel::valueChanged( INT32 row, INT32 col )
{
    if( col == 1 )
    {
        SoundDevices* devices = editor_->getDevices();
        CView* view           = gbPorts_->getView( row );
        MidiBoxItem* item     = dynamic_cast< MidiBoxItem* >( view );

        if( item )
        {
            string portName = item->getText();
            bool open       = item->getState();

            open ? 
                devices->openMidiInPort( portName ) : 
                devices->closeMidiInPort( portName );
        }
    }
}


void MidiInPanel::drawRect( CDrawContext* pContext, const CRect& updateRect )
{
    CViewContainer::drawRect( pContext, updateRect );
        
    pContext->setLineWidth( 1 );
    pContext->setFrameColor( colors.fill2 );
    pContext->setFillColor( colTransparent );

    CRect rcFrame( size.left, size.top+15, size.right, size.bottom );
    pContext->drawRect( rcFrame, kDrawFilledAndStroked );

    setDirty( false );
}






#endif // _RT_AUDIO
