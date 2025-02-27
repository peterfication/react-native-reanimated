#pragma once

#ifndef NDEBUG

#include <cxxabi.h>

#include <iostream>
#include <string>

#ifdef ANDROID
#include <android/log.h>
#endif

namespace reanimated {

// This is a class that counts how many instances of a different class there
// are. It is meant only to be used with classes that should only have one
// instance.

template <class T>
class SingleInstanceChecker {
 public:
  SingleInstanceChecker();
  ~SingleInstanceChecker();

 private:
  void assertWithMessage(bool condition, std::string message) {
    if (!condition) {
#ifdef ANDROID
      __android_log_print(
          ANDROID_LOG_WARN, "Reanimated", "%s", message.c_str());
#else
      std::cerr << "[Reanimated] " << message << std::endl;
#endif

#ifdef IS_REANIMATED_EXAMPLE_APP
      assert(false);
#endif
    }
  }

  // A static field will exist separately for every class template.
  // This has to be inline for automatic initialization.
  inline static volatile int instanceCount_;
};

template <class T>
SingleInstanceChecker<T>::SingleInstanceChecker() {
  int status = 0;
  std::string className =
      __cxxabiv1::__cxa_demangle(typeid(T).name(), nullptr, nullptr, &status);

  // Only one instance should exist, but it is possible for two instances
  // to co-exist during a reload.
  assertWithMessage(
      instanceCount_ <= 1,
      "[Reanimated] More than one instance of " + className +
          " present. This may indicate a memory leak due to a retain cycle.");

  instanceCount_++;
}

template <class T>
SingleInstanceChecker<T>::~SingleInstanceChecker() {
  instanceCount_--;
}

} // namespace reanimated

#endif // NDEBUG
