//
// Created by Cem Gokmen on 2/12/18.
//

#include <iostream>
#include "JSONResultExporter.hpp"

void lc3_write_json_suite_report(std::stringstream& oss, lc3_test_suite& suite, const std::string& filename) {
    json arr = json::array();

    for (unsigned int i = 0; i < suite.tests.size(); i++)
    {
        int score, max_score;
        std::string name;
        std::stringstream output;

        output << filename << " - ";

        lc3_test& test = suite.tests[i];
        lc3_get_test_report(name, score, max_score, output, test);
        json output2 = lc3_get_sub_tests_from_test(test);

        arr[i] = {
                {"name", name},
                {"score", score},
                {"max_score", max_score},
                {"passed", test.passed},
                {"output2", output2 },
                {"has_halted", test.has_halted},
                {"has_halted_normally", test.has_halted_normally},
                {"warning", test.warning},
                {"output", output.str()}
        };
    }

    json j = {
            {"filename", filename},
            {"points", suite.points},
            {"max_points", suite.max_points},
            {"passed", suite.passed},
            {"tests", arr}
    };

    oss << j.dump(4);
}

json lc3_get_sub_tests_from_test(lc3_test& test) {
    json arr = json::array();

    for(uint32_t i = 0; i < test.output.size(); i++) {
        lc3_test_output& test_output = test.output[i];

        // build sub test object
        json x;
        x["i"] = i;
        x["input_string"] = lc3_test_output_string(test_output);
        x["passed"] = test_output.passed;
        x["earned"] = test_output.earned;
        x["points"] = test_output.points;
        x["expected"] = test_output.expected;
        x["actual"] = test_output.actual;
        x["extra_output"] = test_output.extra_output;

        // push object onto return list
        arr.push_back(x);
    }

    return arr;
}

void lc3_get_test_report(std::string& name, int& score, int& max_score, std::stringstream& output, lc3_test& test) {
    // Should have name, score, max_score, output
    name = test.name;
    max_score = test.max_points ? test.max_points : 1;
    score = test.max_points ? test.points : (test.passed ? 1 : 0);

    if (!test.has_halted)
        output << "(Did not finish!)\n";
    else if (!test.has_halted_normally)
        output << "(Terminated abnormally!)\n";

    for (unsigned int j = 0; j < test.output.size(); j++)
    {
        lc3_test_output& test_output = test.output[j];
        std::string type = lc3_test_output_string(test_output);

        if (test_output.points)
        {
            output << (j + 1) << " (" << (test_output.passed ? 'P' : 'F') << " " << test_output.earned << " / " << test_output.points <<
                ") " << type << "\n  expected: " << test_output.expected << "\n    actual: " << test_output.actual << "\n";
        }
        else
        {
            output << (j + 1) << " (" << (test_output.passed ? 'P' : 'F') << ") " << type << "\n  expected: " << test_output.expected <<
                "\n    actual : " << test_output.actual << "\n";
        }

        if (!test_output.extra_output.empty())
            output << test_output.extra_output;
    }

    if (test.warnings)
    {
        output << "\nTest completed with " << test.warnings <<" warnings\n";
        output << "-----------------------------------\n";
        output << test.warning;
        output << "\n";
    }

    output << "\n";
}

/*
 * void lc3_write_test_report(std::stringstream& oss, lc3_test_suite& suite, const std::string& filename)
{
    int pass_count = 0;
    int minipass_count = 0;
    int total_minitests = 0;

    oss << "Test suite report for file " << filename << "\n";
    oss << "-------------------------------------\n\n";

    for (unsigned int i = 0; i < suite.tests.size(); i++)
    {
        lc3_test& test = suite.tests[i];
        lc3_write_test_report(oss, test, minipass_count, total_minitests);
        if (test.passed) pass_count++;
    }

    oss << "Summary\n";
    oss << "-------\n";
    oss << "Checks passed: " << minipass_count << " / " << total_minitests << "\n";
    oss << "Tests passed: " << pass_count << " / " << suite.tests.size() << "\n";
    if (suite.max_points) oss << "Grade: " << suite.points << " / " << suite.max_points << "\n";
}

void lc3_write_test_report(std::stringstream& oss, lc3_test& test, int& minipass_count, int& total_minitests)
{

}
 */