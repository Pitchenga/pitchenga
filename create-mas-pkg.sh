#!/bin/bash
set -e

# Default values
ARTIFACTS_DIR=${1:-"build/Pitchenga_artefacts/Release"}
OUTPUT_PKG=${2:-"Pitchenga-macOS-AppStore.pkg"}
VERSION=${3:-"1.0.0"}
APP_IDENTITY=${4:-"Apple Distribution"}
INSTALLER_IDENTITY=${5:-"Mac Installer Distribution"}

echo "--- Creating macOS App Store Package ---"
echo "Artifacts source:   $ARTIFACTS_DIR"
echo "Output PKG:         $OUTPUT_PKG"
echo "Version:            $VERSION"
echo "App Identity:       $APP_IDENTITY"
echo "Installer Identity: $INSTALLER_IDENTITY"

APP_PATH="$ARTIFACTS_DIR/Standalone/Pitchenga.app"

if [ ! -d "$APP_PATH" ]; then
    echo "Error: Standalone app not found at $APP_PATH"
    exit 1
fi

# MAS requires App Sandbox
echo "Generating MAS entitlements..."
cat <<EOF > mas.entitlements
<?xml version="1.0" encoding="UTF-8"?>
<!DOCTYPE plist PUBLIC "-//Apple//DTD PLIST 1.0//EN" "http://www.apple.com/DTDs/PropertyList-1.0.dtd">
<plist version="1.0">
<dict>
    <key>com.apple.security.app-sandbox</key>
    <true/>
    <key>com.apple.security.device.audio-input</key>
    <true/>
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

echo "Signing app for Mac App Store..."
codesign --force --deep --options runtime --timestamp \
    --sign "$APP_IDENTITY" \
    --entitlements mas.entitlements \
    "$APP_PATH"

echo "Building store-bound package..."
productbuild --component "$APP_PATH" /Applications \
    --sign "$INSTALLER_IDENTITY" \
    "$OUTPUT_PKG"

# Clean up
rm mas.entitlements

echo "Success! MAS Package created at $OUTPUT_PKG"
