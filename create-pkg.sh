#!/bin/bash
set -e

# Default artifacts path if not provided
ARTIFACTS_DIR=${1:-"cmake-build-release/Pitchenga_artefacts/Release"}
OUTPUT_PKG=${2:-"Pitchenga-macOS.pkg"}
VERSION=${3:-"1.0.0"}

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

# Build Component Packages
echo "Building component packages..."

# Standalone -> /Applications
if [ -d "$ARTIFACTS_DIR/Standalone/Pitchenga.app" ]; then
    pkgbuild --component "$ARTIFACTS_DIR/Standalone/Pitchenga.app" \
      --install-location "/Applications" \
      --version "$VERSION" \
      "$STAGING_DIR/components/standalone.pkg"
else
    echo "Error: Standalone app not found."
    exit 1
fi

# AU -> /Library/Audio/Plug-Ins/Components
if [ -d "$ARTIFACTS_DIR/AU/Pitchenga.component" ]; then
    pkgbuild --component "$ARTIFACTS_DIR/AU/Pitchenga.component" \
      --install-location "/Library/Audio/Plug-Ins/Components" \
      --version "$VERSION" \
      "$STAGING_DIR/components/au.pkg"
else
    echo "Error: AU plugin not found."
    exit 1
fi

# VST3 -> /Library/Audio/Plug-Ins/VST3
if [ -d "$ARTIFACTS_DIR/VST3/Pitchenga.vst3" ]; then
    pkgbuild --component "$ARTIFACTS_DIR/VST3/Pitchenga.vst3" \
      --install-location "/Library/Audio/Plug-Ins/VST3" \
      --version "$VERSION" \
      "$STAGING_DIR/components/vst3.pkg"
else
    echo "Error: VST3 plugin not found."
    exit 1
fi

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
