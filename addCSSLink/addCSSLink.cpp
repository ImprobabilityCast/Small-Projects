#include "addCSSLink.h"

bool addLine(const char* path) {
	std::ifstream ifile(path);
	if (!ifile.is_open()) {
		printf("Could not open file: %s\n", path);
		return false;
	}
	// read file into buffer
	std::vector<char> buffer(
					(std::istreambuf_iterator<char>(ifile)),
					std::istreambuf_iterator<char>()
							);
	ifile.close();
	// get the offset of the </title> tag
	size_t index = (strstr(buffer.data(), "</title>") + 9) -
						buffer.data();
	const char* line = "<link rel='stylesheet' type='text/css' "
						"href='css/php-manual-en.css' />\n";
	//insert @var line into the buffer at @var index
	buffer.insert(buffer.begin()+index, line, line+strlen(line));
	std::ofstream ofile(path);
	if (!ofile.is_open()) {
		printf("Could not save file: %s\n", path);
		return false;
	}
	// save changes
	std::copy(	buffer.begin(), buffer.end(), 
				std::ostreambuf_iterator<char>(ofile)	);
	ofile.close();
	printf("Wrote file: %s\n", path);
	return true;
}

bool loopDir(const char* dirName) {
	DIR* dir;
	struct dirent* ent;
	struct stat fileStats;
	
	// loop through the files in the directory
	if ((dir = opendir(dirName)) != NULL) {
		std::string str = dirName;
		// check to see if there's a slash at the end of the path
		if ('/' != str.at(str.size() - 1))
			str += "/";
		while((ent = readdir(dir)) != NULL) {
			// Update path to the next file.
			std::string path = str + std::string(ent->d_name);
			if (stat(path.c_str(), &fileStats) == -1) {
				printf("Could not stat the given path:\n%s%s",
						path.c_str(), "\n");
				continue;
			}
			// If ent points to a directory, skip it.
			if (S_IFDIR == (fileStats.st_mode & S_IFMT))
				continue;
			else{
			// If file does not match filter, and is not at the 
			// end of @var path, skip it.
				std::regex reg ("^(.*)(\\.[hH][tT][mM][lL])$");
				if (!std::regex_match(path, reg))
					continue;
				else
					addLine(path.c_str());
			}
		}
		closedir(dir);
	} else {
		printf("Sorry, that is not a valid path.\n");
		return false;
	}
	return true;
}

void printHelp() {
	printf("addCSSLink v0.2 June 2017\n"
		"Adds a <link> element to the header element of html file.\n"
		"Sadly, this works quite slowly at the moment.\n"
		"Usage: addCSSLink [DIR]\n"
		"Options:\n"
		"    DIR:   optional. Directory full of html files to change.  \n"
		"           Default is the current directory.  Any non-HTML files\n"
		"           will be ignored.\n"
		);
}

int main(int argc, char** argv) {
	// if a path is specified, do all this.
	if (argc > 1) {
		if (0 == strcmp(argv[1], "--help")) {
			printHelp();
		} else {	// loop through the specified directories
			for(int a=1; a<argc; a++) {
				if (!loopDir(argv[a]))
					printHelp();
			}
        }
	} else {
		loopDir("./");
    }
	return 0;
}
