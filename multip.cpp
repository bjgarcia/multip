// ReadMultipart.cpp : This file will read out the next file in a multipart form
// and save it using the file path passed in. It will return the end position
// of that file with in the file or -1 if last file.

#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>

int main(int argc, char* argv[])
{
	std::string boundary;
	std::string in_file;
	std::string out_file;
	size_t file_pos = std::ios::end;

	if (argc == 5) {
		boundary = argv[1];
		in_file = argv[2];
		out_file = argv[3];
		file_pos = std::stoul(argv[4]);
	}

	std::ifstream input_file(in_file, std::ios::binary);
	if (!input_file.is_open()) {
		std::cerr << "Error opening file for binary reading: " << in_file << std::endl;
		return 1;
	}

	input_file.seekg(file_pos);
	std::string s_string = "\r\n\r\n";
	std::vector<char> buffer(s_string.size());
	while (input_file.read(buffer.data(), buffer.size())) {
		if (std::string(buffer.begin(), buffer.end()) == s_string) {
			file_pos = input_file.tellg();
			break;
		}
		input_file.seekg(++file_pos);
	}

	std::ofstream output_file(out_file, std::ios::binary);
	if (!output_file.is_open()) {
		std::cerr << "Error opening file for binary reading: " << out_file << std::endl;
		return 1;
	}

	input_file.seekg(file_pos, std::ios::beg);
	buffer.resize(4096);
	s_string = boundary;

	std::vector<char>::iterator it;
	while (true) {
		input_file.read(buffer.data(), buffer.size());
		long write_size = input_file.gcount();

		if (write_size > 0) {
			auto it = std::search(buffer.begin(), buffer.end(), s_string.begin(), s_string.end());
			if (it != buffer.end()) {
				write_size = std::distance(buffer.begin(), it);
				output_file.write(buffer.data(), write_size);
				file_pos = input_file.tellg();
				goto endSection;
			}
			output_file.write(buffer.data(), write_size);
			file_pos = input_file.tellg();
		}

		if (input_file.eof()) {
			break;
		}
		else if (input_file.fail()) {
			std::cerr << "Error reading file." << std::endl;
			break;
		}
	}
	endSection:
	input_file.close();
	output_file.close();
	if (file_pos >= std::ios::end) {
		file_pos = -1;
	}
	return file_pos;
}
