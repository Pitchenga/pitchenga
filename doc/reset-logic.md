Logic Pro has a notoriously stubborn Audio Unit (AU) cache, and simply hitting "Build" in your IDE often isn't enough to make Logic register the new binary—especially if you changed parameters or the plugin's internal ID.

Here is how to properly rebuild and force Logic to see the newest version.

### The Standard Rebuild

*   Compile your plugin in your IDE (CLion/Xcode).
*   Ensure your build target includes the **AU** format.
*   Verify that the freshly built `.component` file is copied to `~/Library/Audio/Plug-Ins/Components/` (your JUCE/CMake setup should do this automatically post-build).

### Force Update: The Plug-in Manager Method

If Logic is already open, you can usually force it to rescan the specific plugin without restarting.

*   Open Logic Pro.
*   Navigate to **Logic Pro** \> **Settings** (or Preferences) \> **Plug-in Manager**.
*   Search for your plugin in the list.
*   Select it, then click **Reset & Rescan Selection** at the bottom of the window.
*   If the plugin is already instantiated on a track, you may need to remove it from the channel strip and add it back for the UI changes to fully take effect.

### Force Update: The "Nuke It" Method

Sometimes Logic's background scanner stubbornly refuses to acknowledge a structural change. If the Plug-in Manager method fails, you have to clear the cache manually.

*   Close Logic Pro completely.
*   Open your Mac's **Terminal**.
*   Run `rm ~/Library/Caches/AudioUnitCache/com.apple.audiounits.cache` to delete the cache file.
*   Run `killall -9 AudioComponentRegistrar` to force the background scanning service to restart.
*   Reopen Logic Pro. It will take slightly longer to boot as it performs a fresh scan of your AU directory.
