#ifndef VERSION_H
#define VERSION_H

namespace AutoVersion{
	
	//Date Version Types
	static const char DATE[] = "23";
	static const char MONTH[] = "09";
	static const char YEAR[] = "2010";
	static const char UBUNTU_VERSION_STYLE[] = "10.09";
	
	//Software Status
	static const char STATUS[] = "Alpha";
	static const char STATUS_SHORT[] = "a";
	
	//Standard Version Type
	static const long MAJOR = 0;
	static const long MINOR = 10;
	static const long BUILD = 9;
	static const long REVISION = 7390;
	
	//Miscellaneous Version Types
	static const long BUILDS_COUNT = 2410;
	#define RC_FILEVERSION 0,10,9,7390
	#define RC_FILEVERSION_STRING "0, 10, 9, 7390\0"
	static const char FULLVERSION_STRING[] = "0.10.9.7390";
	
	//These values are to keep track of your versioning state, don't modify them.
	static const long BUILD_HISTORY = 23;
	

}
#endif //VERSION_H
