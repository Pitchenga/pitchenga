#!/bin/bash
set -euo pipefail

# Default artifacts path if not provided
ARTIFACTS_DIR=${1:-"cmake-build-release/Pitchenga_artefacts/Release"}
OUTPUT_DMG=${2:-"Pitchenga-macOS-Portable.dmg"}

echo "--- Creating macOS Portable DMG ---"
echo "Artifacts source: $ARTIFACTS_DIR"
echo "Output DMG:       $OUTPUT_DMG"

if [ ! -d "$ARTIFACTS_DIR" ]; then
    echo "Error: Artifacts directory not found: $ARTIFACTS_DIR"
    exit 1
fi

STAGING_DIR="dmg_staging"
rm -rf "${STAGING_DIR:?}"
mkdir -p "$STAGING_DIR"

# Copy bundles
echo "Copying bundles..."
cp -R "$ARTIFACTS_DIR/Standalone/Pitchenga.app" "$STAGING_DIR/"
cp -R "$ARTIFACTS_DIR/AU/Pitchenga.component" "$STAGING_DIR/"
cp -R "$ARTIFACTS_DIR/VST3/Pitchenga.vst3" "$STAGING_DIR/"

# Create symlink to Applications for easy installation
ln -s /Applications "$STAGING_DIR/Applications"

# Create the DMG
echo "Generating DMG..."
hdiutil create -volname "Pitchenga" -srcfolder "$STAGING_DIR" -ov -format UDZO "$OUTPUT_DMG"

# Clean up
rm -rf "$STAGING_DIR"

echo "Success! Portable DMG created at $OUTPUT_DMG"
