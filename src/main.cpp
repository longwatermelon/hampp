#include "../include/lexer.h"
#include "../include/parser.h"
#include "../include/visitor.h"
#include "../include/colors.h"
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
    const int config_size = 1;

    std::string config_path = ".";
    if (argc >= 3) { config_path = argv[2]; }


    std::map <std::string, std::string> config{
        {"version", "default"}
    };

    try
    {
        config = json::load<std::string>(config_path + "/hamppconfig.json");
        colors::version = config["version"];

        if (config.size() != config_size)
        {
            std::cout << colors::get_color("red") << "The configuration file should contain version.\nFor default configuration, set version to \"default\"."
                << colors::get_color("reset") << "\n";
            config["version"] = "default";
            colors::version = "default";
        }
    }
    catch (const std::runtime_error&)
    {
        std::cout << colors::get_color("yellow") <<  "Warning: Failed to read hampp configuration file (\"hamppconfig.json\") in current directory.\n";
        std::cout << "Specify a path to a configuration file when running hampp or create a configuration file in the current directory." << colors::get_color("reset") << "\n";
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
        std::cout << "Error in file " << colors::get_color("yellow") << argv[1] << colors::get_color("reset") << ex.what();
    }

    return 0;
}