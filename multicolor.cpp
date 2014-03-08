#include <iostream>
#include <vector>
#include <sstream>
#include "clife.hpp"
#include <unistd.h>
#include <openssl/md5.h>
#include <stdlib.h>
#include <time.h>
#include <cassert>

/** Written by Sjors Gielen, eth0 winter 2014
 *  Feel free to use this for anything you like
 */

struct MulticolorValue {
	bool value;
	char red;
	char green;
	char blue;

	MulticolorValue() : value(false), red(0), green(0), blue(0) {}
	MulticolorValue(std::vector<MulticolorValue> vec) : value(true) {
		int sum_red = 0;
		int sum_green = 0;
		int sum_blue = 0;
		int sum = 0;
		std::vector<MulticolorValue>::iterator it;
		for(it = vec.begin(); it != vec.end(); ++it) {
			if(it->value) {
				sum_red += it->red;
				sum_green += it->green;
				sum_blue += it->blue;
				sum++;
			}
		}
		assert(sum == 3);
		red = sum_red / sum;
		green = sum_green / sum;
		blue = sum_blue / sum;
	}
	MulticolorValue(bool value) : value(value), red(0), green(0), blue(0) {
		if(value) {
			char color = rand() % 3;
			red = color == 0 ? 255 : 0;
			green = color == 1 ? 255 : 0;
			blue = color == 2 ? 255 : 0;
		}
	}

	operator bool() const { return value; }
	std::string hash() const {
		if(value) {
			return std::string("1") + red + green + blue;
		} else {
			return "0\x00\x00\x00";
		}
	}

	char getChar() const {
		return (value ? 'o' : ' ');
	}

	void print(std::ostream &os) const {
		if(value) {
			os << red << green << blue;
		} else {
			os << '\x00' << '\x00' << '\x00';
		}
	}
};

template <typename FieldType>
void check_stop_condition(FieldType field, std::vector<std::string> &earlier_hashes, bool &done, int &repeats_to_do) {
	std::string hash = field.field_hash();
	for(int i = 0; i < earlier_hashes.size(); ++i) {
		if(earlier_hashes[i] == hash) {
			done = true;
			repeats_to_do = 10;
			break;
		}
	}
	earlier_hashes.push_back(hash);
}

int main(int argc, char *argv[]) {
	int microsleeptime = 100000;
	if(argc == 2) {
		std::stringstream ss;
		ss << argv[1];
		ss >> microsleeptime;
		if(!ss) {
			std::cerr << "Usage: " << argv[0] << " [microsleeptime]" << std::endl;
		}
	} else if(argc > 2) {
		std::cerr << "Usage: " << argv[0] << " [microsleeptime]" << std::endl;
	}

	srand(time(NULL));
	std::vector<std::string> earlier_hashes;
	GameOfLifeField<MulticolorValue, 8, 80> field;

	field.generateRandom(35);

	bool field_done = false;
	int repeats_to_do = 0;
	field.print_simple(std::cout);
	while(!field_done || repeats_to_do > 0) {
		if(repeats_to_do > 0) {
			--repeats_to_do;
		}
		field.nextState();
		field.print_simple(std::cout);
		if(!field_done) {
			check_stop_condition(field, earlier_hashes, field_done, repeats_to_do);
		}
		usleep(microsleeptime);
		continue;
	}
}
