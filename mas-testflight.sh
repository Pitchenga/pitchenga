#!/bin/bash
set -e

# Load environment variables if .env exists
if [ -f .env ]; then
    export $(grep -v '^#' .env | xargs)
fi

# Configuration
VERSION=${1:-"1.0.0"}
export ARTIFACTS_DIR="cmake-build-release/Pitchenga_artefacts/Release"
export OUTPUT_PKG="Pitchenga-macOS-TestFlight.pkg"

# Build the MAS-compliant package
# This script handles staging, Info.plist metadata injection, and signing.
printf "\n📦 Building MAS Package for TestFlight (Version: $VERSION)...\n"
./create-mas-pkg.sh "$ARTIFACTS_DIR" "$OUTPUT_PKG" "$VERSION"

# Check for upload credentials
if [ -z "${APPLE_ID:-}" ] || [ -z "${APPLE_PASSWORD:-}" ] || [ -z "${APPLE_TEAM_ID:-}" ] || [ -z "${MAC_APP_ID:-}" ]; then
    printf "\n⚠️  Required secrets (APPLE_ID, APPLE_PASSWORD, APPLE_TEAM_ID, or MAC_APP_ID) not set.\n"
    printf "The PKG has been built locally at: $OUTPUT_PKG\n"
    exit 0
fi

# Upload to App Store Connect / TestFlight
printf "\n📤 Uploading to TestFlight...\n"

# Using --apple-id (numeric) is much more reliable than Bundle ID lookup.
# Explicitly setting --type osx to resolve platform determination errors.
xcrun altool --upload-package "$OUTPUT_PKG" \
    --type osx \
    --username "$APPLE_ID" \
    --password "$APPLE_PASSWORD" \
    --asc-provider "$APPLE_TEAM_ID" \
    --apple-id "$MAC_APP_ID" \
    --bundle-version "$VERSION" \
    --bundle-short-version-string "$VERSION"

printf "\n✅ Successfully uploaded to TestFlight!\n"
