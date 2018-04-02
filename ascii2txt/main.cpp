#include <iostream>
#include <fstream>
#include <vector>

using namespace std;
typedef unsigned char byte;

void bin2Txt(string& buffer, char* out) {
	byte i = '0';
	for (unsigned long spot=0; spot<buffer.size(); spot+=8) {
		out[spot / 8] = '\0';
		for (unsigned short s=0; s<8; s++)
			out[spot / 8] ^= ( i ^ buffer[s + spot] ) << (7 - s);
	}
}

int main(int argc, char* argv[])
{
	if (argc != 2) {
		cerr << "One argument please" << endl;
		return -3;
	} else {
		fstream file (argv[1]);
		if (file.is_open()) {
			string buffer(
					(istreambuf_iterator<char>(file)),
					(istreambuf_iterator<char>())
									);
            // output will be 1/8 the size of the input
			byte div = 8;
			char* out = new (nothrow) char[ (buffer.size() / div) + 1 ];
			if (out == nullptr) {
				cerr << "Memory could not be allocated" << endl;
				return -2;
			}
            // null terminate string
            out[buffer.size() / div] = '\0';
            
			bin2Txt(buffer, out);
			cout << out;
			delete[] out;
		} else {
			cerr << "File could not be opened" << endl;
			return -1;
		}
	}
	return 0;
}
