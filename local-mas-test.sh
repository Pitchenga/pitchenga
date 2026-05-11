#!/bin/bash
set -e

# Configuration
APP_PATH="cmake-build-release/Pitchenga_artefacts/Release/Standalone/Pitchenga.app"
TEST_ENTITLEMENTS=".local-mas-test.entitlements"

# Verify build exists
if [ ! -d "$APP_PATH" ]; then
    echo "❌ Error: Release build not found at $APP_PATH"
    echo "Please run the Release build in CLion first."
    exit 1
fi

echo "--- Local macOS Sandbox Test ---"

# Generate minimal test entitlements (No Team ID requirements to avoid launch failures)
# This uses the exact same folder exception as the production MAS build.
cat <<EOF > "$TEST_ENTITLEMENTS"
<?xml version="1.0" encoding="UTF-8"?>
<!DOCTYPE plist PUBLIC "-//Apple//DTD PLIST 1.0//EN" "http://www.apple.com/DTDs/PropertyList-1.0.dtd">
<plist version="1.0">
<dict>
    <key>com.apple.security.app-sandbox</key>
    <true/>
    <key>com.apple.security.device.audio-input</key>
    <true/>
    <key>com.apple.security.files.user-selected.read-write</key>
    <true/>
    <key>com.apple.security.temporary-exception.files.home-relative-path.read-write</key>
    <array>
        <string>Library/Pitchenga/</string>
    </array>
</dict>
</plist>
EOF

# Perform the sandboxed signing
# We use "Apple Development" which is safe for local execution.
echo "📦 Signing application with local sandbox entitlements..."
codesign --force --deep --sign "Apple Development" --entitlements "$TEST_ENTITLEMENTS" "$APP_PATH"

# Launch
echo "🚀 Launching sandboxed app..."
if open "$APP_PATH"; then
    echo "✅ Success! The app is now running in a local sandbox."
    echo "Check if your presets are visible."
else
    echo "❌ Failed to launch application."
fi

# Clean up
rm "$TEST_ENTITLEMENTS"
