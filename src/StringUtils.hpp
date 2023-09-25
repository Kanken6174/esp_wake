#include <vector>
#include <tuple>
#include "Arduino.h"

std::vector<String> splitStringBy(const String& text, const char character);
bool extractKeyFromKSV(const String& configContent, const String& key, String& out);