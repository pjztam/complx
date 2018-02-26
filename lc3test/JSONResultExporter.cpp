//
// Created by Cem Gokmen on 2/12/18.
//

#include "JSONResultExporter.hpp"

void lc3_write_json_suite_report(std::stringstream& oss, lc3_test_suite& suite, const std::string& filename) {
    json arr = json::array();

    for (unsigned int i = 0; i < suite.tests.size(); i++)
    {
        lc3_test& test = suite.tests[i];

        json test_obj = json::object();
        lc3_get_test_report(test_obj, test);

        arr[i] = test_obj;
    }

    json j = {
            {"tests", arr}
    };

    oss << j.dump(4);
}

void lc3_get_test_report(json &j, lc3_test &test) {
    // Should have name, score, max_score, output

    j["name"] = test.name;
    j["max_score"] = test.max_points ? test.max_points : 1;
    j["score"] = test.max_points ? test.points : (test.passed ? 1 : 0);
    j["num_warnings"] = test.warnings;

    std::stringstream output;
    json warnings = json::array();

    if (!test.has_halted)
        output << "(Did not finish!)\n";
    else if (!test.has_halted_normally)
        output << "(Terminated abnormally!)\n";

    for (unsigned int i = 0; i < test.output.size(); i++)
    {
        lc3_test_output& test_output = test.output[i];
        std::string type = lc3_test_output_string(test_output);

        if (test_output.points)
        {
            output << (i + 1) << " (" << (test_output.passed ? 'P' : 'F') << " " << test_output.earned << " / " << test_output.points <<
                ") " << type << "\n  expected: " << test_output.expected << "\n    actual: " << test_output.actual << "\n";
        }
        else
        {
            output << (i + 1) << " (" << (test_output.passed ? 'P' : 'F') << ") " << type << "\n  expected: " << test_output.expected <<
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

    j["output"] = output.str();
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