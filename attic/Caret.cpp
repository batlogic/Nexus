
class Caret : public CBaseObject
{
public:
    Caret();
    ~Caret();

    void start( CView* parent, const CPoint& pos, CCoord height );
    void stop();
    void show();
    void hide();
    CMessageResult notify( CBaseObject* sender, const char* message );
    void setPosition( const CPoint& pos );

    CFrame* frame_;
    CPoint position_;
    CCoord height_;

protected:
    void killTimer();

    CView* parent_;
    CVSTGUITimer* timer_;
};



//-------------------------------------------
// class Caret
//-------------------------------------------

Caret::Caret() :
	CBaseObject(),
    frame_( NULL ),
    timer_( NULL )
    {}


Caret::~Caret()
{
    killTimer();
}
    

void Caret::start( CView* parent, const CPoint& pos, CCoord height )
{
    parent_ = parent;
    frame_  = parent_->getFrame();
    ASSERT( frame_ );
    
    HWND hwnd = (HWND)frame_->getSystemWindow();
    CreateCaret( hwnd, NULL, 1, height );

    setPosition( pos );
    height_   = height;
    
    ShowCaret( hwnd );

	//timer_ = new CVSTGUITimer( this, 500 );
	//timer_->start ();
}


void Caret::stop()
{
    frame_ = NULL;
    parent_ = NULL;
    
    ASSERT( timer_ );
    killTimer();
}


void Caret::killTimer()
{
    if( timer_ ) {
        timer_->stop();
        delete timer_;
        timer_ = NULL;
    }
}


CMessageResult Caret::notify( CBaseObject* sender, const char* message )
{
	static UINT16 counter = 0;
    if( message == CVSTGUITimer::kMsgTimer && timer_ )
	{
		if( (++counter % 2 ) == 0 ) {
            counter = 0;
            hide();
        }
        else {
            show();
        }
        return kMessageNotified;
	}
	return kMessageUnknown;
}


void Caret::show()
{
    if( frame_ ) 
    {
        CDrawContext* context = frame_->createDrawContext();
        context->setFrameColor( kBlackCColor );
        context->setLineWidth( 1 );

        CPoint p = position_;
        context->moveTo( p );
        p.offset( 0, height_ );
        context->lineTo( p );

        context->forget();
    }
}


void Caret::hide()
{
    if( parent_ ) {
        parent_->invalid();
    }
}


void Caret::setPosition( const CPoint& pos )
{
    if( parent_ ) 
    {
        position_ = pos;
        parent_->localToFrame( position_ );
        SetCaretPos( position_.x, position_.y );
    }
}



