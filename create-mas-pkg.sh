#!/bin/bash
set -euo pipefail

# Default values
artifactsDirectory=${1:-"cmake-build-release/Pitchenga_artefacts/Release"}
outputPackage=${2:-"Pitchenga-macOS-AppStore.pkg"}
version=${3:-"1.0.0"}
applicationIdentity=${4:-"Apple Distribution"}
installerIdentity=${5:-"Mac Installer Distribution"}

echo "--- Creating macOS App Store Package ---"
echo "Artifacts source:   $artifactsDirectory"
echo "Output Package:     $outputPackage"
echo "Version:            $version"
echo "App Identity:       $applicationIdentity"
echo "Installer Identity: $installerIdentity"

applicationPath="$artifactsDirectory/Standalone/Pitchenga.app"

if [ ! -d "$applicationPath" ]; then
    echo "Error: Standalone application not found at $applicationPath"
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
codesign --force --options runtime --timestamp \
    --sign "$applicationIdentity" \
    --entitlements mas.entitlements \
    "$applicationPath"

echo "Building store-bound package..."
productbuild --component "$APP_PATH" /Applications \
    --sign "$INSTALLER_IDENTITY" \
    "$OUTPUT_PKG"

# Clean up
rm mas.entitlements

echo "Success! MAS Package created at $OUTPUT_PKG"
