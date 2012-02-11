
#ifndef WIRE_H
#define WIRE_H

#include "Editor.h"
#include "../ModuleData.h"
#include "../ModuleInfo.h"



class Wire : public vector< CPoint >
{
public:
	Wire( const CPoint& first, const CPoint& last );
    Wire( Wire* wire );

	void addPoint( const CPoint& pos );
    bool hitTest( const CRect& r );
    void getBoundingRect( CRect& r );

	void draw( CDrawContext* pContext );

    bool selected_;

protected:
    bool Wire::segmentIntersectRectangle( const CRect& rc, const CPoint& a1, const CPoint& a2 );
};

typedef map< const LinkInfo, Wire* > WireMap;

#endif // WIRE_H