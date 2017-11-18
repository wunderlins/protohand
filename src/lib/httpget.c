/**
 * Fetch a file with http(s) or ftp(s)
 *
 * 2017, Simon Wunderlin
 * cURL Copyright (c) 1996 - 2017, Daniel Stenberg, daniel@haxx.se
 */
#include "httpget.h"

static size_t write_data(void *ptr, size_t size, size_t nmemb, void *stream) {
  size_t written = fwrite(ptr, size, nmemb, (FILE *)stream);
  return written;
}

void tempfilepath(char** tempfile) {
	char* temp = getenv("TEMP");
	//char tempfile[PATH_MAX] = {0};
	*tempfile = malloc(sizeof(char*) * (PATH_MAX+1));
	tempfile[0][0] = 0;
	strcat(*tempfile, temp);
	strcat(*tempfile, "/ph.dat");
}

int getfile(char* url) {
	
	char* tempfile;
	tempfilepath(&tempfile);
	//printf("TEMP: %s\n", tempfile);
	
	FILE* fp;
	fp = fopen(tempfile, "wb");
	
	CURL *curl_handle;

	curl_global_init(CURL_GLOBAL_ALL);

	/* init the curl session */
	curl_handle = curl_easy_init();

	/* set URL to get here */
	curl_easy_setopt(curl_handle, CURLOPT_URL, url);

	#ifdef SKIP_PEER_VERIFICATION
	/*
	 * If you want to connect to a site who isn't using a certificate that is
	 * signed by one of the certs in the CA bundle you have, you can skip the
	 * verification of the server's certificate. This makes the connection
	 * A LOT LESS SECURE.
	 *
	 * If you have a CA cert for the server stored someplace else than in the
	 * default bundle, then the CURLOPT_CAPATH option might come handy for
	 * you.
	 */
	curl_easy_setopt(curl_handle, CURLOPT_SSL_VERIFYPEER, 0L);
	#endif

	#ifdef SKIP_HOSTNAME_VERIFICATION
	/*
	 * If the site you're connecting to uses a different host name that what
	 * they have mentioned in their server certificate's commonName (or
	 * subjectAltName) fields, libcurl will refuse to connect. You can skip
	 * this check, but this will make the connection less secure.
	 */
	curl_easy_setopt(curl_handle, CURLOPT_SSL_VERIFYHOST, 0L);
	#endif
	
	/* Switch on full protocol/debug output while testing */
	//curl_easy_setopt(curl_handle, CURLOPT_VERBOSE, 1L);

	/* disable progress meter, set to 0L to enable and disable debug output */
	curl_easy_setopt(curl_handle, CURLOPT_NOPROGRESS, 1L);
	
	/* send all data to this function  */
	curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, write_data);

	/* write the page body to this file handle */
	curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, fp);
	
	/* follow http redirects */
	curl_easy_setopt(curl_handle, CURLOPT_FOLLOWLOCATION, 1L);
	
	/* get it! */
	CURLcode ret = curl_easy_perform(curl_handle);

	/* cleanup curl stuff */
	curl_easy_cleanup(curl_handle);
	
	fclose(fp);

	if (ret != CURLE_OK)
		return 1;
	
	#ifdef MAIN_GETFILE
	printf("Output stored in: %s\n", tempfile);
	#endif
	
	return 0;
}

#ifdef MAIN_GETFILE
int main(int argc, char** argv) {
	if(argc < 2) {
		printf("Usage: %s <URL>\n", argv[0]);
		return 1;
	}
	
	int res = getfile(argv[1]);
	
	return res;
}
#endif