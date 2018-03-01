#include <lc3_all.hpp>

#include <algorithm>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>
#include <vector>

#include <wx/tokenzr.h>
#include <wx/xml/xml.h>
#include <wx/wx.h>

#include <XmlTestParser.hpp>
#include <JSONResultExporter.hpp>

struct suite_summary
{
    suite_summary() : grade(0), tests(0), executions(0), warnings(0) {}
    int grade;
    int tests;
    int executions;
    int warnings;
};

unsigned int digits(unsigned int v)
{
    return 1 + ((v >= 1000000000) ? 9 : (v >= 100000000) ? 8 : (v >= 10000000) ? 7 :
                (v >= 1000000) ? 6 : (v >= 100000) ? 5 : (v >= 10000) ? 4 :
                (v >= 1000) ? 3 : (v >= 100) ? 2 : (v >= 10) ? 1 : 0);
}

int main(int argc, char** argv)
{
    // Typical
    if (argc < 3)
    {
        printf("Usage: lc3test testfile.xml asmfile.asm -random_seed=int -runs=int -result=filename.json\n");
        return EXIT_FAILURE;
    }

    lc3_test_suite suite;
    std::string xmlfile = argv[1];
    std::string asmfile = argv[2];
    int random_seed = -1;
    unsigned int runs = 1;
    std::string output_json = "INVALID";

    for (int i = 3; i < argc; i++)
    {
        const std::string& arg = argv[i];
        if (arg.find("-random_seed=") != std::string::npos)
            random_seed = atoi(arg.substr(arg.find("=") + 1).c_str());
        else if (arg.find("-runs=") != std::string::npos)
            runs = (unsigned int) atoi(arg.substr(arg.find("=") + 1).c_str());
        else if (arg.find("-result=") != std::string::npos)
            output_json = arg.substr(arg.find("=") + 1).c_str();
        else
        {
            fprintf(stderr, "Invalid param %s passed in!", arg.c_str());
            return EXIT_FAILURE;
        }
    }

    try
    {
        if (!XmlTestParser().LoadTestSuite(suite, xmlfile))
        {
            fprintf(stderr, "Xml file not found or parse errors found\n");
            return EXIT_FAILURE;
        }
    }
    catch (XmlTestParserException x)
    {
        fprintf(stderr, "%s\n", x.what().c_str());
        return EXIT_FAILURE;
    }

    std::vector<suite_summary> results;
    int num_passed = 0;

    lc3_test_suite test_suite;
    std::stringstream oss_json;
    for (unsigned int i = 0; i < runs; i++)
    {
        test_suite = suite;
        try
        {
            lc3_run_test_suite(test_suite, asmfile, random_seed, i);
        }
        catch (const char* x)
        {
            fprintf(stderr, "%s\n", x);
            return EXIT_FAILURE;
        }
        catch (std::string x)
        {
            fprintf(stderr, "%s\n", x.c_str());
            return EXIT_FAILURE;
        }


        std::stringstream oss;
        lc3_write_test_report(oss, test_suite, asmfile);
        lc3_write_json_suite_report(oss_json, test_suite, asmfile);
        if (runs > 1)
            std::cout << "Run " << (i + 1) << ": ";
        std::cout << oss.str() << "\n";

        if (random_seed != -1) random_seed++;
        if (test_suite.passed) num_passed++;

        suite_summary summary;
        summary.grade = test_suite.points;
        for (const auto& test : test_suite.tests)
        {
            if (test.passed) summary.tests++;
            summary.warnings += test.warnings;
            summary.executions += test.executions;
        }
        results.push_back(summary);
    }

    if (runs > 1)
    {
        std::cout << "Overall results\n";
        std::cout << "----------------\n\n";
        for (unsigned int i = 0; i < results.size(); i++)
        {
            const auto& summary = results[i];
            std::cout << "Run " << std::setw(digits(runs)) << (i+1) << " Grade: " << std::setw(3) << summary.grade << "/" << test_suite.max_points << " Tests Passed: " << summary.tests << "/" << test_suite.tests.size() << " Executions: " << summary.executions << " Warnings: " << summary.warnings << "\n";
        }
    }

    std::cout << "If you have any questions about the output, then please include the full output generated by this program in your email / piazza post!\n";

    // write output json to file if -result command line parameter specified
    if(output_json.compare("INVALID") != 0) {
        std::cout << "Writing result JSON to " << output_json << std::endl;
        std::fstream fs;
        fs.open (output_json, std::fstream::out);
        fs << oss_json.rdbuf();
        fs.close();
    }
    return EXIT_SUCCESS;
}