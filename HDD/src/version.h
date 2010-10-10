#ifndef VERSION_H
#define VERSION_H

namespace AutoVersion{
	
	//Date Version Types
	static const char DATE[] = "10";
	static const char MONTH[] = "10";
	static const char YEAR[] = "2010";
	static const char UBUNTU_VERSION_STYLE[] = "10.10";
	
	//Software Status
	static const char STATUS[] = "Alpha";
	static const char STATUS_SHORT[] = "a";
	
	//Standard Version Type
	static const long MAJOR = 1;
	static const long MINOR = 1;
	static const long BUILD = 5;
	static const long REVISION = 10638;
	
	//Miscellaneous Version Types
	static const long BUILDS_COUNT = 3472;
	#define RC_FILEVERSION 1,1,5,10638
	#define RC_FILEVERSION_STRING "1, 1, 5, 10638\0"
	static const char FULLVERSION_STRING[] = "1.1.5.10638";
	
	//These values are to keep track of your versioning state, don't modify them.
	static const long BUILD_HISTORY = 128;
	

}
#endif //VERSION_H
