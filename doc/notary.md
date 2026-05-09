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

### Step 2: Generate the Certificate on Apple's Portal

Now you trade that request file for the actual certificate.

1. Go to
   [developer.apple.com](https://developer.apple.com/)
   and log in.
2. Click on **Certificates, IDs & Profiles** in the left sidebar.
3. Click the blue **+** icon next to the "Certificates" header to create a new one.
4. Scroll down to the "Software" section and select **Developer ID Application**.
    * _Note: Do NOT select "Apple Distribution" or "Mac App Distribution". Those are strictly for the Mac App Store.
      Developer ID is for direct downloads._
5. Click **Continue**. (If it asks about a G2 Sub-CA profile, select the default option).
6. Click **Choose File**, upload the `.certSigningRequest` you saved to your Desktop, and click **Continue**.
7. Click **Download** to get your shiny new `developerID_application.cer` file.

### Step 3: Install and Export the `.p12`

Now you need to pair the certificate Apple gave you with the private key your Mac generated in Step 1, and package them
together for GitHub.

1. Double-click the downloaded `developerID_application.cer` file. This automatically installs it into Keychain Access.
2. Open **Keychain Access** and select the **login** keychain on the left, then click the **My Certificates** tab at the
   top.
3. Find your new certificate. It will be named **Developer ID Application: \[Your Name\] (\[Your Team ID\])**.
4. Click the small expansion arrow `>` next to it to ensure your private key is attached underneath it.
5. Right-click the **Developer ID Application** certificate and select **Export "Developer ID Application..."**
6. Ensure the File Format is set to **Personal Information Exchange (.p12)** and save it to your Desktop.
7. It will ask you to create a password. **Write this password down**—this is what you will put into GitHub as your
   `MAC_CERTS_PASSWORD`.

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
  [appleid.apple.com](https://appleid.apple.com/)
  , log in, go to the "App-Specific Passwords" section, generate one (call it "GitHub Actions"), and paste it here. Do
  not use your actual Apple ID login password.
* **`APPLE_TEAM_ID`**: Your 10-character Team ID. You can find this in the top right corner of the Apple Developer
  portal under your name.
