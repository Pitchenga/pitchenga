### Step 1: Create Certificate Signing Requests (CSRs)

Apple needs a cryptographic lock from your Mac for each certificate. **CRITICAL:** You must create a **unique** CSR for
**each** certificate type (Application, Installer, and Distribution). Using the same CSR for multiple certificates will
cause them to "collapse" into a single identity on GitHub runners.

* Open the **Keychain Access** app on your Mac.
* For **each** certificate you need (Application, Installer, etc.):
    * Click **Keychain Access** \> **Certificate Assistant** \>
      **Request a Certificate from a Certificate Authority...**
    * **User Email Address:** Your Apple Developer email.
    * **Common Name:** Something descriptive (e.g., "Pitchenga App CSR", "Pitchenga Installer CSR").
    * **Request is:** Select **Saved to disk**.
    * Save it with a unique name (e.g., `app.certSigningRequest`, `installer.certSigningRequest`).

### Step 2: Generate the Certificates on Apple's Portal

To distribute Pitchenga, you need different sets of certificates depending on the destination:

#### For Direct Distribution (Website/GitHub)

* **Developer ID Application**: Used to sign the app and plugins.
* **Developer ID Installer**: Used to sign the `.pkg` installer.

#### For Mac App Store (MAS)

* **Apple Distribution**: Used to sign the Standalone app for the store.
* **Mac Installer Distribution** (**3rd Party Mac Developer Installer**): Used to sign the `.pkg` destined for App Store
  Connect.

For **each** of these:

* Go to [developer.apple.com](https://developer.apple.com/) \> **Certificates, IDs & Profiles**.
* Click the blue **+** icon.
* Select the type (e.g., **Developer ID Application**).
* **Choose File** and upload the **specific CSR** you created for that type in Step 1.
* Download the resulting `.cer` file.
* **Repeat** for all required types.

### Step 3: Install and Export the combined `.p12`

GitHub Actions needs both certificates in a single file to sign everything correctly.
You can also include the **Apple Distribution** certificate in this same file if you are building for iOS.

* Double-click **all** downloaded `.cer` files to install them into your Keychain.
* Open **Keychain Access**, select the **login** keychain, and click the **My Certificates** tab.
* Find the certificates (including **Apple Distribution** and **3rd Party Mac Developer Installer** if applicable):
    * **Developer ID Application: [Your Name] ([Team ID])**
    * **Developer ID Installer: [Your Name] ([Team ID])**
    * **Apple Distribution: [Your Name] ([Team ID])**
    * **3rd Party Mac Developer Installer: [Your Name] ([Team ID])**
* **Shift-Click** to select all applicable certificates.
* Ensure the expansion arrows `>` are toggled so the private keys are visible (and selected).
* Right-click and select **Export N items...** (N should be number of certificates times 2).
* Save as a **Personal Information Exchange (.p12)** file.
* Create a password and save it as your `MAC_CERTS_PASSWORD` secret in GitHub.

### Inject it into GitHub Actions

This is the exact same process we discussed earlier, just with the production-ready certificate.

* Open your Mac Terminal and convert the `.p12` to a text string:
   ```
   PKSC="apple-all.p12"; base64 -i $PKSC | tee $PKSC.txt | pbcopy
   ```
* Open `apple-all.p12.txt` and copy the massive block of text.
* Go to your GitHub Repository \> **Settings** \> **Secrets and variables** \> **Actions**.
* Update (or create) the `MAC_CERTS` secret with that text block.
* Update the `MAC_CERTS_PASSWORD` secret with the password you made in Step 3.

### The Final Notarization and App Store Pieces

Because your `release.yaml` is already configured to run Apple's `notarytool` and `altool`, you just need to add the
final three text secrets to your GitHub repository to authenticate the uploads:

* **`APPLE_ID`**: Your standard Apple ID email address.
* **`APPLE_PASSWORD`**: An app-specific password. Go to [appleid.apple.com](https://appleid.apple.com/),
  log in, go to the "App-Specific Passwords" section, generate one (call it "GitHub Actions"), and paste it here.
  Do not use your actual Apple ID login password.
* **`APPLE_TEAM_ID`**: Your 10-character Team ID. This is used as the **ASC Provider** for App Store uploads.

## Troubleshooting

If your GitHub build fails with "item could not be found" or "0 valid identities found", verify your `.p12` file
locally.
Run this one-liner in your terminal to check if your identities are present and get the MD5 checksum to compare with the
GitHub logs:

```bash
TXT="apple-all.p12.txt"; PKSC="$TXT.p12"; wc -c "$TXT" && base64 -D -i "$TXT" -o "$PKSC" && md5 -q "$PKSC"; KEYCHAIN="temp.keychain"; security create-keychain -p t "$KEYCHAIN" && security import "$PKSC" -k "$KEYCHAIN" -T /usr/bin/codesign && security find-identity -v "$KEYCHAIN"; security delete-keychain "$KEYCHAIN"; rm -f "$PKSC"
```
### List your current identities

```
security find-identity -v
```

### Reset your Keychain Search List

```
security list-keychains -s ~/Library/Keychains/login.keychain-db /Library/Keychains/System.keychain
```

