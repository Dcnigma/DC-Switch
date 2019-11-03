#ifndef _DOWNLOAD_H_
#define _DOWNLOAD_H_

#define AMS_URL     "https://github.com/Dcnigma/DC-Switch/raw/master/DCnigma-switch-exampels.zip"
#define HEKATE_URL  "https://github.com/Dcnigma/DC-Switch/raw/master/emuiibo.zip"
#define EBOOK_URL     "https://github.com/Dcnigma/DC-Switch/raw/master/ebooks.zip"
#define APP_URL     "https://github.com/Dcnigma/DC-Switch/raw/master/DC-Switch.nro"

int downloadFile(const char *url, const char *output, int api_mode);

#endif