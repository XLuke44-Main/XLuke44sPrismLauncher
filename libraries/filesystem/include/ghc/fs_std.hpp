
// Copyright (c) 2018, Steffen Schümann <s.schuemann@pobox.com>

// Permission is hereby granted, free of charge, to any person obtaining a copy

// The above copyright notice and this permission notice shall be included in all

// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR

// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,

#ifndef GHC_FILESYSTEM_STD_H
#define GHC_FILESYSTEM_STD_H

#if _MSVC_LANG >= 201703L || __cplusplus >= 201703L && defined(__has_include)

    #if __has_include(<filesystem>)  
        #define GHC_USE_STD_FS

        #ifdef __APPLE__
            #include <Availability.h>

            #if __MAC_OS_X_VERSION_MIN_REQUIRED && __MAC_OS_X_VERSION_MIN_REQUIRED < 101500 \
             || __IPHONE_OS_VERSION_MIN_REQUIRED && __IPHONE_OS_VERSION_MIN_REQUIRED < 130000 \
             || __TV_OS_VERSION_MIN_REQUIRED && __TV_OS_VERSION_MIN_REQUIRED < 130000 \
             || __WATCH_OS_VERSION_MAX_ALLOWED && __WATCH_OS_VERSION_MAX_ALLOWED < 60000
                #undef GHC_USE_STD_FS
            #endif  
        #endif
    #endif
#endif

#ifdef GHC_USE_STD_FS
    #include <filesystem>
    namespace fs {
        using namespace std::filesystem;
        using ifstream = std::ifstream;
        using ofstream = std::ofstream;
        using fstream = std::fstream;
    }
#else
    #include "filesystem.hpp"
    namespace fs {
        using namespace ghc::filesystem;
        using ifstream = ghc::filesystem::ifstream;
        using ofstream = ghc::filesystem::ofstream;
        using fstream = ghc::filesystem::fstream;
    }
#endif

#endif  
