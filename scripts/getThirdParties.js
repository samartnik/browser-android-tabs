cp -f scripts/.gclient ../.gclient
cp -f scripts/.gclient_entries ../.gclient_entries
gclient sync --with_branch_heads
cd ..
echo "{ 'GYP_DEFINES': 'OS=android target_arch=arm buildtype=Official', }" > chromium.gyp_env
gclient runhooks
cd src
build/install-build-deps-android.sh
gclient sync
sh . build/android/envsetup.sh
sh scripts/postThirdPartiesSetup.js
npm install
npm run web-ui
npm run brave-rewards-panel-web-ui
gn args out/Default
