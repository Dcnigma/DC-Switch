#include <stdio.h>
#include <string.h>
#include <switch.h>

#include "util.h"
#include "menu.h"
#include "unzip.h"
#include "download.h"
#include "reboot_payload.h"

#define TEMP_FILE               "/switch/DC-Switch/temp"
#define FILTER_STRING           "browser_download_url\":\""

char g_sysVersion[50];
char g_amsVersion[50];


char *getSysVersion()
{
    return g_sysVersion;
}

char *getAmsVersion()
{
    return g_amsVersion;
}

void writeSysVersion()
{
	Result ret = 0;
	SetSysFirmwareVersion ver;

	if (R_FAILED(ret = setsysGetFirmwareVersion(&ver))) 
    {
		printf("GetFirmwareVersion() failed: 0x%x.\n\n", ret);
		return;
	}

    char sysVersionBuffer[20];
	snprintf(sysVersionBuffer, 20, "%u.%u.%u", ver.major, ver.minor, ver.micro);
    snprintf(g_sysVersion, sizeof(g_sysVersion), "Firmware Ver: %s", sysVersionBuffer);
}

void writeAmsVersion()
{
	Result ret = 0;
	u64 ver;
    u64 fullHash;
    SplConfigItem SplConfigItem_ExosphereVersion = (SplConfigItem)65000;
    SplConfigItem SplConfigItem_ExosphereVerHash = (SplConfigItem)65003;

	if (R_FAILED(ret = splGetConfig(SplConfigItem_ExosphereVersion, &ver))) 
    {
		printf("SplConfigItem_ExosphereVersion() failed: 0x%x.\n\n", ret);
		return;
	}

    if (R_FAILED(ret = splGetConfig(SplConfigItem_ExosphereVerHash, &fullHash))) 
    {
		printf("SplConfigItem_ExosphereVerHash() failed: 0x%x.\n\n", ret);
		return;
	}

    // write only the first 8 char of the hash.
    char shortHash[8];
	snprintf(shortHash, sizeof(shortHash), "%lx", fullHash);

    // write ams version number + hash.
    char amsVersionNum[25];
	snprintf(amsVersionNum, sizeof(amsVersionNum), "%lu.%lu.%lu (%s)", (ver >> 32) & 0xFF,  (ver >> 24) & 0xFF, (ver >> 16) & 0xFF, shortHash);

    // write string + ams version to global variable.
    snprintf(g_amsVersion, sizeof(g_amsVersion), "Atmosphere Ver: %s", amsVersionNum);
}

void copyFile(char *src, char *dest)
{
    FILE *srcfile = fopen(src, "rb");
    FILE *newfile = fopen(dest, "wb");

    if (srcfile && newfile)
    {
        char buffer[10000]; // 10kb per write, which is fast
        size_t bytes;       // size of the file to write (10kb or filesize max)

        while (0 < (bytes = fread(buffer, 1, sizeof(buffer), srcfile)))
        {
            fwrite(buffer, 1, bytes, newfile);
        }
    }
    fclose(srcfile);
    fclose(newfile);
}

int parseSearch(char *parse_string, char *filter, char* new_string)
{
    FILE *fp = fopen(parse_string, "r");
    
    if (fp)
    {
        char c;
        while ((c = fgetc(fp)) != EOF)
        {
            if (c == *filter)
            {
                for (int i = 0, len = strlen(filter) - 1; c == filter[i]; i++)
                {
                    c = fgetc(fp);
                    if (i == len)
                    {
                        for (int j = 0; c != '\"'; j++)
                        {
                            new_string[j] = c;
                            new_string[j+1] = '\0';
                            c = fgetc(fp);
                        }
                        fclose(fp);
                        remove(parse_string);
                        return 0;
                    }
                }
            }
        }
    }

    errorBox(350, 250, "Failed to find parse url!");
    fclose(fp);
    return 1;
}

int update_dcapps(char *url, char *output, int mode)
{
    if (mode == UP_AMS)
    {
        // ask if user wants to install atmosphere as well.
        int res = yesNoBox(mode, 390, 250, "Install Dcnigma's Pong?");

        if (res == YES)
        {
            // ask if user wants to overwite the atmosphere ini files.
            res = yesNoBox(mode, 355, 250, "Overwite Dcnigma's Pong?");

            if (res == YES)
            {

    if (!downloadFile(AMS_URL, TEMP_FILE, ON))
    {
	rename(TEMP_FILE, AMS_OUTPUT);
        unzip(AMS_OUTPUT, mode);
	remove(MAC_OUTPUT);                
	    }
                return 0;
            }
            return 1;
        }
        return 1;
    }
    return 1;
}
int update_ebooks(char *url, char *output, int mode)
{
    if (mode == UP_AMS_NOINI)
    {
        // ask if user wants to install atmosphere as well.
        int res = yesNoBox(mode, 390, 250, "Install Ebooks?");

        if (res == YES)
        {
            // ask if user wants to overwite the atmosphere ini files.
            res = yesNoBox(mode, 355, 250, "Overwite any Ebooks files?");

            if (res == YES)
            {

    if (!downloadFile(EBOOK_URL, TEMP_FILE, ON))
    {
	rename(TEMP_FILE, EBOOK_OUTPUT);
        unzip(EBOOK_OUTPUT, mode);
	    }
                return 0;
            }
            return 1;
        }
        return 1;
    }
    return 1;
}

int update_amiibo(char *url, char *output, int mode)
{
    if (mode == UP_HEKATE)
    {
        // ask if user wants to install atmosphere as well.
        int res = yesNoBox(mode, 390, 250, "Install Dcnigma's Amiibo's?");

        if (res == YES)
        {
            // ask if user wants to overwite the atmosphere ini files.
            res = yesNoBox(mode, 355, 250, "Overwite Amiibo's files?");

            if (res == YES)
            {

    if (!downloadFile(HEKATE_URL, TEMP_FILE, ON))
    {
	rename(TEMP_FILE, HEKATE_OUTPUT);
        unzip(HEKATE_OUTPUT, mode);
	remove(MAC_OUTPUT);  
                }
                return 0;
            }
            return 1;
        }
        return 1;
    }
    return 1;
}

void update_app()
{
    // download new nro as a tempfile.
    if (!downloadFile(APP_URL, TEMP_FILE, OFF))
    {
        // remove current nro file.
        remove(APP_OUTPUT);
        // remove nro from /switch/.
        remove(OLD_APP_PATH);
        // rename the downloaded temp_file with the correct nro name.
        rename(TEMP_FILE, APP_OUTPUT);
        // using errorBox as a message window on this occasion. 
        errorBox(400, 250, "      Update complete!\nRestart app to take effect");
    }
}
