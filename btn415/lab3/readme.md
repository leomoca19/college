# SMTP Client (Lab3)

A standalone Windows executable for sending emails with attachments via Gmail’s SMTP server. No build required—just configure and run.

## Contents
- `client.exe` — the SMTP client application
- `config.txt` — configuration file (sender, password, receivers)
- `email-attach.txt` — sample attachment file (replace with your own)
- `base64.cpp`, `base64.h` — Base64 encoder source
- `smtpClientLab3.cpp` — main C++ source (for reference)

---

## Configuration
1. Open **config.txt** in a text editor.
2. Enter your settings exactly as follows:
   ```text
   sender
   your_email@gmail.com
   password
   <16-character App Password>
   receivers
   recipient1@example.com
   recipient2@example.com
   ```
   - **sender**: your Gmail address
   - **password**: the App Password generated in your Google Account (no spaces)
   - **receivers**: list each recipient on its own line under the `receivers` key

3. Save and close `config.txt`.

---

## Attachment
- By default, the program looks for **email-attach.txt** in this folder.
- To attach a different file, rename or copy your file to **email-attach.txt**.

---

## Usage
1. Ensure `client.exe`, `config.txt`, and your attachment are all in this folder.
2. Double‑click **client.exe** or run from a Command Prompt:
   ```powershell
   .\client.exe
   ```
3. Watch the console for server responses.
4. On success, you will see:
   ```text
   Email sent successfully.
   ```

---

## Multiple Recipients
- Add as many addresses as you like under `receivers` in `config.txt`.
- The program will send one email to all listed recipients.

---

## Troubleshooting
- **535 BadCredentials**: Your App Password is incorrect or expired. Regenerate in Google Account → Security → App passwords.
- **SSL_connect failed**: Verify internet access and that port 465 is open. Use TLS_client_method().
- **No attachment**: Ensure **email-attach.txt** exists in this folder.

---

## FAQ
**Do I need to compile anything?**  
No—`client.exe` is pre‑built. Just configure and run.

**How do I generate an App Password?**  
In Google Account → Security → App passwords → select Mail + Other → Generate.

---

