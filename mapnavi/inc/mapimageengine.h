#ifndef __IMAGEREADER_H__
#define __IMAGEREADER_H__

#include <e32base.h>
#include <coecntrl.h>
#include <w32std.h>
#include <e32std.h>
#include <cntdef.h>
#include <cntdb.h>
#include <ImageConversion.h>



class CFbsBitmap;

	class MImageReadyCallBack
	{
	public:
		virtual void ImageReadyL(const TInt& aError) = 0;
		};

	class CImageReader : public CActive
	{
	public:
	    CImageReader(MImageReadyCallBack& aNotify);
	    void ConstructL(const TDesC& aFileName);
	    ~CImageReader();
	public:
	    CFbsBitmap* Bitmap();
	protected:
		void DoCancel();
		void RunL();
	private:
		void GetFileType(const TDesC& aFileName, TDes8& aFileType);
	private:
		MImageReadyCallBack&	iNotify;
		CImageDecoder*			iImageDecoder;
		CFbsBitmap*				iFrame;
		TFileName				iImageName;
	};


#endif