## ps4_ftp

Fixed for now. Althought i call it a dirty way since we use the browser to run the payload, it works for now.

Works with ESP8266 from Draqen88 & c0d3m4st4 as well as with Alazif Exploit Host.
I added all Possible Language Regions from the SDK. If you use a ESP from the above named devs and ftp do not start for you, let me know and tell me the link from the browser. 


Ported FTP App (Simple for now, no SELF Decryption) and made it match for libHB. It's running but have some problems with getting 'LIST'.
I'll put it into the release so others also can have a look while i am bussy.
It use the usb drive for logging. Create a Folder called 'PS4FTP' into the root of your usb drive. If you want to enable debug logging,
create a empty text file within 'PS4FTP' directory and rename it to 'usedebug.txt'.
Attemption: The drive need to be connected before you run the app. Since the check for logging is within the preparation process.