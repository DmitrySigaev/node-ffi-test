/* defs.h
 * Copyright 2016 Dmitry Sigaev
 *
 * Released under the MIT license -- see MIT-LICENSE for details
 */

#if !defined(__DEFS_H__)
#define __DEFS_H__

#if !defined(__max)
#define __max(a,b)  (((a) > (b)) ? (a) : (b))
#endif
#if !defined(__min)
#define __min(a,b)  (((a) < (b)) ? (a) : (b))
#endif
#if !defined(__min3)
#define __min3(a,b,c)  (__min(a, __min(b, c)))
#endif
#if !defined(__max3)
#define __max3(a,b,c)  (__max(a, __max(b, c)))
#endif
#if !defined(__sign)
#define __sign(a)  (a > 0 ? 1 : (a < 0 ? -1 : 0))
#endif

#define __swap(a, b, tmp) ((tmp) = (b), (b) = (a), (a) = (tmp))

#if defined(_WIN32) && !defined(__MINGW32__)
//#define vsnprintf _vsnprintf
#define snprintf _snprintf
#define strcasecmp _stricmp
#define strncasecmp _strnicmp
#endif

#ifndef M_PI
#define M_PI       3.14159265358979323846
#endif

#define NELEM(arr) ((int)(sizeof(arr) / sizeof(arr[0])))

#ifndef dword
typedef unsigned int dword;
#endif
#ifndef __byte_typedef__
#define __byte_typedef__
typedef unsigned char byte;
#endif

#ifndef EXPORT_SYMBOL
   #ifdef _WIN32
      #define EXPORT_SYMBOL __declspec(dllexport)
   #elif (defined __GNUC__ || defined __APPLE__)
      #define EXPORT_SYMBOL __attribute__ ((visibility ("default")))
   #else
      #define EXPORT_SYMBOL
   #endif
#endif

#ifndef DLLEXPORT
   #ifdef _WIN32
      #ifdef DATANEST_PLUGIN
         #define DLLEXPORT __declspec(dllimport)
      #else
         #define DLLEXPORT EXPORT_SYMBOL
      #endif
   #else
      #define DLLEXPORT EXPORT_SYMBOL
   #endif
#endif

#ifndef CEXPORT
   #ifndef __cplusplus
      #define CEXPORT EXPORT_SYMBOL
   #else
      #define CEXPORT extern "C" EXPORT_SYMBOL
   #endif
#endif

#if defined(_WIN32)  && !defined(__MINGW32__)
#define qword unsigned _int64
#else
#define qword unsigned long long
#endif

typedef unsigned short word;

#if !defined(NULL)
#define NULL 0L
#endif

#endif /* !defined(__DEFS_H__) */
