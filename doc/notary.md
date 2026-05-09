### Step 1: Create a Certificate Signing Request (CSR)

Apple needs a cryptographic lock from your specific Mac before they issue a certificate.

1. Open the **Keychain Access** app on your Mac.
2. In the top menu bar, click **Keychain Access** \> **Certificate Assistant** \> **Request a Certificate from a
   Certificate Authority...**
3. In the window that opens:
    * **User Email Address:** Enter the email associated with your Apple Developer account.
    * **Common Name:** Enter your name or company name.
    * **CA Email Address:** Leave this blank.
    * **Request is:** Select **Saved to disk**.
4. Click **Continue** and save the `.certSigningRequest` file to your Desktop.

### Step 2: Generate the Certificates on Apple's Portal

You need **two** different types of certificates to distribute a Mac app outside the App Store:

1. **Developer ID Application**: Used to sign the app and plugins.
2. **Developer ID Installer**: Used to sign the `.pkg` installer.

For **each** of these:

1. Go to [developer.apple.com](https://developer.apple.com/) and log in.
2. Click on **Certificates, IDs & Profiles**.
3. Click the blue **+** icon next to "Certificates".
4. Select the specific type (**Developer ID Application** first, then repeat for **Developer ID Installer**).
5. Click **Continue**.
6. Upload the `.certSigningRequest` you saved to your Desktop.
7. Click **Download** to get the `.cer` files.

### Step 3: Install and Export the combined `.p12`

GitHub Actions needs both certificates in a single file to sign everything correctly.
You can also include **Apple Distribution** certificate in this same file if you are building for iOS.

1. Double-click **all** downloaded `.cer` files to install them into your Keychain.
2. Open **Keychain Access**, select the **login** keychain, and click the **My Certificates** tab.
3. Find both certificates:
    * **Developer ID Application: [Your Name] ([Team ID])**
    * **Developer ID Installer: [Your Name] ([Team ID])**
4. **Shift-Click** to select BOTH certificates.
5. Ensure the expansion arrows `>` are toggled so the private keys are visible (and selected).
6. Right-click and select **Export 4 items...**
7. Save as a **Personal Information Exchange (.p12)** file.
8. Create a password and save it as your `MAC_CERTS_PASSWORD` secret in GitHub.

### Step 4: Inject it into GitHub Actions

This is the exact same process we discussed earlier, just with the production-ready certificate.

1. Open your Mac Terminal and convert the `.p12` to a text string:
   ```
   base64 -i ~/Desktop/your_developer_id.p12 -o ~/Desktop/cert_base64.txt
   ```
2. Open `cert_base64.txt` and copy the massive block of text.
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
