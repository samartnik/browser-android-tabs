#ifndef CHECK_UTIL_H_
#define CHECK_UTIL_H_

#include "base/android/scoped_java_ref.h"
#include "net/base/completion_once_callback.h"

#include <jni.h>
#include <string>

namespace check_util {

using ClientAttestationCallback = base::OnceCallback<void(bool)>;

class CheckUtil {
  public:
    CheckUtil();
    ~CheckUtil();
    // Performs client attestation, called from C++
    bool ClientAttestation(ClientAttestationCallback attest_callback);
    // Callback returns client attestation final result, called from Java
    void ClientAttestationResult(JNIEnv* env, const base::android::JavaRef<jobject>& jobj, jboolean result);
  private:
    base::android::ScopedJavaGlobalRef<jobject> java_obj_;
    ClientAttestationCallback attest_callback_;
};

}

#endif //CHECK_UTIL_H_
