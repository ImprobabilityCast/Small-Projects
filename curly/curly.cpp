#include "curly.h"

std::ofstream ofile;
bool save2file = false;
size_t saveToBuffer(char* ptr, size_t size, size_t nmemb, void* userdata) {
	if (save2file) {
		ofile.write(ptr, (size*nmemb));
    }
	std::cout << ptr;
	return (size * nmemb);
}

const char* printHelp() {
	return "curly v0.3.1 July 2017\n"
	"Usage:\tcurly URL [RANGE] [FILE]\n"
	"URL:\trequired.  This can also be a file containing a valid URL\n"
	"\ton the first line\n"
	"RANGE:\toptional.  Request this range of bytes (e.g. 0-150)\n"
	"FILE:\toptional.  Save response body to this file\n\n"
	"Options:\n"
	"\t--help\t\tprints this message\n";
}

bool save(const char* fileName, CURL* easyHand) {
	ofile.open(fileName, std::ios::binary);
	if (!ofile.is_open()) {
		std::cout << "Could not save to the specified file" << std::endl;
		return false;
	}
	save2file = true;
	std::cout << "Here we go..." << std::endl;
	curl_easy_perform(easyHand);
	ofile.close();
	return true;
}


int main(int argc, char** argv) {
	// see if there are any args and if the first arg is "--help"
	if ( (argc < 2) || (0 == strcmp(argv[1], "--help")) ) {
		std::cout << printHelp() << std::endl;
		return 0;
	}
	// initilize curl
	CURL* easyHand = curl_easy_init();
	curl_easy_setopt(easyHand, CURLOPT_VERBOSE, 1L);
	curl_easy_setopt(easyHand, CURLOPT_WRITEFUNCTION, saveToBuffer);
	curl_easy_setopt(easyHand, CURLOPT_FOLLOWLOCATION, 1L);
	curl_easy_setopt(easyHand, CURLOPT_USERAGENT,
		"Mozilla/5.0 (X11; Linux x86_64) AppleWebKit/537.36"
		" (KHTML, like Gecko) Ubuntu Chromium/58.0.3029.110"
		" Chrome/58.0.3029.110 Safari/537.36");
	// if it got this far, then one or more arguments 
	// were passed to curly
	
	// Check to see if argv[1] is a file or a URL
	std::ifstream ifile (argv[1]);
	if (ifile.is_open()) {
		std::string str;
		std::getline(ifile, str);
		curl_easy_setopt(easyHand, CURLOPT_URL, str.c_str());
		ifile.close();
	} else {
		curl_easy_setopt(easyHand, CURLOPT_URL, argv[1]);
    }

	// see if only one argument was passed
	if (argc == 2) {
		std::cout << "Here we go..." << std::endl;
		curl_easy_perform(easyHand);
		std::cout << std::endl;
	}
    
	// if argc is not less than 2 and not equal to two
	// then it must be more than two, therefore, 
	// see if argv[2] a valid range argument [digit-digit]
	else if (regex_match(argv[2], std::regex("(\\d)+-(\\d)*"))) {
		curl_easy_setopt(easyHand, CURLOPT_RANGE, argv[2]);
		// if there's a 3rd argument, treat it like a 
		// filename to save to
		if (argc > 3) {
			save(argv[3], easyHand);
		} else { // otherwise, run curl
			std::cout << "Here we go..." << std::endl;
			curl_easy_perform(easyHand);
			std::cout << std::endl;
		}
	} else {
        // if argv[2] is not a valid range argument, try to
        // use it as a file name
		save(argv[2], easyHand);
    }

	// clean up
	curl_easy_cleanup(easyHand);
	return 0;
}
