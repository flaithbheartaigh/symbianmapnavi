#include "mapimageengine.h"

#include <cntfield.h>
#include <cntdef.h> 
#include <cntitem.h>
#include <cntfldst.h>
#include <cntdb.h>
#include <COEMAIN.H>
#include <APMREC.H>
#include <APGCLI.H>
 
 /**
  * 
  * Reads image from filesystem and displays it
  * 
  * 
  * 
  * 
  */
CImageReader::CImageReader(MImageReadyCallBack& aNotify)
:CActive(0),iNotify(aNotify)
    {
    }
 
CImageReader::~CImageReader()
{
	Cancel();
	delete iFrame;
	delete iImageDecoder;
}
 
 
void CImageReader::ConstructL(const TDesC& aFileName)
{
	CActiveScheduler::Add(this);
	
	iImageName.Copy(aFileName);
	
	TBuf8<255> ImageType;
	GetFileType(aFileName, ImageType);
	
	if(ImageType.Length() && iImageName.Length())
	{		
		iImageDecoder = CImageDecoder::FileNewL(CCoeEnv::Static()->FsSession(),aFileName,ImageType);
 
		delete iFrame;
		iFrame = NULL;
		iFrame = new(ELeave)CFbsBitmap();
		iFrame->Create(iImageDecoder->FrameInfo(0).iOverallSizeInPixels,iImageDecoder->FrameInfo(0).iFrameDisplayMode);
 
		iImageDecoder->Convert(&iStatus,*iFrame,0);
		SetActive();
	}
	else
	{
		TRequestStatus* status=&iStatus;
		User::RequestComplete(status, KErrNotSupported);
		SetActive();
	}
}
 
void CImageReader::GetFileType(const TDesC& aFileName, TDes8& aFileType)
{
	TEntry FileEntry;
 
	if(CCoeEnv::Static()->FsSession().Entry(aFileName,FileEntry) == KErrNone)
	{
		TBuf8<255> FileBuffer;
		
		if(!FileEntry.IsDir())
		{
			TInt FileSize = FileEntry.iSize;
 
			if(FileSize > 255)
			{
				FileSize = 255;
			}
			
			if(CCoeEnv::Static()->FsSession().ReadFileSection(aFileName,0,FileBuffer,FileSize) == KErrNone)
			{
				RApaLsSession RSession;
				if(RSession.Connect() == KErrNone)
				{	
					TDataRecognitionResult FileDataType;
 
					RSession.RecognizeData(aFileName,FileBuffer,*&FileDataType);
					
				//	if(FileDataType.iConfidence > aResult.iConfidence)
				//	{
						aFileType.Copy(FileDataType.iDataType.Des8());
				//	}
					
					RSession.Close();
				}
			}
		}
	}
}
 
void CImageReader::DoCancel()
{
	iImageDecoder->Cancel();
}
 
CFbsBitmap* CImageReader::Bitmap()
{
	return iFrame;
}
 
	
void CImageReader::RunL()
{
	iNotify.ImageReadyL(iStatus.Int());
}
