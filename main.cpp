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
  std::string json_dir = "";
  std::string json_file_path = "";
  std::string compiler_command = "gcc";

  if (argc > 3)
  {
    json_file_path = std::string(argv[1]) + std::string(argv[2]);
    in_comp_db_json.open(json_file_path);
    json_dir = argv[1];
    compiler_command = argv[3];
  }
  else
  {
    std::cout << std::endl << "Using: ./include-explorer path/to/json/file/dir/ json_file_name.json compiler_command" << std::endl;
    std::cout << "Example : ./include-explorer test compile_commands.json gcc" << std::endl << std::endl;
    return 0;
  }

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


  makefile << "CC=" << compiler_command << std::endl << std::endl;
  makefile << "all:" << std::endl;


  std::string command = "";
  for (auto& car : a.GetArray()) {
  	makefile << "\t";

  	//path to sources where "bear make" had been launched first time
  	makefile << "cd " << json_dir << " && ";
  	makefile << "$(CC)";
  	command = car["command"].GetString();
      makefile << command.substr(2);
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
