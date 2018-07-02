
#include <application.h>

// Our application instance.
Application app;

// The Port to use.
#define PS4_PORT 1337

// Draw the Powered Developers to screen.
void DrawPowerDevelopers(void) { app.drawStringf(app.getCenteredPosX(strlen("Powered by IDC & Vortex")), 0.1, "Powered by IDC & Vortex"); }

// The Entry for the FTP app.
void *FtpEntry(void *) {
	// Get Local IP.
	char *PS4_IP = SwissKnife::GetLocalIP();

	// Check if IP is not empty.
	if (strlen(PS4_IP) > 0) {
		Console::LineBreak();
		Console::LineBreak();
		Console::LineBreak();
		Console::LineBreak();
		Console::WriteWarning("PS4 listening on IP %s Port %i\n", PS4_IP, PS4_PORT);
		Console::WriteLine("Press X to run ftp.\n.");

		// Clear the input before we wait for the user.
		app.ClearInput();
		while (app.Input() != ssi::kButtonCross) { /*  */ }

		// Get SSID.
		char *ssid = SwissKnife::GetSsid();

		// Get Language of this user.
		char *lang = SwissKnife::GetLanguage();

		// Resolve the exploit host used on this console.
		char *link;
		int ret = -99;
		if (!strcmp(ssid, "ESP8266XploitHost")) {
			Console::WriteLine("Found a ESP8266 'C0d3m4st4'.\n");
			
			// Generate link.
			link = (char *)malloc(76);
			sprintf(link, "%s%s%s", "http://manuals.playstation.net/document/", lang, "/ps4/505_xvtx_ftp_index.html");
		} else if (!strcmp(ssid, "ESP8266 For PS4")) {
			Console::WriteLine("Found a ESP8266 '@Draqen88'.\n");

			// Generate link.
			link = (char *)malloc(76);
			sprintf(link, "%s%s%s", "http://manuals.playstation.net/document/", lang, "/ps4/ftp_index.html");
		} else {
			Console::WriteLine("I Assume AlAzif Exploit Host.\n");
			link = strdup("http://manuals.playstation.net/exploits/5.05/FTP/index.html");
		}

		// Run the browser and overload the payload link.
		ret = sceSystemServiceLaunchWebBrowser(link, NULL);
		if (ret == SCE_OK) Console::WriteWarning("FTP running !\n");
		else Console::WriteError("Couldn't run FTP... :'(\n");
		
		// Free up used mem.
		free(ssid);
		free(link);
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