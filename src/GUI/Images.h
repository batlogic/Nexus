
#ifndef IMAGES_H
#define IMAGES_H

#include <vector>
#include "../VST/vstgui.h"

using std::vector;


class Images
{
public:
	enum Id {
		imagesFirst		 = 1000,
        bkgndCtrlDialog  = 1000,
        bkgndPrefsDialog = 1001,
        btnOk            = 1002,
        btnCancel        = 1003,
        iconFrame        = 1004,
		imagesLast		 = 1004	
    };

	~Images();

    void loadImages();
	CBitmap* get( int id );

protected:
	static int instanceCount_;
	static vector< CBitmap* > images_;
};


extern Images images;

#endif // IMAGES_H