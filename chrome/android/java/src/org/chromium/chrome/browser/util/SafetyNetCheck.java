
package org.chromium.chrome.browser.util;

import android.app.Activity;
import android.content.Context;
import android.content.SharedPreferences;
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
import java.nio.charset.Charset;
import java.security.SecureRandom;
import java.util.Arrays;
import java.util.Random;

import org.chromium.base.ApplicationStatus;
import org.chromium.base.annotations.CalledByNative;
import org.chromium.base.annotations.JNINamespace;
import org.chromium.base.ContextUtils;
import org.chromium.base.Log;
import org.chromium.chrome.browser.ConfigAPIs;
import org.chromium.chrome.browser.preferences.PrefServiceBridge;

/**
 * Utility class for providing additional safety checks.
 */
@JNINamespace("safetynet_check")
public class SafetyNetCheck {
    private static final String TAG = "SafetyNetCheck";
    public static final String PREF_NAME = "SafetyNet";
    public static final String PREF_SHOULD_RUN_NAME = "SafetyNetShouldRun";

    private long mNativeSafetyNetCheck;

    // For usage within Java
    public SafetyNetCheck() {
        mNativeSafetyNetCheck = 0;
    }

    private SafetyNetCheck(long staticSafetyNetCheck) {
        mNativeSafetyNetCheck = staticSafetyNetCheck;
    }

    @CalledByNative
    private static SafetyNetCheck create(long staticSafetyNetCheck) {
        return new SafetyNetCheck(staticSafetyNetCheck);
    }

    @CalledByNative
    private void destroy() {
        assert mNativeSafetyNetCheck != 0;
        mNativeSafetyNetCheck = 0;
    }

    /**
    * Performs client attestation
    */
    @CalledByNative
    public boolean clientAttestation(String nonceData) {
        Log.i(TAG, "SAM: clientAttestation");
        boolean res = false;
        try {
            Activity activity = ApplicationStatus.getLastTrackedFocusedActivity();
            if (activity == null) return false;
            if (GoogleApiAvailability.getInstance().isGooglePlayServicesAvailable(activity) == ConnectionResult.SUCCESS) {
                byte[] nonce = nonceData.isEmpty() ? getRequestNonce() : nonceData.getBytes();
                SafetyNetClient client = SafetyNet.getClient(activity);
                Task<SafetyNetApi.AttestationResponse> attestTask = client.attest(nonce, ConfigAPIs.GS_API_KEY);                
                attestTask.addOnSuccessListener(activity,
                    new OnSuccessListener<SafetyNetApi.AttestationResponse>() {
                        @Override
                        public void onSuccess(SafetyNetApi.AttestationResponse response) {
                            clientAttestationResult(true, response.getJwsResult(), false);
                        }
                    }).addOnFailureListener(activity, new OnFailureListener() {
                        @Override
                        public void onFailure(@NonNull Exception e) {
                            Log.e(TAG, "SAM: Failed to perform SafetyNetCheck: " + e);
                            clientAttestationResult(false, e.toString(), e instanceof ApiException);
                        }
                    });
                res = true;
            }
        } catch (Exception e) {
            Log.e(TAG, "SAM: SafetyNetCheck error: " + e);
        }
        return res;
    }

    /**
    * Generates a random 24-byte nonce.
    */
    private byte[] getRequestNonce() {
        byte[] bytes = new byte[24];
        Random random = new SecureRandom();
        random.nextBytes(bytes);
        return bytes;
    }

    /**
    * Returns client attestation final result
    */
    private void clientAttestationResult(boolean result, String resultString, boolean isApiException) {
        Log.i(TAG, "SAM: clientAttestationResult: " + result + ", " + resultString);
        if (mNativeSafetyNetCheck == 0) {
            if (!isApiException) {
                // For Java calls just save the last result
                SharedPreferences sharedPref = ContextUtils.getApplicationContext().getSharedPreferences(PREF_NAME, 0);
                SharedPreferences.Editor editor = sharedPref.edit();
                editor.putBoolean(PREF_SHOULD_RUN_NAME, false);
                editor.apply();
                PrefServiceBridge.getInstance().setSafetyNetResult(result);
            }
            return;
        }
        nativeclientAttestationResult(mNativeSafetyNetCheck, result, resultString);
    }

    public static boolean isSuccessful() {
        return PrefServiceBridge.getInstance().isSafetyNetResultSuccessful();
    }

    private native void nativeclientAttestationResult(long nativeSafetyNetCheck, boolean result, String resultString);
}