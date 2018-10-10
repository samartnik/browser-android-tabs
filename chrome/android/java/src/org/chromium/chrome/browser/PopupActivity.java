// Copyright 2017 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

package org.chromium.chrome.browser;

import android.content.ComponentName;
import android.content.Intent;
import android.provider.Browser;
import android.util.SparseArray;
import android.view.View;
import android.view.ViewGroup;

import org.chromium.base.ActivityState;
import org.chromium.base.ApplicationStatus;
import org.chromium.base.Log;
import org.chromium.chrome.R;
import org.chromium.chrome.browser.compositor.layouts.LayoutManager;
import org.chromium.chrome.browser.fullscreen.ChromeFullscreenManager;
import org.chromium.chrome.browser.fullscreen.FullscreenOptions;
import org.chromium.chrome.browser.tab.Tab;
import org.chromium.chrome.browser.util.IntentUtils;
import org.chromium.chrome.browser.widget.ControlContainer;
import org.chromium.content.browser.ScreenOrientationProvider;
import org.chromium.content_public.browser.WebContentsObserver;

import android.view.WindowManager;
import android.graphics.Point;
import android.view.Display;
import org.chromium.content_public.browser.LoadUrlParams;
import android.view.MotionEvent;
import android.graphics.Color;
import android.view.ViewParent;
import org.chromium.chrome.browser.tabmodel.TabModel;
import org.chromium.ui.base.PageTransition;
import android.graphics.PixelFormat;
import android.graphics.drawable.ColorDrawable;

/**
 * An Activity used to display popup html page.
 */
public class PopupActivity extends SingleTabActivity {
    private static final String TAG = "PopupActivity";

    @Override
    public void postInflationStartup() {
        super.postInflationStartup();

        WindowManager.LayoutParams windowParams = getWindow().getAttributes();
        windowParams.height = WindowManager.LayoutParams.WRAP_CONTENT;
        windowParams.width = WindowManager.LayoutParams.WRAP_CONTENT;
        windowParams.gravity = android.view.Gravity.END/* | android.view.Gravity.AXIS_CLIP*/;
        //windowParams.y = 100;
        //windowParams.flags = WindowManager.LayoutParams.FLAG_FORCE_NOT_FULLSCREEN;
        //windowParams.alpha = 0f;
        getWindow().setAttributes(windowParams);
        getWindow().setFormat(PixelFormat.TRANSLUCENT);
        getWindow().setBackgroundDrawable(getResources().getDrawable(R.drawable.rounded_activity));

        ViewGroup rootView = (ViewGroup) getWindow().getDecorView().getRootView();
        if (rootView != null) {
            rootView.setOnTouchListener(new View.OnTouchListener()
            {
                @Override
                public boolean onTouch(View view, MotionEvent ev)
                {
                    finish();
                    return false;
                }
            });
            //setTransparentBackground(getWindow().getDecorView());
            ViewGroup compositor = getCompositorViewHolder().getCompositorView();
            compositor.setBackgroundDrawable(getResources().getDrawable(R.drawable.rounded_activity));
            compositor.setContentDescription(getResources().getString(R.string.accessibility_content_view) + "1");
            if (compositor.getChildCount() > 0) {
                View child = compositor.getChildAt(0);
                child.setContentDescription(getResources().getString(R.string.accessibility_content_view) + "2");
                Log.i(TAG, "SAM: view: " + child.getClass().getName());
                ViewGroup.LayoutParams params = child.getLayoutParams();
                params.height = WindowManager.LayoutParams.WRAP_CONTENT;
                params.width = WindowManager.LayoutParams.WRAP_CONTENT;
                if (params instanceof ViewGroup.MarginLayoutParams) {
                    Log.i(TAG, "SAM: setMargins");
                    ((ViewGroup.MarginLayoutParams)params).setMargins(10, 100, 10, 10);
                }
                child.setLayoutParams(params);
            }
            /*for (int i = 0; i < compositor.getChildCount(); i++) {
                View child = compositor.getChildAt(i);
                Log.i(TAG, "SAM: view: " + child.getClass().getName());
            }*/
            /*ViewGroup.LayoutParams params = compositor.getLayoutParams();
            params.height = WindowManager.LayoutParams.WRAP_CONTENT;
            params.width = WindowManager.LayoutParams.WRAP_CONTENT;
            if (params instanceof ViewGroup.MarginLayoutParams) {
                ((ViewGroup.MarginLayoutParams)params).setMargins(10, 100, 10, 10);
            }
            compositor.setLayoutParams(params);*/
        }
        int i = R.drawable.rounded_activity;
    }

    private void setTransparentBackground(View v) {
        v.setBackgroundColor(Color.TRANSPARENT);
        if (v instanceof ViewGroup) {
            ViewGroup vg = (ViewGroup) v;
            for (int i = 0; i < vg.getChildCount(); i++) {
                View child = vg.getChildAt(i);
                setTransparentBackground(child);
            }
        }
    }

    @Override
    protected Tab createTab() {
        Tab tab = new Tab(Tab.INVALID_TAB_ID, Tab.INVALID_TAB_ID, false, getWindowAndroid(),
                            TabModel.TabLaunchType.FROM_CHROME_UI, null, null);

        tab.initialize(null, getTabContentManager(), createTabDelegateFactory(), false, false);
        tab.loadUrl(new LoadUrlParams("chrome://rewards"));
        return tab;
    }

    public static void show(ChromeActivity activity/*final Tab tab*/) {
        //ChromeActivity activity = tab.getActivity();

        //sTabsToSteal.put(tab.getId(), tab);

        Intent intent = new Intent();
        intent.setClass(activity, PopupActivity.class);
        //intent.putExtra(IntentHandler.EXTRA_TAB_ID, tab.getId());
        intent.putExtra(IntentHandler.EXTRA_PARENT_COMPONENT, activity.getComponentName());
        intent.putExtra(Browser.EXTRA_APPLICATION_ID, activity.getPackageName());
        intent.addFlags(Intent.FLAG_ACTIVITY_MULTIPLE_TASK);

        activity.startActivity(intent);
    }

    @Override
    protected void initializeToolbar() {}

    @Override
    protected int getControlContainerLayoutId() {
        return R.layout.fullscreen_control_container;
    }

    @Override
    public int getControlContainerHeightResource() {
        return R.dimen.fullscreen_activity_control_container_height;
    }

    @Override
    public void finishNativeInitialization() {
        ControlContainer controlContainer = (ControlContainer) findViewById(R.id.control_container);
        initializeCompositorContent(new LayoutManager(getCompositorViewHolder()),
                (View) controlContainer, (ViewGroup) findViewById(android.R.id.content),
                controlContainer);

        if (getFullscreenManager() != null) getFullscreenManager().setTab(getActivityTab());
        super.finishNativeInitialization();
    }
}
