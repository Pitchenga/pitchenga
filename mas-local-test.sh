#!/bin/bash
set -e

# Run the build
cmake --build cmake-build-release

# Configuration
APP_PATH="cmake-build-release/Pitchenga_artefacts/Release/Standalone/Pitchenga.app"
TEST_ENTITLEMENTS=".local-mas-test.entitlements"
MODE=${1:-"manual"}

# Verify build exists
if [ ! -d "$APP_PATH" ]; then
    echo "❌ Error: Release build not found at $APP_PATH"
    exit 1
fi

# Reset TCC for the clean bundle ID to ensure a fresh test
tccutil reset All com.github.pitchenga.Pitchenga || true

echo "--- Local macOS Sandbox Test (Mode: $MODE) ---"

# Generate minimal test entitlements (No Team ID requirements to avoid launch failures)
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
    <key>com.apple.security.temporary-exception.audio-unit-host</key>
    <true/>
    <key>com.apple.security.temporary-exception.files.home-relative-path.read-write</key>
    <array>
        <string>/Library/Pitchenga/</string>
    </array>
    <key>com.apple.security.cs.disable-library-validation</key>
    <true/>
    <key>com.apple.security.cs.allow-jit</key>
    <true/>
    <key>com.apple.security.cs.allow-unsigned-executable-memory</key>
    <true/>
    <key>com.apple.security.cs.disable-executable-page-protection</key>
    <true/>
</dict>
</plist>
EOF

# Perform the sandboxed signing
echo "📦 Signing application with local sandbox entitlements..."
codesign --force --deep --sign "Apple Development" --entitlements "$TEST_ENTITLEMENTS" "$APP_PATH"

if [ "$MODE" == "--ci" ]; then
    # CI Mode: Run directly, capture logs, and kill
    echo "🚀 Launching sandboxed app binary directly (CI Mode)..."
    BINARY="$APP_PATH/Contents/MacOS/Pitchenga"
    "$BINARY" &
    PID=$!
    sleep 5
    kill $PID || true
    echo "✅ Finished capturing logs."
else
    # Manual Mode: Just open it
    echo "🚀 Launching sandboxed app..."
    if open "$APP_PATH"; then
        echo "✅ Success! The app is now running in a local sandbox."
    else
        echo "❌ Failed to launch application."
    fi
fi

# Clean up
rm "$TEST_ENTITLEMENTS"
