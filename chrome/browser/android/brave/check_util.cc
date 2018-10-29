 #include "check_util.h"
 #include "base/android/jni_android.h"
 #include "base/android/jni_string.h"
 #include "jni/CheckUtil_jni.h"

namespace check_util {

CheckUtil::CheckUtil() {
  JNIEnv* env = base::android::AttachCurrentThread();
  java_obj_.Reset(env, Java_CheckUtil_create(env, reinterpret_cast<intptr_t>(this)).obj());
}

CheckUtil::~CheckUtil() {
  Java_CheckUtil_destroy(base::android::AttachCurrentThread(), java_obj_);
}

bool CheckUtil::ClientAttestation(ClientAttestationCallback attest_callback) {
  attest_callback_ = std::move(attest_callback);
  JNIEnv* env = base::android::AttachCurrentThread();
  return Java_CheckUtil_clientAttestation(env, java_obj_);
}

void CheckUtil::ClientAttestationResult(JNIEnv* env, const base::android::JavaRef<jobject>& jobj, jboolean result) {
  bool res = result;
  std::move(attest_callback_).Run(res);  
}

}
