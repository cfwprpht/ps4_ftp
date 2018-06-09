# ps4_ftp

Ported FTP payload to an application (simple for now, no SELF Decryption) and made it match for libHB.

It is working but have some problems with getting 'LIST'.

I will put it into the release so others also can have a look while I am busy.

It uses the USB drive for logging. Create a folder called 'PS4FTP' into the root of your USB drive.

If you want to enable debug logging, create an empty text file within 'PS4FTP' directory and rename it to 'usedebug.txt'.

Attemption: The drive need to be connected before you run the app. Since the check for logging is within the preparation process.
