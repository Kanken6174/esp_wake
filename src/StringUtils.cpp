#include "StringUtils.hpp"

std::vector<String> splitStringBy(const String& text, const char character) {
    std::vector<String> result;
    int startIndex = 0;
    int endIndex = 0;

    while (endIndex != -1) {
        endIndex = text.indexOf(character, startIndex);
        if (endIndex == -1) {
            result.push_back(text.substring(startIndex));
        } else {
            result.push_back(text.substring(startIndex, endIndex));
            startIndex = endIndex + 1;
        }
    }

    return result;
}

bool extractKeyFromKSV(const String& configContent, const String& key, String& out) {
  String searchKey = key + ";";
  int startIndex = 0;

  while (startIndex < configContent.length()) {
    int lineEndIndex = configContent.indexOf('\n', startIndex);
    if (lineEndIndex == -1) {
      break;  // Reached the end of the content
    }

    String line = configContent.substring(startIndex, lineEndIndex);
    std::vector<String> parts = splitStringBy(line, ';');

    if (parts.size() == 2 && parts[0] == searchKey) {
      out = parts[1];
      out.trim();  // Remove leading/trailing whitespace
      return true;
    }

    startIndex = lineEndIndex + 1;
  }

  return false;
}