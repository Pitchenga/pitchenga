#!/bin/bash
set -euo pipefail

# Default values
artifactsDirectory=${1:-"cmake-build-release/Pitchenga_artefacts/Release"}
outputPackage=${2:-"Pitchenga-macOS-AppStore.pkg"}
version=${3:-"1.0.0"}
applicationIdentity=${4:-"Apple Distribution"}
installerIdentity=${5:-"3rd Party Mac Developer Installer"}

if [ "$version" == "1.0.0" ]; then
    version=$(bash version.sh)
fi

echo "--- Creating macOS App Store Package ---"
echo "Artifacts source:   $artifactsDirectory"
echo "Output Package:     $outputPackage"
echo "Version:            $version"
echo "App Identity:       $applicationIdentity"
echo "Installer Identity: $installerIdentity"

originalAppPath="$artifactsDirectory/Standalone/Pitchenga.app"

if [ ! -d "$originalAppPath" ]; then
    echo "Error: Standalone application not found at $originalAppPath"
    exit 1
fi

# Create a clean staging directory for MAS modifications
# This avoids permission issues with build artifacts
stagingDir="mas_staging"
rm -rf "$stagingDir"
mkdir -p "$stagingDir"

echo "Staging application for modification..."
cp -R "$originalAppPath" "$stagingDir/"
stagedAppPath="$stagingDir/Pitchenga.app"

# Ensure staged files are writable
chmod -R +w "$stagedAppPath"

# Find Info.plist (it might be missing in the bundle but present in JuceLibraryCode folder)
plistPath="$stagedAppPath/Contents/Info.plist"
if [ ! -f "$plistPath" ]; then
    echo "Warning: Info.plist not found in bundle. Searching for fallback..."
    # Attempt to find fallback in build artifacts
    fallbackPlist=$(find "$(dirname "$artifactsDirectory")" -name "Info.plist" | grep "Pitchenga_Standalone" | head -n 1)
    if [ -n "$fallbackPlist" ] && [ -f "$fallbackPlist" ]; then
        echo "Found fallback Info.plist at $fallbackPlist"
        mkdir -p "$(dirname "$plistPath")"
        cp "$fallbackPlist" "$plistPath"
    else
        echo "Error: Could not find Info.plist for Standalone app."
        exit 1
    fi
fi

# MAS requires App Sandbox
echo "Generating MAS entitlements..."
entitlementsPath="$stagingDir/mas.entitlements"
cat <<EOF > "$entitlementsPath"
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

echo "Injecting MAS metadata into Info.plist..."
plutil -replace CFBundleSupportedPlatforms -json '["MacOSX"]' "$plistPath"
plutil -replace LSApplicationCategoryType -string "public.app-category.music" "$plistPath"

# Ensure version matches the package
plutil -replace CFBundleVersion -string "$version" "$plistPath"
plutil -replace CFBundleShortVersionString -string "$version" "$plistPath"

echo "Signing app for Mac App Store..."
codesign --force --deep --options runtime --timestamp \
    --sign "$applicationIdentity" \
    --entitlements "$entitlementsPath" \
    "$stagedAppPath"

echo "Building store-bound package..."
# For MAS, we must ensure relocation is disabled to avoid "nothing installed" errors
# We use a temporary root folder pattern for pkgbuild
masRoot="$stagingDir/pkg_root"
mkdir -p "$masRoot/Applications"
cp -R "$stagedAppPath" "$masRoot/Applications/"

componentPlist="$stagingDir/component.plist"
pkgbuild --analyze --root "$masRoot" "$componentPlist"
sed -i '' 's/<key>BundleIsRelocatable<\/key>.*<true\/>/<key>BundleIsRelocatable<\/key><false\/>/' "$componentPlist"

# Build the component package using the temporary root
# Dynamically extract bundle identifier from Info.plist
bundleIdentifier=$(plutil -extract CFBundleIdentifier raw "$plistPath")

pkgbuild --root "$masRoot" \
    --identifier "$bundleIdentifier" \
    --install-location "/" \
    --version "$version" \
    --component-plist "$componentPlist" \
    "$stagingDir/component.pkg"

# Wrap it in a signed product package
productbuild --package "$stagingDir/component.pkg" \
    --sign "$installerIdentity" \
    "$outputPackage"

# Clean up
rm -rf "$stagingDir"

echo "Success! MAS Package created at $outputPackage"
