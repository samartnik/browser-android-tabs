
package org.chromium.chrome.browser.util;

import android.app.Activity;
import android.content.Context;
import android.support.annotation.NonNull;


import com.google.android.gms.common.api.ApiException;
import com.google.android.gms.common.ConnectionResult;
import com.google.android.gms.common.GoogleApiAvailability;
import com.google.android.gms.safetynet.SafetyNet;
import com.google.android.gms.safetynet.SafetyNetApi;
import com.google.android.gms.safetynet.SafetyNetClient;
import com.google.android.gms.tasks.OnCompleteListener;
import com.google.android.gms.tasks.OnFailureListener;
import com.google.android.gms.tasks.OnSuccessListener;
import com.google.android.gms.tasks.Task;

import java.io.ByteArrayOutputStream;
import java.io.IOException;
import java.security.SecureRandom;
import java.util.Arrays;
import java.util.Random;

import org.chromium.base.ApplicationStatus;
import org.chromium.base.annotations.CalledByNative;
import org.chromium.base.annotations.JNINamespace;
import org.chromium.base.ContextUtils;
import org.chromium.base.Log;
import org.chromium.chrome.browser.ConfigAPIs;

/**
 * Utility class for providing additional safety checks.
 */
@JNINamespace("check_util")
public class CheckUtil {
    private static final String TAG = "CheckUtil";

    private long mNativeCheckUtil;

    private CheckUtil(long staticCheckUtil) {
        mNativeCheckUtil = staticCheckUtil;
    }

    @CalledByNative
    private static CheckUtil create(long staticCheckUtil) {
        return new CheckUtil(staticCheckUtil);
    }

    @CalledByNative
    private void destroy() {
        assert mNativeCheckUtil != 0;
        mNativeCheckUtil = 0;
    }

    /**
    * Performs client attestation
    */
    @CalledByNative
    public boolean clientAttestation() {
        boolean res = false;
        try {
            Log.i(TAG, "clientAttestation testing");
            Activity activity = ApplicationStatus.getLastTrackedFocusedActivity();
            if (activity == null) return false;
            if (GoogleApiAvailability.getInstance().isGooglePlayServicesAvailable(activity) == ConnectionResult.SUCCESS) {
                String nonceData = "brave_nonce" + System.currentTimeMillis();
                byte[] nonce = getRequestNonce(nonceData);
                Log.i(TAG, "SAM: nonce: " + Arrays.toString(nonce));                
                Log.i(TAG, "SAM: GS_API_KEY: " + ConfigAPIs.GS_API_KEY);
                SafetyNetClient client = SafetyNet.getClient(activity);
                Task<SafetyNetApi.AttestationResponse> attestTask = client.attest(nonce, ConfigAPIs.GS_API_KEY);                
                attestTask.addOnSuccessListener(activity,
                    new OnSuccessListener<SafetyNetApi.AttestationResponse>() {
                        @Override
                        public void onSuccess(SafetyNetApi.AttestationResponse response) {
                            // TODO: send response.getJwsResult() to server and get result
                            Log.i(TAG, "SAM: jws: " + response.getJwsResult().length());
                            Log.i(TAG, "SAM: jws1: " + response.getJwsResult().substring(0, response.getJwsResult().length() / 2));
                            Log.i(TAG, "SAM: jws2: " + response.getJwsResult().substring(response.getJwsResult().length() / 2));
                            clientAttestationResult(true);
                        }
                    }).addOnFailureListener(activity, new OnFailureListener() {
                        @Override
                        public void onFailure(@NonNull Exception e) {
                            Log.e(TAG, "Failed to perform check: " + e);
                            clientAttestationResult(false);
                        }
                    });
                res = true;
            }
        } catch (Exception e) {
            Log.e(TAG, "SAM: Exception: " + e);
        }
        Log.i(TAG, "SAM: clientAttestation result: " + res);
        return res;
    }

    /**
    * Generates a 16-byte nonce with additional data.
    */
    private byte[] getRequestNonce(String data) {
        ByteArrayOutputStream byteStream = new ByteArrayOutputStream();
        byte[] bytes = new byte[24];
        Random random = new SecureRandom();
        random.nextBytes(bytes);
        try {
            byteStream.write(bytes);
            byteStream.write(data.getBytes());
        } catch (IOException e) {
            return null;
        }

        return byteStream.toByteArray();
    }

    /**
    * Returns client attestation final result
    */
    private void clientAttestationResult(boolean result) {
        if (mNativeCheckUtil == 0) return;
        nativeClientAttestationResult(mNativeCheckUtil, result);
    }

    private native void nativeClientAttestationResult(long nativeCheckUtil, boolean result);
}