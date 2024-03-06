module;

#include "pch.h"

export module mod_strtools;

export namespace DE::StringTools {
    std::string str_trim(std::string line) {
        // leading
        line.erase(line.begin(), std::find_if_not(line.begin(), line.end(), ::isspace));
        // trailing
        line.erase(std::find_if_not(line.rbegin(), line.rend(), ::isspace).base(), line.end());

        return line;
    }

    /// <summary>
    /// Converts a string of space deliminated number values into an array of strings
    /// </summary>
    /// <param name="line"></param>
    /// <returns></returns>
    std::vector<std::string> str_split(std::string line, char split_at = ' ') {
        std::istringstream iss(line);
        std::vector<std::string> values;

        std::string token;

        while (std::getline(iss, token, split_at)) {
            //uint32_t num = std::stoul(token);
            values.push_back(token);
        }

        return values;
    }

    /// <summary>
    /// Converts a string of space deliminated number values into an array of strings
    /// </summary>
    /// <param name="line"></param>
    /// <returns></returns>
    std::vector<int> str_split_to_int(std::string line, char split_at = ' ') {
        std::istringstream iss(line);
        std::vector<int> values;

        std::string token;
        while (std::getline(iss, token, split_at)) {
            int num = std::stoi(token);
            values.push_back(num);
        }

        return values;
    }

    /// <summary>
    /// Converts a string of space deliminated number values into an array of strings
    /// </summary>
    /// <param name="line"></param>
    /// <returns></returns>
    std::vector<uint32_t> str_split_to_uint(std::string line, char split_at = ' ') {
        std::istringstream iss(line);
        std::vector<uint32_t> values;

        std::string token;
        while (std::getline(iss, token, split_at)) {
            uint32_t num = std::stoul(token);
            values.push_back(num);
        }

        return values;
    }

    std::string str_normalize_whitespace(std::string line) {
        std::string result;

        bool is_prev_white = false;
        for (int i = 0; i < line.size(); ++i) {
            if (std::isspace(line[i])) {
                if (!is_prev_white) {
                    result += ' ';
                    is_prev_white = true;
                }
            }
            else {
                result += line[i];
                is_prev_white = false;
            }
        }

        return result;
    }
}