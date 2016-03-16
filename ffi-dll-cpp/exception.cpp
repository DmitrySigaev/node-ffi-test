/* exception.cpp
 * Copyright 2016 Dmitry Sigaev
 *
 * Released under the MIT license -- see MIT-LICENSE for details
 */
#include <stdio.h>
#include <string.h>

#include "defs.h"
#include "exception.h"

using namespace dnest;

Exception::Exception (const char *format, ...)
{
   va_list args;

   va_start(args, format);

   _init(format, args);
   _code = -1;
   
   va_end(args);
}

Exception::Exception ()
{
   _code = -1;
   snprintf(_message, sizeof(_message), "unknown error");
}

Exception::Exception (const Exception &other)
{
   _code = other._code;
   strncpy(_message, other._message, sizeof(_message));
}

const char * Exception::message ()
{
   return _message;
}

int Exception::code ()
{
   return _code;
}

void Exception::appendMessage (const char *format, ...)
{
   va_list args;

   va_start(args, format);
   
   char added_message[1024];
   vsnprintf(added_message, sizeof(added_message), format, args);
   strncat(_message, added_message, sizeof(_message));
   
   va_end(args);
}

void Exception::_init (const char *format, va_list args)
{
   vsnprintf(_message, sizeof(_message), format, args);
}

void Exception::_init (const char *prefix, const char *format, va_list args)
{
   char format_full[1024];

   snprintf(format_full, sizeof(format_full), "%s: %s", prefix, format);
   _init(format_full, args);
}

Exception * Exception::clone ()
{
   Exception *cloned = new Exception;
   _cloneTo(cloned);
   return cloned;
}

void Exception::_cloneTo (Exception *dest) const
{
   dest->_code = _code;
   strncpy(dest->_message, _message, sizeof(_message));
}

void Exception::throwSelf ()
{
   throw *this;
}

Exception::~Exception ()
{
}
