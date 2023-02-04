// these are simple-as-possible starting points, low quality code!

// read a file
void ReadFile(char const * const path, bool binary, std::shared_ptr<char> & bufferContent, size_t & bufferSize) {
	auto mode = std::ios::in;
	if (binary) mode = std::ios::binary;
	std::ifstream iFile;

	iFile.open(path, mode);
	iFile.seekg(0, std::ios_base::end);
	bufferSize = iFile.tellg();
	bufferContent = std::make_shared<char>(bufferSize);
	iFile.seekg(0, std::ios_base::beg);
	iFile.read(bufferContent.get(), bufferSize);
	iFile.close();
}

// write to a file
void WriteFile(char const* const path, bool binary, std::shared_ptr<char>& bufferContent, size_t bufferSize) {
	auto mode = std::ios::out;
	if (binary) mode = std::ios::binary;
	std::ofstream oFile;
	oFile.open(path, mode);
	oFile.write(bufferContent.get(), bufferSize);
	oFile.close();
}

// split a string into a vector of sustrings
void Split(std::vector<std::string> & result, const std::string& input, const std::string& splitter) {

    size_t previousPos = 0;
    size_t sLen = splitter.length();
    size_t iLen = input.length();

    for (size_t i = 0; i < iLen; ) {
        if (input[i] == splitter[0] && i + sLen <= iLen && input.substr(i, sLen) == splitter) {
            if (i != previousPos) result.push_back(input.substr(previousPos, i - previousPos));
            previousPos = i + sLen;
            i += sLen;
        }
        else {
            i++;
        }
    }

    if (previousPos < input.length() - 1) result.push_back(input.substr(previousPos));
}

// same thing but instead of moving the value as arg, returns it
std::vector<std::string> Split(const std::string& input, const std::string& splitter) {
    std::vector<std::string> result;

    Split(result, input, splitter);

    return result;
}

// repeated instances of a character are condensed into just one occurance... e.g. condensing spaces will remove all double/triple/etc... spaces from the input string
void CondenseCharacters(std::string& input, const std::string& condensors) {
    int foundChar = -1;
    int condenseChar = -1;
    size_t resPos = 0;

    for (auto i : input) {
        foundChar = -1;
        for (int x = 0; x < condensors.length(); x++) {
            if (i == condensors[x]) {
                foundChar = x;
            }
        }

        // we are currently already condensing a character from condensors string
        if (condenseChar != -1) {
            if (foundChar != -1) { // current character matches a char from condensors
                // and its NOT the character we are already condensing
                if (foundChar != condenseChar) {
                    // add the character we were condensing and start condensing the next one
                    input[resPos++] = condensors[condenseChar];
                    condenseChar = foundChar;
                } // otherwise it MUST be the one we are still condensing, so do nothing
            }
            else { // found no further match from condensors, so we can finish condensing this character
                input[resPos++] = condensors[condenseChar];
                input[resPos++] = i;
                condenseChar = -1;
            }
        }
        else { // we are NOT currently already condensing a character
            if (foundChar != -1) {
                // we just found a character to be condensed, so we start condensing
                condenseChar = foundChar;
            }
            else {
                // havent found a character to be condensed, so just add the character
                input[resPos++] = i;
            }
        }

        
    }

    input.resize(resPos);
}

// char filter
void RemoveCharacters(std::string& input, const std::string& acceptedChars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz\n 0123456789.,%$£^&*+-_=?!/><(){}[]'\"#~@;:") {
    size_t resPos = 0;

    for (auto i : input) {
        for (auto x : acceptedChars) {
            if (i == x) input[resPos++] = i;
        }
    }

    input.resize(resPos);
}

// same with return instead of arg
std::string GetRemovedCharacters(const std::string& input, const std::string& acceptedChars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz\n 0123456789.,%$£^&*+-_=?!/><(){}[]'\"#~@;:") {
    std::string result = input;

    RemoveCharacters(result, acceptedChars);

    return result;
}
