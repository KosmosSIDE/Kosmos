#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <stdio.h>
#include <stdlib.h>

#define LABSETUP 1
#ifdef LABSETUP
const std::string PATH = "E:/Shared/labsvn/aszgard/szg/vrclass/";
const std::string TEMPLATEPATH = "E:/Shared/labsvn/aszgard/szg/vrclass/Kosmos/";
const std::string ROOTPATH = "E:/Shared/labsvn/aszgard/szg/vrclass/";
const std::string SANDBOXPATH = "E:/Shared/labsvn/aszgard/szg/vrclass/sandbox/";
const std::string OBJECTPATH = "E:/Shared/labsvn/aszgard/szg/vrclass/";
#else
const std::string PATH = "C:\\aszgard5\\szg\\projects\\";
const std::string TEMPLATEPATH = "c:\\aszgard5\\szg\\projects\\Kosmos\\";
const std::string ROOTPATH = "C:\\";
const std::string SANDBOXPATH = "c:\\aszgard5\\szg\\projects\\sandbox\\";
const std::string OBJECTPATH = "c:\\obj\\";
#endif


#endif

