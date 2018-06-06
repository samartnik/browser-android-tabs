package org.chromium.chrome.browser;

import android.support.test.espresso.Espresso;
import android.support.test.espresso.action.ViewActions;
import android.support.test.espresso.matcher.ViewMatchers;
import android.support.test.filters.SmallTest;
import org.chromium.base.Log;
import org.chromium.base.test.util.CommandLineFlags;
import org.chromium.base.test.util.Feature;
import org.chromium.chrome.browser.test.ScreenShooter;
import org.chromium.chrome.R;
import org.chromium.chrome.test.ChromeActivityTestRule;
import org.chromium.chrome.test.ChromeJUnit4ClassRunner;
import org.junit.After;
import org.junit.Assert;
import org.junit.Before;
import org.junit.Rule;
import org.junit.Test;
import org.junit.runner.RunWith;
import java.io.IOException;

/**
 * Instrumentation tests for Brave functionality.
 */
@RunWith(ChromeJUnit4ClassRunner.class)
@CommandLineFlags.Add({ChromeSwitches.DISABLE_FIRST_RUN_EXPERIENCE})
public class BraveTest {
    private static final String TAG = "BraveTest";

    @Rule
    public ChromeActivityTestRule<ChromeTabbedActivity> mActivityTestRule =
            new ChromeActivityTestRule<>(ChromeTabbedActivity.class);

    @Before
    public void setUp() throws InterruptedException {
        mActivityTestRule.startMainActivityFromLauncher();
    }

    /**
     * Trial test for Brave functionality.
     */
    @Test
    @SmallTest
    @Feature({"BraveTest"})
    public void testTrialBrave() throws IOException, InterruptedException {
        Espresso.onView(ViewMatchers.withId(R.id.brave_shields_button)).perform(ViewActions.click());
    }
}
