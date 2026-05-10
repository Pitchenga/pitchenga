#!/bin/bash
set -e

# Default artifacts path if not provided
ARTIFACTS_DIR=${1:-"cmake-build-release/Pitchenga_artefacts/Release"}
OUTPUT_PKG=${2:-"Pitchenga-macOS-Installer.pkg"}
VERSION=${3:-"1.0.0"}

# Generate dynamic version if 1.0.0 is provided
if [ "$VERSION" == "1.0.0" ]; then
    VERSION=$(bash version.sh)
fi

echo "--- Creating macOS Package Installer ---"
echo "Artifacts source: $ARTIFACTS_DIR"
echo "Output PKG:       $OUTPUT_PKG"
echo "Version:          $VERSION"

if [ ! -d "$ARTIFACTS_DIR" ]; then
    echo "Error: Artifacts directory not found: $ARTIFACTS_DIR"
    exit 1
fi

STAGING_DIR="pkg_staging"
rm -rf "$STAGING_DIR"
mkdir -p "$STAGING_DIR/components"
mkdir -p "$STAGING_DIR/roots"

# Helper function to build a component package with relocation disabled
build_component() {
    local bundle_name=$1
    local install_path=$2
    local pkg_name=$3
    local bundle_path="$ARTIFACTS_DIR/$bundle_name"
    
    if [ -d "$bundle_path" ]; then
        echo "Building component package for $bundle_name..."
        
        # Create a clean temporary root for this specific component
        # This avoids permission issues and relocation bugs
        local temp_root="$STAGING_DIR/roots/$pkg_name"
        local install_dir
        install_dir=$(dirname "$install_path")
        mkdir -p "$temp_root/$install_dir"
        cp -R "$bundle_path" "$temp_root/$install_dir/"
        
        # Ensure staged files are writable
        chmod -R +w "$temp_root"
        
        # Create a temporary plist to disable relocation
        local plist_path="$STAGING_DIR/${pkg_name}.plist"
        pkgbuild --analyze --root "$temp_root" "$plist_path"
        
        # Use sed to set BundleIsRelocatable to false
        sed -i '' 's/<key>BundleIsRelocatable<\/key>.*<true\/>/<key>BundleIsRelocatable<\/key><false\/>/' "$plist_path"
        
        # Build the component package using the temporary root
        pkgbuild --root "$temp_root" \
          --component-plist "$plist_path" \
          --version "$VERSION" \
          "$STAGING_DIR/components/$pkg_name"
    else
        echo "Error: $bundle_name not found at $bundle_path"
        exit 1
    fi
}

# Build Component Packages
build_component "Standalone/Pitchenga.app" "/Applications/Pitchenga.app" "standalone.pkg"
build_component "AU/Pitchenga.component" "/Library/Audio/Plug-Ins/Components/Pitchenga.component" "au.pkg"
build_component "VST3/Pitchenga.vst3" "/Library/Audio/Plug-Ins/VST3/Pitchenga.vst3" "vst3.pkg"

# Generate Distribution XML
echo "Generating Distribution.xml..."
cat <<EOF > "$STAGING_DIR/Distribution.xml"
<?xml version="1.0" encoding="utf-8"?>
<installer-gui-script minSpecVersion="2">
    <title>Pitchenga</title>
    <options customize="always" require-scripts="false" hostArchitectures="arm64,x86_64"/>
    <choices-outline>
        <line choice="choice_standalone"/>
        <line choice="choice_au"/>
        <line choice="choice_vst3"/>
    </choices-outline>
    
    <choice id="choice_standalone" title="Standalone Application" description="Install the Pitchenga Standalone application to /Applications.">
        <pkg-ref id="com.github.pitchenga.Pitchenga"/>
    </choice>
    <choice id="choice_au" title="Audio Unit Plugin" description="Install the Pitchenga AU plugin for Logic Pro, Ableton Live, etc.">
        <pkg-ref id="com.github.pitchenga.Pitchenga.AU"/>
    </choice>
    <choice id="choice_vst3" title="VST3 Plugin" description="Install the Pitchenga VST3 plugin for Cubase, Studio One, etc.">
        <pkg-ref id="com.github.pitchenga.Pitchenga.VST3"/>
    </choice>

    <pkg-ref id="com.github.pitchenga.Pitchenga" version="$VERSION" onConclusion="none">standalone.pkg</pkg-ref>
    <pkg-ref id="com.github.pitchenga.Pitchenga.AU" version="$VERSION" onConclusion="none">au.pkg</pkg-ref>
    <pkg-ref id="com.github.pitchenga.Pitchenga.VST3" version="$VERSION" onConclusion="none">vst3.pkg</pkg-ref>
</installer-gui-script>
EOF

# Build Final Product Package
echo "Building final product package..."
productbuild --distribution "$STAGING_DIR/Distribution.xml" \
  --package-path "$STAGING_DIR/components" \
  --version "$VERSION" \
  "$OUTPUT_PKG"

# Clean up
rm -rf "$STAGING_DIR"

echo "Success! Installer created at $OUTPUT_PKG"
