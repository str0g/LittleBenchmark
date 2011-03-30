#ifndef MYHASH_HPP_INCLUDED
#define MYHASH_HPP_INCLUDED

/***************************************************************
 * Name:      <Input name>.cpp/h
 * Purpose:   Code for Application <Name>
 * Author:    Łukasz Buśko (buskol.waw.pl@gmail.com)
 * Created:   <Input date>
 * Copyright: Łukasz Buśko (http://str0g.wordpress.com)
 * License:   GNU / General Public License
 **************************************************************/
//Headers
#define SHA512LEN 128
#include "Globals.h"
#ifndef COMPILE_WITH_CRYPTOPP
    #define COMPILE_WITH_CRYPTOPP 1
#endif

#if COMPILE_WITH_CRYPTOPP == 1

#define CRYPTOPP_ENABLE_NAMESPACE_WEAK 1
#include <string>
#if ( _WIN32 ||  _WIN64) || ( __WIN32__ || __WIN64__ )
    #include "md5.h"
    #include "rsa.h"
    #include "base64.h"
    #include "hex.h"
#else
    #include <cryptopp/md5.h>
    #include <cryptopp/rsa.h>
    #include <cryptopp/base64.h>
    #include "cryptopp/hex.h"
#endif

//Specials
//Globals Varuabels
/**
* Hash functions has been taken from http://nopaste.gamedev.pl/?id=5546
*/
namespace Hash
{
        //Hash data by MD5 algorhitm
        inline std::string MD5(const std::string &data)
        {
                std::string res;
                CryptoPP::Weak1::MD5 hash;
                byte digest[CryptoPP::Weak1::MD5::DIGESTSIZE];

                hash.CalculateDigest(digest, (byte*)data.c_str(), data.size());

                CryptoPP::HexEncoder encoder;
                encoder.Attach(new CryptoPP::StringSink(res));
                encoder.Put(digest, sizeof(digest));
                encoder.MessageEnd();

                return res;
        }

        //Hash data by SHA512 algorhitm
        inline std::string SHA512(const std::string &data)
        {
                std::string res;
                CryptoPP::SHA512 hash;

                CryptoPP::StringSource encoder(data, true,
                        new CryptoPP::HashFilter(hash, new CryptoPP::HexEncoder(new CryptoPP::StringSink(res))));

                return res;
        }
}
    #endif //COMPILE_WITH_CRYPTOPP
#endif // MYHASH_HPP_INCLUDED
