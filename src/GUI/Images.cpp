
#include "Images.h"


int Images::instanceCount_ = 0;
vector< CBitmap* > Images::images_;

Images images;


void Images::loadImages()
{
	if( instanceCount_ == 0 )
	{
		instanceCount_++;
        images_.resize( imagesLast - imagesFirst + 1 );
		
		for( int i=imagesFirst; i<=imagesLast; i++ )
		{
			CBitmap* bm = new CBitmap( i );
			images_[ i-imagesFirst ] = bm;
		}
	}
}


Images::~Images()
{
	if( --instanceCount_ <= 0 )
	{
		for( unsigned int i=0; i<images_.size(); i++ )
		{
			CBitmap* bm = images_[ i ];
			if( bm ) bm->forget();
		}
		images_.resize( 0 );
	}
    instanceCount_ = max( 0, instanceCount_ );
}


CBitmap* Images::get( int id ) 
{ 
	if( instanceCount_ == 0 ) {
        loadImages();
    }

    id -= imagesFirst;
	if( id >= 0 && id < (INT16)images_.size() ) {
		return images_.at( id );
	}
	return NULL;
}
