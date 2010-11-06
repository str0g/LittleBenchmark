#ifndef VERSION_H
#define VERSION_H

namespace AutoVersion{
	
	//Date Version Types
	static const char DATE[] = "11";
	static const char MONTH[] = "10";
	static const char YEAR[] = "2010";
	static const char UBUNTU_VERSION_STYLE[] = "10.10";
	
	//Software Status
	static const char STATUS[] = "Alpha";
	static const char STATUS_SHORT[] = "a";
	
	//Standard Version Type
	static const long MAJOR = 1;
	static const long MINOR = 1;
	static const long BUILD = 2;
	static const long REVISION = 10680;
	
	//Miscellaneous Version Types
	static const long BUILDS_COUNT = 3482;
	#define RC_FILEVERSION 1,1,2,10680
	#define RC_FILEVERSION_STRING "1, 1, 2, 10680\0"
	static const char FULLVERSION_STRING[] = "1.1.2.10680";
	
	//These values are to keep track of your versioning state, don't modify them.
	static const long BUILD_HISTORY = 136;
	

}
#endif //VERSION_H
