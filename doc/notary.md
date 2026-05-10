### Step 1: Create Certificate Signing Requests (CSRs)

Apple needs a cryptographic lock from your Mac for each certificate. **CRITICAL:** You must create a **unique** CSR for
**each** certificate type (Application, Installer, and Distribution). Using the same CSR for multiple certificates will
cause them to "collapse" into a single identity on GitHub runners.

1. Open the **Keychain Access** app on your Mac.
2. For **each** certificate you need (Application, Installer, etc.):
    * Click **Keychain Access** \> **Certificate Assistant** \> 
      **Request a Certificate from a Certificate Authority...**
    * **User Email Address:** Your Apple Developer email.
    * **Common Name:** Something descriptive (e.g., "Pitchenga App CSR", "Pitchenga Installer CSR").
    * **Request is:** Select **Saved to disk**.
    * Save it with a unique name (e.g., `app.certSigningRequest`, `installer.certSigningRequest`).

### Step 2: Generate the Certificates on Apple's Portal

You need **two** different types of certificates to distribute a Mac app outside the App Store:

1. **Developer ID Application**: Used to sign the app and plugins.
2. **Developer ID Installer**: Used to sign the `.pkg` installer.

For **each** of these:

1. Go to [developer.apple.com](https://developer.apple.com/) \> **Certificates, IDs & Profiles**.
2. Click the blue **+** icon.
3. Select the type (e.g., **Developer ID Application**).
4. **Choose File** and upload the **specific CSR** you created for that type in Step 1.
5. Download the resulting `.cer` file.
6. **Repeat** for **Developer ID Installer** (using its unique CSR) and any other types.

### Step 3: Install and Export the combined `.p12`

GitHub Actions needs both certificates in a single file to sign everything correctly.
You can also include the **Apple Distribution** certificate in this same file if you are building for iOS.

1. Double-click **all** downloaded `.cer` files to install them into your Keychain.
2. Open **Keychain Access**, select the **login** keychain, and click the **My Certificates** tab.
3. Find the certificates (including **Apple Distribution** if applicable):
    * **Developer ID Application: [Your Name] ([Team ID])**
    * **Developer ID Installer: [Your Name] ([Team ID])**
4. **Shift-Click** to select the certificates.
5. Ensure the expansion arrows `>` are toggled so the private keys are visible (and selected).
6. Right-click and select **Export N items...** (N should be number of certificates times 2).
7. Save as a **Personal Information Exchange (.p12)** file.
8. Create a password and save it as your `MAC_CERTS_PASSWORD` secret in GitHub.

### Step 4: Inject it into GitHub Actions

This is the exact same process we discussed earlier, just with the production-ready certificate.

1. Open your Mac Terminal and convert the `.p12` to a text string:
   ```
   PKSC="apple-all.p12"; base64 -i $PKSC | tee $PKSC.txt | pbcopy
   ```
2. Open `apple-all.p12.txt` and copy the massive block of text.
3. Go to your GitHub Repository \> **Settings** \> **Secrets and variables** \> **Actions**.
4. Update (or create) the `MAC_CERTS` secret with that text block.
5. Update the `MAC_CERTS_PASSWORD` secret with the password you made in Step 3.

### The Final Notarization Pieces

Because your `release.yaml` is already configured to run Apple's `notarytool`, you just need to add the final three text
secrets to your GitHub repository to authenticate the notarization upload:

* **`APPLE_ID`**: Your standard Apple ID email address.
* **`APPLE_PASSWORD`**: An app-specific password. Go to
  [appleid.apple.com](https://appleid.apple.com/),
  log in, go to the "App-Specific Passwords" section, generate one (call it "GitHub Actions"), and paste it here. Do
  not use your actual Apple ID login password.
* **`APPLE_TEAM_ID`**: Your 10-character Team ID. You can find this in the top right corner of the Apple Developer
  portal under your name.

## Troubleshooting

If your GitHub build fails with "item could not be found" or "0 valid identities found", verify your `.p12` file
locally.
Run this one-liner in your terminal to check if your identities are present and get the MD5 checksum to compare with the
GitHub logs:

```bash
TXT="apple-all.p12.txt"; PKSC="$TXT.p12"; wc -c "$TXT" && base64 -D -i "$TXT" -o "$PKSC" && md5 -q "$PKSC"; KEYCHAIN="temp.keychain"; security create-keychain -p t "$KEYCHAIN" && security import "$PKSC" -k "$KEYCHAIN" -T /usr/bin/codesign && security find-identity -v "$KEYCHAIN"; security delete-keychain "$KEYCHAIN"; rm -f "$PKSC"
```


