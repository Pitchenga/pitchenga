#!/bin/bash
set -euo pipefail

# Load environment variables if .env exists
if [ -f .env ]; then
    export $(grep -v '^#' .env | xargs)
fi

# Default values
artifactsDirectory=${1:-"cmake-build-release/Pitchenga_artefacts/Release"}
outputPackage=${2:-"Pitchenga-macOS-AppStore.pkg"}
version=${3:-"1.0.0"}
bundleIdentifier=${4:-"${MAC_APP_BUNDLE_ID:-}"}
applicationIdentity=${5:-"Apple Distribution"}
installerIdentity=${6:-"3rd Party Mac Developer Installer"}

if [ "$version" == "1.0.0" ]; then
    version=$(bash version.sh)
fi

echo "--- Creating macOS App Store Package ---"
echo "Artifacts source:   $artifactsDirectory"
echo "Output Package:     $outputPackage"
echo "Version:            $version"
echo "Bundle ID:          $bundleIdentifier"
echo "App Identity:       $applicationIdentity"
echo "Installer Identity: $installerIdentity"

if [ -z "$bundleIdentifier" ]; then
    echo "Error: bundleIdentifier must be provided."
    exit 1
fi

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
    <key>com.apple.security.screen-capture</key>
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

echo "Injecting MAS metadata into Info.plist..."
plutil -replace CFBundleSupportedPlatforms -json '["MacOSX"]' "$plistPath"
plutil -replace LSApplicationCategoryType -string "public.app-category.music" "$plistPath"
plutil -replace LSMinimumSystemVersion -string "10.15" "$plistPath"

# Bypass App Store Connect Export Compliance prompt
plutil -replace ITSAppUsesNonExemptEncryption -bool false "$plistPath"

# Set the Bundle Identifier
plutil -replace CFBundleIdentifier -string "$bundleIdentifier" "$plistPath"

# Ensure version matches the package
plutil -replace CFBundleVersion -string "$version" "$plistPath"
plutil -replace CFBundleShortVersionString -string "$version" "$plistPath"

echo "Using Bundle Identifier: $(plutil -extract CFBundleIdentifier raw "$plistPath")"

echo "Embedding Provisioning Profile..."
profilePath=".macprovisionprofile"
if [ -f "$profilePath" ]; then
    cp "$profilePath" "$stagedAppPath/Contents/embedded.provisionprofile"
    
    # Extract identifiers from profile to inject into entitlements
    echo "Extracting profile entitlements..."
    if ! security cms -D -i "$profilePath" > "$stagingDir/profile.plist" 2>/dev/null; then
        echo "ERROR: Failed to read .macprovisionprofile. The file is likely corrupted (e.g., saved as text instead of binary)."
        echo "Please re-download the profile directly from Apple Developer and DO NOT open/copy-paste its contents."
        exit 1
    fi
    
    # Note: Plutil requires escaping dots in key paths
    appIdentifier=$(plutil -extract 'Entitlements.com\.apple\.application-identifier' raw "$stagingDir/profile.plist" 2>/dev/null || echo "")
    teamIdentifier=$(plutil -extract 'Entitlements.com\.apple\.developer\.team-identifier' raw "$stagingDir/profile.plist" 2>/dev/null || echo "")
    
    if [ -n "$appIdentifier" ] && [ "$appIdentifier" != "No value" ]; then
        plutil -insert 'com\.apple\.application-identifier' -string "$appIdentifier" "$entitlementsPath" || true
    fi
    if [ -n "$teamIdentifier" ] && [ "$teamIdentifier" != "No value" ]; then
        plutil -insert 'com\.apple\.developer\.team-identifier' -string "$teamIdentifier" "$entitlementsPath" || true
    fi
else
    echo "ERROR: No provisioning profile found at $profilePath."
    echo "TestFlight requires an embedded.provisionprofile for Mac App Store builds."
    exit 1
fi

echo "Removing extended attributes (like quarantine)..."
xattr -rc "$stagedAppPath"

echo "Signing app for Mac App Store..."
codesign --force --deep --options runtime --timestamp \
    --identifier "$bundleIdentifier" \
    --sign "$applicationIdentity" \
    --entitlements "$entitlementsPath" \
    "$stagedAppPath"

echo "Building store-bound package..."
# App Store Connect strictly requires the metadata generated by productbuild --component.
# (The pkgbuild/Distribution method strips product metadata and OS versions).
productbuild --component "$stagedAppPath" /Applications \
    --sign "$installerIdentity" \
    --version "$version" \
    "$outputPackage"

# Clean up
rm -rf "$stagingDir"

echo "Success! MAS Package created at $outputPackage"
