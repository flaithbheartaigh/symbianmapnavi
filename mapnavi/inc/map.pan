/*
============================================================================
 Name		: LBSSample.pan
 Author	  : Me
 Copyright   : Me
 Description : This file contains panic codes.
============================================================================
*/

#ifndef __LBSSAMPLE_PAN__
#define __LBSSAMPLE_PAN__

/** LBSSample application panic codes */
enum TLBSSamplePanics
	{
	ELBSSampleUi = 1,
    EClientEngine
	// add further panics here
	};
	


inline void Panic(TLBSSamplePanics aReason)
	{
	_LIT(applicationName,"LBSSample");
	User::Panic(applicationName, aReason);
	}

#endif // __LBSSAMPLE_PAN__
