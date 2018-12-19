#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/istreamwrapper.h"
#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>

using namespace rapidjson;

int main(int argc, char* argv[]) {

  /* modify json file received from bear
	to parse it by means of rapidjson.
	During modification we just assign a name to 
	array of objects created by bear.
	Output file will be used for parsing with rapidjson.
   */

  std::ifstream in_comp_db_json;
  std::string line = "";
  std::ofstream out_comp_db_json("tmpJson.json");
  std::string json_dir = "./";
  std::string json_file_path = "./compile_commands.json";
  std::string compiler_command = "";
  std::string json_compiler_name = "";

  //parse arguments for options
  if (argc > 1)
  {
    for (int i = 1; i < argc; i++ )
    {
      std::string argument_str = std::string(argv[i]);
      if (argument_str.compare("-f") == 0)
      {
        json_file_path = std::string(argv[i+1]);
        
        std::size_t found = json_file_path.find_last_of("/");
        json_dir = json_file_path.substr(0,found);
      }
      else if (argument_str.compare("-p") == 0)
      {
        compiler_command = std::string(argv[i+1]);
      }
      else if (argument_str.compare("-h") == 0)
      {
        std::cout << std::endl << "Using: ./include-explorer [-f path/to/json/file/dir/json_file_name.json] [-p compiler_prefix]" << std::endl;
        std::cout << "-f - full path to json file with compile commands. Default: \"./compile_commands.json\"" << std::endl;
        std::cout << "-p - full path to compiler prefix. Default: \"\"" << std::endl;
        std::cout << "Example : ./include-explorer -f test/compile_commands.json -p /opt/arm-linux-gnueabihf/arm-linux-gnueabihf-" << std::endl << std::endl;
        return -1;
      }
    }
  }
  else
  {
      std::cout << std::endl << "**************   include-explorer use defaults values   **************" << std::endl << std::endl;
      std::cout << "-f = \"./compile_commands.json\"" << std::endl;
      std::cout << "-p = \"\"" << std::endl;
      std::cout << std::endl << "**********************************************************************" << std::endl << std::endl;
  }

  in_comp_db_json.open(json_file_path);

  if (in_comp_db_json.is_open())
  {
    out_comp_db_json << "{" << std::endl;
    out_comp_db_json << "\"commands\":";

    while ( getline (in_comp_db_json,line) )
    {
       out_comp_db_json << line << std::endl;
    }
    
    out_comp_db_json << std::endl << "}";
    
    in_comp_db_json.close();
    out_comp_db_json.close();
  }
  else std::cout << "Unable to open file"; 


  //Parse output file by rapidjson.
  //Modify commands by adding cross-compiler name and option -H
  //Put new commands to Makefile to perform this command.
  std::ifstream ifs("tmpJson.json");
  IStreamWrapper isw(ifs);
  Document documentFromFile;

  documentFromFile.ParseStream(isw);

  // Using a reference for consecutive access is handy and faster.
  const Value& a = documentFromFile["commands"];
  assert(a.IsArray());

  std::ofstream makefile("tmpMakefile");


  makefile << "CC=" << compiler_command << "gcc" << std::endl << std::endl;
  makefile << "CXX=" << compiler_command << "g++" << std::endl << std::endl;
  makefile << "all:" << std::endl;


  std::string command = "";
  for (auto& record : a.GetArray()) {
  	makefile << "\t";

  	//path to sources where "bear make" had been launched first time
  	makefile << "cd " << json_dir << " && ";

    command = record["command"].GetString();
    
    json_compiler_name = command.substr(0,3);
    if (json_compiler_name.compare("cc ") == 0)
    {
      makefile << "$(CC)";
    }
    else if (json_compiler_name.compare("c++") == 0)
    {
      makefile << "$(CXX)";
    }
    else
    {
      std::cout << "Error. Unknown compiler command in json file: " << json_compiler_name << std::endl;
      return -1;
    }

    //process -include option
    size_t search_start_pos = 0;
    size_t inc_opt_start_pos = 0;
    size_t inc_filename_start_pos = 0;
    size_t inc_filename_end_pos = 0;
    std::string search_pattern = std::string(" -include ");
    std::string inc_filename = "";
    while ((inc_opt_start_pos = command.find(search_pattern, search_start_pos)) != std::string::npos)
    {
      inc_filename_start_pos = command.find_first_not_of(" ",inc_opt_start_pos + 10);
      inc_filename_end_pos = command.find_first_of(" ",inc_filename_start_pos + 1);
      inc_filename = command.substr(inc_filename_start_pos, inc_filename_end_pos - inc_filename_start_pos);
      //inc_filename = command.substr(inc_filename_start_pos, 10);
      search_start_pos = inc_opt_start_pos + 5;
      std::cout << inc_filename << std::endl;
    }

    //process \" in macro definition in gcc command (-D=\"Hello World\")
    search_start_pos = 0;
    size_t start_pos = 0;
    search_pattern = std::string("\\\"");
    while ((start_pos = command.find("\\\"", search_start_pos)) != std::string::npos)
    {
      command.replace(start_pos,2,"\"\\\"");
      search_start_pos = start_pos + 2;
    }

    //write the rest of the command
    makefile << " ";
    makefile << command.substr(3);
    makefile << " -H ";
    makefile << "-fsyntax-only ";
    makefile << std::endl;
  }

  makefile.close();
  
  //Perform generated Makefile

  //show includes that directly comes from gcc command
  //because they are not shown after preprocessing or after using -H option

  std::cout << std::endl;
  std::cout << "Includes that come directly from gcc command:" << std::endl;

  system("make -f tmpMakefile 2>&1 |  grep --color \"\\-include\"");
  
  std::cout << std::endl;
  std::cout << "First level includes (includes that are in source file compiled with gcc):" << std::endl;
  
  //show first level includes, i.e. includes that are in source file compiled with gcc
  system("make -f tmpMakefile 2>&1 | grep '^\\. ' | sort -u");
  

  std::cout << std::endl;
  std::cout << "Other levels includes:" << std::endl;
  //show other levels includes that contains some keyword (for example "lima")
  //system("make -f tmpMakefile 2>&1 | grep '^\\.\\+ ' | grep lima | sort -u");
  system("make -f tmpMakefile 2>&1 | grep '^\\..\\+ ' | sort -u");

  std::cout << std::endl;
  std::cout << "Finish. To see full output run 'make -f tmpMakefile'." << std::endl;
  
  return 1;

}
