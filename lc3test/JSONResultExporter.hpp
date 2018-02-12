//
// Created by Cem Gokmen on 2/12/18.
//

#ifndef COMPLX_TOOLS_JSONRESULTEXPORTER_H
#define COMPLX_TOOLS_JSONRESULTEXPORTER_H

#include <string>
#include "lc3_test.hpp"
#include "json.hpp"

using json = nlohmann::json;

void lc3_write_json_suite_report(std::stringstream& oss, lc3_test_suite& suite, const std::string& filename);

void lc3_get_test_report(std::string& name, int& score, int& max_score, std::stringstream& output, lc3_test& test);

#endif //COMPLX_TOOLS_JSONRESULTEXPORTER_H
