
#include "ps4_ftp.h"

// Our application instance.
Application app;

// The Port to use.
#define PS4_PORT 1337

// Flag to indicate, the app is running.
int run;

// Loggers.
Logger debug;
Logger info;

void custom_SHUTDOWN(ftps4_client_info_t *client) {
	FTP::ftps4_ext_client_send_ctrl_msg(client, "200 Shutting down..." FTPS4_EOL);
	run = 0;
}

char mount_from_path[PATH_MAXX]; /* Yes, global. Lazy */ /* As long it works, who cares ? */

void custom_MTFR(ftps4_client_info_t *client) {
	char from_path[PATH_MAX];
	/* Get the origin filename */
	FTP::ftps4_gen_ftp_fullpath(client, from_path, sizeof(from_path));

	/* The file to be renamed is the received path */
	strncpy(mount_from_path, from_path, sizeof(mount_from_path));
	FTP::ftps4_ext_client_send_ctrl_msg(client, "350 I need the destination name b0ss." FTPS4_EOL);
}

void custom_MTTO(ftps4_client_info_t *client) {
	char path_to[PATH_MAX];
	struct iovec iov[8];
	char msg[512];
	char errmsg[255];
	int result;

	/* Get the destination filename */
	FTP::ftps4_gen_ftp_fullpath(client, path_to, sizeof(path_to));

	/* Just in case */
	Sys::unmount(path_to, 0);

	iov[0].iov_base = (void *)"fstype";
	iov[0].iov_len = sizeof("fstype");
	iov[1].iov_base = (void *)"nullfs";
	iov[1].iov_len = sizeof("nullfs");
	iov[2].iov_base = (void *)"fspath";
	iov[2].iov_len = sizeof("fspath");
	iov[3].iov_base = path_to;
	iov[3].iov_len = strlen(path_to) + 1;
	iov[4].iov_base = (void *)"target";
	iov[4].iov_len = sizeof("target");
	iov[5].iov_base = mount_from_path;
	iov[5].iov_len = strlen(mount_from_path) + 1;
	iov[6].iov_base = (void *)"errmsg";
	iov[6].iov_len = sizeof("errmsg");
	iov[7].iov_base = errmsg;
	iov[7].iov_len = sizeof(errmsg);
	result = Sys::nmount(iov, 8, 0);
	if (result < 0) {
		if (strlen(errmsg) > 0)
			snprintf(msg, sizeof(msg), "550 Could not mount (%d): %s." FTPS4_EOL, errno, errmsg);
		else
			snprintf(msg, sizeof(msg), "550 Could not mount (%d)." FTPS4_EOL, errno);
		FTP::ftps4_ext_client_send_ctrl_msg(client, msg);
		return;
	}

	FTP::ftps4_ext_client_send_ctrl_msg(client, "200 Mount success." FTPS4_EOL);
}

void custom_UMT(ftps4_client_info_t *client) {
	char msg[512];
	int result;
	char mount_path[PATH_MAX];

	FTP::ftps4_gen_ftp_fullpath(client, mount_path, sizeof(mount_path));

	result = Sys::unmount(mount_path, 0);
	if (result < 0) {
		sprintf(msg, "550 Could not unmount (%d)." FTPS4_EOL, errno);
		FTP::ftps4_ext_client_send_ctrl_msg(client, msg);
		return;
	}

	FTP::ftps4_ext_client_send_ctrl_msg(client, "200 Unmount success." FTPS4_EOL);
}

bool UsbCheck(int device) {
	// Is device flag valid ?
	if (device < 0 || device > 1) return false;

	// Resolve device and set paths.
	String format;
	String usb = "/mnt/";
	String test = "ghse7ihbredguwezs.txt";
	String folder = "PS4FTP/";
	String debugTest = "PS4FTP/usedebug.txt";
	String _debug = "PS4FTP/debug_log_";

	if (device == 0) usb += "usb0/";
	else usb += "usb1/";

	// The Magic.
	format = usb;
	format += test;
	FILE *check = fopen(format.c_str(), "wb");
	if (check) {
		fclose(check);
		Sys::unlink(format.c_str());

		// Check for folder and generate if not exist.
		format = usb;
		format += folder;
		format += test;
		check = fopen(format.c_str(), "wb");
		if (check) {
			fclose(check);
			Sys::unlink(format.c_str());
		} else {
			format = usb;
			format += folder;
			Sys::mkdir(format.c_str(), 0777);
		}

		// Generate Time String.
		String time = SwissKnife::GetTimeString();
		format = usb;
		format += folder;
		format += "info_log_";
		format += time;
		format += ".txt";

		// Init logger.
		info.Init(format.c_str());

		// Set the info logger for the FTP.
		FTP::info = &info;

		// Shall we use debug logging ?
		format = usb;
		format += debugTest;
		check = fopen(format.c_str(), "r");
		if (check) {
			fclose(check);
			format = usb;
			format += _debug;
			format += time;
			format += ".txt";

			debug.Init(format.c_str());

			FTP::debug = &debug;
		}
		return true;
	}
	return false;
}

// Check for Usb drives and generate log files if found.
void CheckUsb(void) { if (!UsbCheck(0)) UsbCheck(1); }

// Draw the Powered Developers to screen.
void DrawPowerDevelopers(void) { app.drawStringf(app.getCenteredPosX(strlen("Powered by IDC & Vortex")), 0.1, "Powered by IDC & Vortex"); }

// The Entry for the FTP app.
void *FtpEntry(void *) {	
	// Setup Loggers and point them to the FTP.
	CheckUsb();

	// Get Local IP.
	char *PS4_IP = SwissKnife::GetLocalIP();

	// Check if IP is not empty.
	if (strlen(PS4_IP) > 0) {
		// Flag to initialize that we are still running.
		run = 1;

		// Initialize the FTP.
		FTP::ftps4_init(PS4_IP, PS4_PORT);
		FTP::ftps4_ext_add_custom_command("SHUTDOWN", custom_SHUTDOWN);
		FTP::ftps4_ext_add_custom_command("MTFR", custom_MTFR);
		FTP::ftps4_ext_add_custom_command("MTTO", custom_MTTO);
		FTP::ftps4_ext_add_custom_command("UMT", custom_UMT);

		// Tell user the IP and Port to use.
		if (FTP::info != nullptr) info.Log("PS4 listening on IP %s Port %i\n", PS4_IP, PS4_PORT);
		Console::LineBreak();
		Console::LineBreak();
		Console::LineBreak();
		Console::LineBreak();
		Console::WriteWarning("PS4 listening on IP %s Port %i\n", PS4_IP, PS4_PORT);
		Console::WriteLine("Press Options to exit on any time\n.");

		// While we are running.
		while (run) {
			// Wait a bit.
			sceKernelUsleep(5 * 1000);

			// Check if we shall close the app.
			if (app.Input() == ssi::kButtonOptions) break;
		}

		// Finalize.
		FTP::ftps4_fini();
		if (FTP::info != nullptr) info.Log("Bye!");
		Console::WriteLine("Bye!\n");
	} else Console::WriteError("Couldn't get PS4s Local IP !\n");
	Console::WriteWarning("Stop.");
	
	// Don't forget to clear allocated memory.
	if (PS4_IP != nullptr) free(PS4_IP);

	return 0;
}

// Main Entry of the program code.
int main(void) {
	// Enable Multi Line.
	app.MultiLine(true);

	// Enable the clock.
	app.UseTime(true);

	// Enable screen shot.
	app.UseScreenShot(true);
	
	// Enable Banner.
	String xerpi = "XERPI FTP";
	app.UseBanner(true);
	app.Banner(xerpi.c_str());
	app.BannerPos(0.4f, 0.01f);
	app.ShaderPos(0.41f, 0.01f);

	// Add the Powered Developers to draw to the screen.
	app.AddDraw(DrawPowerDevelopers);

	// Add our self.
	app.Add((void *)FtpEntry);

	// Run Application code now.
	return app.exec();
}