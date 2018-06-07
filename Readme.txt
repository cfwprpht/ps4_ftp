## ps4_ftp

Ported FTP App (Simple for now, no SELF Decryption) and made it match for libHB. It's running but have some problems with getting 'LIST'.
I'll put it into the release so others also can have a look while i am bussy.
It use the usb drive for logging. Create a Folder called 'PS4FTP' into the root of your usb drive. If you want to enable debug logging,
create a empty text file within 'PS4FTP' directory and rename it to 'usedebug.txt'.
Attemption: The drive need to be connected before you run the app. Since the check for logging is within the preparation process.