/* tmp-thread-obj.h
* Copyright 2016 Dmitry Sigaev
*
* Released under the MIT license -- see MIT-LICENSE for details
*/

#if !defined(__TMP_THREAD_OBJ_H__)
#define __TMP_THREAD_OBJ_H__

#include <unordered_map>
#include <chrono>
#include <thread>
#include <memory>
#include <mutex>

#ifdef _WIN32
#pragma warning(push)
#pragma warning(disable:4251)
#endif

namespace dnest {

//
// Various DataNest API methods returns pointers to the temporary buffers
// This leads to an issues when multiple threads gets the same pointers
// To avoid this we returns a temporary object that belongs to the current
// thread for a short time (10 sec).
//
template <typename T>
class TemporaryThreadObjManager
{
public:
   // This method return a temporary object for this thread that lives at least 10 seconds.
   // Method is synchronized
   T& getObject ();

private:
   typedef std::chrono::steady_clock steady_clock;
   struct Container
   {
      Container();
      Container(Container&& that);

      steady_clock::time_point timestamp;
      std::unique_ptr<T> obj;
   };

   typedef std::unordered_map<std::thread::id, Container> Map;
   Map _objects;

   std::mutex access_mutex;
};

#include "tmp-thread-obj.hpp"

}

#ifdef _WIN32
#pragma warning(pop)
#endif

#endif /* defined(__TMP_THREAD_OBJ_H__) */
