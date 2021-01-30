#include "../include/lexer.h"
#include "../include/parser.h"
#include "../include/visitor.h"
#include <jsonlib.h> // https://github.com/longwatermelon/simple-json-lib
#include <fstream>
#include <sstream>
#include <iostream>


std::string read_file(std::string fp)
{
    std::ifstream infile;
    infile.open(fp);
    std::stringstream contents;
    std::string line;
    while (std::getline(infile, line)) contents << line << "\n";
    
    infile.close();
    return contents.str();
}


void run_file(Parser* parser, Visitor* visitor, std::string contents)
{
    std::shared_ptr<AST> root = parser->parse();
    visitor->visit(root);
}


int main(int argc, char* argv[])
{
    // load optional hampp configurations
    const int config_size = 2;

    std::string config_path = ".";
    if (argc >= 3) { config_path = argv[2]; }


    std::map <std::string, std::string> config{
        {"version", "default"},
        {"platform", "default"}
    };

    try
    {
        config = json::load<std::string, std::string>(config_path + "/hamppconfig.json");
        if (config.size() != config_size)
        {
            std::cout << "\x1B[31mThe configuration file should contain version and platform.\nFor default configuration, set both values to \"default\".\x1B[0m\n";
            config["version"] = "default";
            config["platform"] = "default";
        }
    }
    catch (const std::runtime_error&)
    {
        std::cout << "\x1B[33mWarning: Failed to read hampp configuration file (\"hamppconfig.json\") in current directory.\033[0m\n";
        std::cout << "\x1B[33mSpecify a path to a configuration file when running hampp or create a configuration file in the current directory.\033[0m\n";
    }

    std::string contents = read_file(argv[1]);
    Parser parser(contents);
    Visitor visitor{config};
   
    try
    {
        run_file(&parser, &visitor, contents);
    }
    catch (const std::runtime_error& ex)
    {
        std::cout << ex.what();
    }

    return 0;
}