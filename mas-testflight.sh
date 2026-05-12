#!/bin/bash
set -e

# Load environment variables if .env exists
if [ -f .env ]; then
    export $(grep -v '^#' .env | xargs)
fi

# Configuration
export ARTIFACTS_DIR="${ARTIFACTS_DIR:-cmake-build-release/Pitchenga_artefacts/Release}"
VERSION=${1:-"1.0.0"}

if [ "$VERSION" == "1.0.0" ]; then
    # Try to extract version from the built app's Info.plist first
    PLIST_PATH="$ARTIFACTS_DIR/Standalone/Pitchenga.app/Contents/Info.plist"
    if [ -f "$PLIST_PATH" ]; then
        VERSION=$(/usr/bin/plutil -extract CFBundleVersion raw "$PLIST_PATH")
        printf "\nDetected version from build artifacts: $VERSION\n"
    else
        VERSION=$(bash version.sh)
    fi
fi

export OUTPUT_PKG="Pitchenga-macOS-AppStore.pkg"

# Check for required identifiers and credentials
if [ -z "${APPLE_ID:-}" ] || [ -z "${APPLE_PASSWORD:-}" ] || [ -z "${APPLE_TEAM_ID:-}" ] || [ -z "${MAC_APP_APPLE_ID:-}" ]; then
    printf "\n⚠️  One or more of required secrets is not set (APPLE_ID, APPLE_PASSWORD, APPLE_TEAM_ID, MAC_APP_APPLE_ID).\n"
    exit 1
fi

# Build the MAS-compliant package
# This script handles staging, Info.plist metadata injection, and signing.
printf "\n📦 Building MAS Package for TestFlight (Version: $VERSION)...\n"
./create-mas-pkg.sh "$ARTIFACTS_DIR" "$OUTPUT_PKG" "$VERSION"

# Upload to App Store Connect / TestFlight
printf "\n📤 Uploading to TestFlight...\n"

# Using --apple-id (numeric) is much more reliable than Bundle ID lookup.
# Explicitly setting --type osx to resolve platform determination errors.
if xcrun altool --upload-package "$OUTPUT_PKG" \
    --type osx \
    --username "$APPLE_ID" \
    --password "$APPLE_PASSWORD" \
    --asc-provider "$APPLE_TEAM_ID" \
    --apple-id "$MAC_APP_APPLE_ID" \
    --bundle-version "$VERSION" \
    --bundle-short-version-string "$VERSION"; then
    printf "\n✅ Successfully uploaded to TestFlight!\n"
else
    printf "\n❌ Failed to upload to TestFlight.\n"
    exit 1
fi
