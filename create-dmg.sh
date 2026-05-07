#!/bin/bash
set -e

# Default artifacts path if not provided
# Use build/Pitchenga_artefacts/Release as default to match CI, 
# but allow override for local testing (e.g. cmake-build-debug/...)
ARTIFACTS_DIR=${1:-"build/Pitchenga_artefacts/Release"}
OUTPUT_DMG=${2:-"Pitchenga.dmg"}

echo "--- Creating macOS DMG ---"
echo "Artifacts source: $ARTIFACTS_DIR"
echo "Output DMG:       $OUTPUT_DMG"

if [ ! -d "$ARTIFACTS_DIR" ]; then
    echo "Error: Artifacts directory not found: $ARTIFACTS_DIR"
    exit 1
fi

STAGING_DIR="Pitchenga_macOS_Staging"
rm -rf "$STAGING_DIR"
mkdir -p "$STAGING_DIR"

# Helper function to copy artifacts if they exist
copy_artifact() {
    local src="$ARTIFACTS_DIR/$1"
    if [ -d "$src" ]; then
        echo "Adding $1..."
        # Ensure the parent directory exists in staging if nested (though we flatten for DMG)
        cp -R "$src" "$STAGING_DIR/"
    else
        echo "Warning: $1 not found at $src"
    fi
}

copy_artifact "Standalone/Pitchenga.app"
copy_artifact "AU/Pitchenga.component"
copy_artifact "VST3/Pitchenga.vst3"

# Create symlinks for easier installation
echo "Creating installation symlinks..."
ln -s /Applications "$STAGING_DIR/Applications"
ln -s /Library/Audio/Plug-Ins/Components "$STAGING_DIR/Components"
ln -s /Library/Audio/Plug-Ins/VST3 "$STAGING_DIR/VST3"

# Create the DMG container
echo "Building DMG with hdiutil..."
rm -f "$OUTPUT_DMG"
hdiutil create -volname "Pitchenga" -srcfolder "$STAGING_DIR" -ov -format UDZO "$OUTPUT_DMG"

# Clean up staging directory
rm -rf "$STAGING_DIR"

echo "Success! DMG created at $OUTPUT_DMG"
