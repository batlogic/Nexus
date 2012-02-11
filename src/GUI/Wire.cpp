
#include "Wire.h"


Wire::Wire( const CPoint& first, const CPoint& last ) :
    selected_( false )
{
	addPoint( first );
	addPoint( last );
}


Wire::Wire( Wire* wire )
{
    ASSERT( wire );
    selected_ = wire->selected_;

    for( UINT16 i=0; i<wire->size(); i++ ) {
        push_back( wire->at( i ));
    }
}


void Wire::addPoint( const CPoint& pos )
{
	push_back( pos );
}


void Wire::draw( CDrawContext* pContext )
{
	if( size() < 2 ) {
		return;
	}
    CDrawMode oldDrawMode = pContext->getDrawMode();
    pContext->setDrawMode( kAntialias );

	selected_ ? pContext->setFrameColor( colors.wire2 ) : pContext->setFrameColor( colors.wire1 );
	pContext->setLineWidth( colors.wireWidth_ );

	pContext->moveTo( at( 0 ));
	for( UINT16 i=1; i<size(); i++ )
	{
		pContext->lineTo( at( i ));
	}
    pContext->setDrawMode( oldDrawMode );
}


void Wire::getBoundingRect( CRect& r )
{
    if( empty() == false )
    {
        CPoint a = front();
        CPoint b = back();

        r.left   = a.x;
        r.top    = a.y;
        r.right  = b.x;
        r.bottom = b.y;
        r.normalize();
    }
    else ASSERT( false );
}


bool Wire::segmentIntersectRectangle( const CRect& rc, const CPoint& a1, const CPoint& a2 )
{
    // Find min and max X for the segment
    float minX = (float)a1.x;
    float maxX = (float)a2.x;

    if( a1.x > a2.x ) {
        minX = (float)a2.x;
        maxX = (float)a1.x;
    }
    // Find the intersection of the segment's and rectangle's x-projections
    if( maxX > rc.right ) {
        maxX = (float)rc.right;
    }
    if( minX < rc.left ) {
        minX = (float)rc.left;
    }
    // If their projections do not intersect return false
    if( minX > maxX )  {
        return false;
    }

    // Find corresponding min and max Y for min and max X we found before
    float minY = (float)a1.y;
    float maxY = (float)a2.y;

    float dx = (float)(a2.x - a1.x);
    if( abs( dx ) > 0.0000001 )
    {
        float a = ( a2.y - a1.y ) / dx;
        float b = a1.y - a * a1.x;
        minY    = a * minX + b;
        maxY    = a * maxX + b;
    }
    if( minY > maxY ) {
        float tmp = maxY;
        maxY = minY;
        minY = tmp;
    }
    // Find the intersection of the segment's and rectangle's y-projections
    if( maxY > rc.bottom ) {
        maxY = (float)rc.bottom;
    }
    if( minY < rc.top ) {
        minY = (float)rc.top;
    }
    if( minY > maxY ) { // If Y-projections do not intersect return false
        return false;
    }
    return true;
}



bool Wire::hitTest( const CRect& rc )
{
    selected_ = segmentIntersectRectangle( rc, front(), back() );
    return selected_;
}

