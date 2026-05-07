#!/bin/bash
set -e

# Default artifacts path if not provided
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
hdiutil create -volname "$VOL_NAME" -srcfolder "$STAGING_DIR" -ov -format UDRW -size 300m "$TEMP_DMG"

# Mount the temporary DMG
echo "Mounting DMG to arrange icons..."
DEVICE=$(hdiutil attach -readwrite -noverify "$TEMP_DMG" | egrep '^/dev/' | sed 1q | awk '{print $1}')
sleep 3

# Use AppleScript to arrange icons
echo "Arranging icons with AppleScript..."
osascript <<EOF
tell application "Finder"
    set theDisk to disk "$VOL_NAME"
    open theDisk
    set theView to container window of theDisk
    
    set current view of theView to icon view
    set toolbar visible of theView to false
    set statusbar visible of theView to false
    -- {left, top, right, bottom}
    set the bounds of theView to {400, 100, 1000, 550}
    
    set viewOptions to the icon view options of theView
    set icon size of viewOptions to 80
    set text size of viewOptions to 12
    set arrangement of viewOptions to not arranged
    
    -- Positioning pairs
    set position of item "Pitchenga.app" of theDisk to {150, 100}
    set position of item "Applications" of theDisk to {450, 100}
    
    set position of item "Pitchenga.component" of theDisk to {150, 240}
    set position of item "Components" of theDisk to {450, 240}
    
    set position of item "Pitchenga.vst3" of theDisk to {150, 380}
    set position of item "VST3" of theDisk to {450, 380}
    
    update items of theDisk
    delay 5
    close theView
end tell
EOF

# Explicitly sync to disk
echo "Syncing and unmounting..."
sync
sleep 2
hdiutil detach "$DEVICE"

# Convert to final compressed DMG
echo "Finalizing DMG..."
rm -f "$OUTPUT_DMG"
hdiutil convert "$TEMP_DMG" -format UDZO -o "$OUTPUT_DMG"

# Clean up
rm -f "$TEMP_DMG"
rm -rf "$STAGING_DIR"

echo "Success! DMG created at $OUTPUT_DMG"
