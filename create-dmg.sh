#!/bin/bash
set -e

# Default artifacts path if not provided
# Use build/Pitchenga_artefacts/Release as default to match CI, 
# but allow override for local testing (e.g. cmake-build-debug/...)
ARTIFACTS_DIR=${1:-"build/Pitchenga_artefacts/Release"}
OUTPUT_DMG=${2:-"Pitchenga.dmg"}
VOL_NAME="Pitchenga"

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
        cp -R "$src" "$STAGING_DIR/"
    else
        echo "Warning: $1 not found at $src"
    fi
}

copy_artifact "Standalone/Pitchenga.app"
copy_artifact "AU/Pitchenga.component"
copy_artifact "VST3/Pitchenga.vst3"

# Create symlinks
echo "Creating installation symlinks..."
ln -s /Applications "$STAGING_DIR/Applications"
ln -s /Library/Audio/Plug-Ins/Components "$STAGING_DIR/Components"
ln -s /Library/Audio/Plug-Ins/VST3 "$STAGING_DIR/VST3"

# Create a temporary read-write DMG
echo "Building temporary DMG..."
TEMP_DMG="temp.dmg"
rm -f "$TEMP_DMG"
hdiutil create -volname "$VOL_NAME" -srcfolder "$STAGING_DIR" -ov -format UDRW "$TEMP_DMG"

# Mount the temporary DMG
echo "Mounting DMG to arrange icons..."
DEVICE=$(hdiutil attach -readwrite -noverify "$TEMP_DMG" | egrep '^/dev/' | sed 1q | awk '{print $1}')
sleep 2

# Use AppleScript to arrange icons
# Layout: Left column (Binaries), Right column (Symlinks)
# Pairs: App -> Applications, Component -> Components, VST3 -> VST3
echo "Arranging icons with AppleScript..."
osascript <<EOF
tell application "Finder"
    tell disk "$VOL_NAME"
        open
        set current view of container window to icon view
        set toolbar visible of container window to false
        set statusbar visible of container window to false
        set the bounds of container window to {400, 100, 1000, 500}
        set viewOptions to the icon view options of container window
        set icon size of viewOptions to 72
        set arrangement of viewOptions to not arranged
        
        -- Binary positions (Left)
        set position of item "Pitchenga.app" to {150, 100}
        set position of item "Pitchenga.component" to {150, 200}
        set position of item "Pitchenga.vst3" to {150, 300}
        
        -- Symlink positions (Right)
        set position of item "Applications" to {450, 100}
        set position of item "Components" to {450, 200}
        set position of item "VST3" to {450, 300}
        
        close
        update without registering applications
        delay 2
    end tell
end tell
EOF

# Unmount and convert to compressed DMG
echo "Unmounting and finalizing DMG..."
hdiutil detach "$DEVICE"
rm -f "$OUTPUT_DMG"
hdiutil convert "$TEMP_DMG" -format UDZO -o "$OUTPUT_DMG"

# Clean up
rm -f "$TEMP_DMG"
rm -rf "$STAGING_DIR"

echo "Success! DMG created at $OUTPUT_DMG"
