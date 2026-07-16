/** HISE Moonbase Bridge

Copyright 2026 Christoph Hart

This file is not part of the open source HISE code base, but only accessible for Moonbase clients.

*/

#pragma once

#include <JuceHeader.h>

// Windows native crypto uses CNG/BCrypt. This removes the OpenSSL dependency,
#if defined(_WIN32) && defined(_MSC_VER)
#pragma comment(lib, "bcrypt.lib")
#endif

#if !USE_COPY_PROTECTION
#error "You need to define USE_COPY_PROTECTION=1 in your extra definitions"
#endif

#if !USE_SCRIPT_COPY_PROTECTION
#error "You need to define USE_SCRIPT_COPY_PROTECTION=1 in your extra definitions"
#endif

#if !HISE_USE_MOONBASE
#error "You need to define HISE_USE_MOONBASE=1 in your extra definitions"
#endif

// delete later lol...
#define HISE_USE_MOONBASE_WITH_HISE_MANAGED 0

#ifndef HISE_USE_MOONBASE_WITH_HISE_MANAGED
#error "You need to define this macro with either 0 or 1 to explicitely state whether this product uses the HISE Managed license"
#endif

/** Internal Moonbase configuration flags. */

#ifndef MOONBASE_CPP_VERSION
#define MOONBASE_CPP_VERSION "3.1.0"
#endif

#define MOONBASE_JUCE_HAS_AUDIO_PROCESSORS 1

#ifndef MOONBASE_DISABLE_CURL_TRANSPORT
#define MOONBASE_DISABLE_CURL_TRANSPORT 1
#endif

#ifndef MOONBASE_CRYPTO_NATIVE
#define MOONBASE_CRYPTO_NATIVE 1
#endif