#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/istreamwrapper.h"
#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>


using namespace rapidjson;

typedef struct includeFile {
	int level;
	int is_system;
	std::string path;
	int parent_num;
} includeFile;

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

	std::ifstream file_to_analyze;
	std::ofstream result_file("include-explorer-result.txt");
	std::ofstream direct_includes_file("include-explorer-direct-includes.txt");
	std::string file_to_analyze_path = "./tmpMakefileResult.txt";
	int analyze_flag = 0;
	int processing_includes = 0;

	size_t cd_start_pos = 0;
	size_t file_path_start_pos = 0;
	size_t file_path_end_pos = 0;
	std::string file_path = "";

	size_t include_file_start_pos = 0;
	size_t include_file_end_pos = 0;
	std::string include_file_path = "";

	int include_file_is_system = 0;
	int include_file_level_num = 0;
	std::string include_file_level_str;
	int prev_include_file_level_num = 0;
	std::string prev_include_file_level_str;
	int prev_include_file_is_system = 0;
	std::string target_directory; //path to directory (part of it or full path) that contains analyzed project
	int include_level_to_process = 1; //path to directory (part of it or full path) that contains analyzed project

	std::ofstream cp_command_file("cp_source_files.sh");

	int i = 0;

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
			else if (argument_str.compare("-a") == 0)
			{
				analyze_flag = 1;
				file_to_analyze_path = std::string(argv[i+1]);
			}
			else if (argument_str.compare("-d") == 0)
			{
				if (analyze_flag == 0)
				{
					std::cout << "Error: -d option is used only with -a option" << std::endl;
					return -1;
				}
				target_directory = std::string(argv[i+1]);
			}
			else if (argument_str.compare("-l") == 0)
			{
				if (analyze_flag == 0)
				{
					std::cout << "Error: -l option is used only with -a option" << std::endl;
					return -1;
				}
				include_level_to_process = atoi(argv[i+1]);
			}
			else if (argument_str.compare("-h") == 0)
			{
				std::cout << std::endl << "Using: ./include-explorer [-f path/to/json/file/dir/json_file_name.json] [-p compiler_prefix] [-a tmpMakefileResult.txt] " << std::endl;
				std::cout << "[-d part/of/project/directory/path] [-l include]" << std::endl;
				std::cout << "-f - full path to json file with compile commands. Default: \"./compile_commands.json\"" << std::endl;
				std::cout << "-p - full path to compiler prefix. Default: \"\"" << std::endl;
				std::cout << "-a - analyze result file Default: \"./tmpMakefileResult.txt\"" << std::endl;
				std::cout << "-d - part of project directory path. Default: \"\"" << std::endl;
				std::cout << "     If some include file path contains part/of/project/directory/path then it is supposed that the file is in the project" << std::endl;
				std::cout << "-l - level of include file to process. Default: \"1\"" << std::endl;
				std::cout << "Example : ./include-explorer -f test/compile_commands.json -p /opt/arm-linux-gnueabihf/arm-linux-gnueabihf-" << std::endl;
				std::cout << "Example : ./include-explorer -a ./tmpMakefileResult.txt -d include-explorer/test -l 1" << std::endl << std::endl;
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

	if (!analyze_flag)
	{
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
		else std::cout << "include-explorer error! Unable to open file:" << json_file_path << std::endl;


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
		std::string directory = "";
		std::string file = "";
		//int commandsCount = a.Size(); ////to output file number
		for (auto& record : a.GetArray()) {

			//makefile << "\t" << "@echo " << i++ << "/" << commandsCount << std::endl;     //to output file number

			std::string file = record["file"].GetString();
			directory = record["directory"].GetString();

			if (file.find("/") != 0) //relative file name
				file = directory + "/" + file;



			makefile << "\t" << "@echo \"file: " << file << "\"" << std::endl;     //to output file number
			makefile << "\t";

			makefile << "cd " << directory << " && ";

			//read command from json file
			int json_has_command_field = 0;
			command = "";

			if (record.HasMember("command"))
			{
				command = record["command"].GetString();
				json_has_command_field = 1;
			}
			else if (record.HasMember("arguments"))
			{
				const Value& arguments = record["arguments"];
				assert(arguments.IsArray());
				for (SizeType i = 0; i < arguments.Size(); i++) // Uses SizeType instead of size_t
				{
					std::string arg = arguments[i].GetString();

					//process "\"" in -D here if record.HasMember("arguments")
					if (arg.find("-D",0) == 0)
					{
						size_t search_start_pos = 0;
						size_t start_pos = 0;
						std::string search_pattern = std::string("\"");
						while ((start_pos = arg.find(search_pattern, search_start_pos)) != std::string::npos)
						{
							arg.replace(start_pos,1,"\"\\\"");
							search_start_pos = start_pos + 3;
						}

					}
					command = command + arg + " ";
				}
			}
			else
			{
				std::cout << "JSON file does not contain \"command\" field or \"arguments\" field." << std::endl;
				return 0;
			}

			//write gcc/g++ command instead cc/c++
			if (json_has_command_field == 1)
			{
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
			}

			//show includes that directly comes from gcc command
			//because they might not been shown after preprocessing or after using -H option
			//process -include option
			size_t search_start_pos = 0;
			size_t inc_opt_start_pos = 0;
			size_t inc_filename_start_pos = 0;
			size_t inc_filename_end_pos = 0;
			std::string search_pattern = std::string(" -include ");
			std::string inc_filename = "";

			if (command.find(search_pattern, search_start_pos) != std::string::npos)
				direct_includes_file << "command: " << command << std::endl;

			while ((inc_opt_start_pos = command.find(search_pattern, search_start_pos)) != std::string::npos)
			{
				inc_filename_start_pos = command.find_first_not_of(" ",inc_opt_start_pos + 10);
				inc_filename_end_pos = command.find_first_of(" ",inc_filename_start_pos + 1);
				inc_filename = command.substr(inc_filename_start_pos, inc_filename_end_pos - inc_filename_start_pos);
				//inc_filename = command.substr(inc_filename_start_pos, 10);
				search_start_pos = inc_opt_start_pos + 5;
				direct_includes_file << inc_filename << std::endl;
			}

			//process "\"" in -D here if record.HasMember("commands")
			//process \" in macro definition in gcc command (-D=\"Hello World\")
			if (json_has_command_field == 1)
			{
				search_start_pos = 0;
				size_t start_pos = 0;
				search_pattern = std::string("\\\"");
				while ((start_pos = command.find("\\\"", search_start_pos)) != std::string::npos)
				{
					command.replace(start_pos,2,"\"\\\"");
					search_start_pos = start_pos + 2;
				}
			}

			//write the rest of the command
			if (json_has_command_field == 1)
			{
				makefile << " ";
				makefile << command.substr(3);
			}
			else
			{
				makefile << command;
			}
			makefile << " -H ";
			makefile << "-fsyntax-only ";
			makefile << std::endl;
		}

		direct_includes_file.close();
		makefile.close();

		//Perform generated Makefile
		system("make -f tmpMakefile > tmpMakefileResult.txt 2>&1");

		std::cout << std::endl;
		std::cout << "Finish." << std::endl;
	}
	else //analyze_flag = 1
	{
		includeFile includeFilesTree [1024]; //array to save includes as tree
		int includeFilesNum = 0;
		int parent_num = 0;

		file_to_analyze.open(file_to_analyze_path);

		if (file_to_analyze.is_open())
		{

			cp_command_file << "rm -rf compiled_sources" << std::endl;
			cp_command_file << "mkdir -p compiled_sources" << std::endl;

			while ( getline (file_to_analyze,line) )
			{
				if (line.find ("file: ") == 0)
				{
					result_file << std::endl << std::endl;
					result_file << line << std::endl;

					cp_command_file << "cp --parent " << line.substr(6) << " compiled_sources" << std::endl; //"file: " length = 6

					//get path from cd command
					getline (file_to_analyze,line);
					cd_start_pos = line.find("cd", 0);
					file_path_start_pos = line.find_first_of(" ",0) + 1;
					file_path_end_pos = line.find_first_of(" ",file_path_start_pos);
					file_path = line.substr(file_path_start_pos, file_path_end_pos - file_path_start_pos);

					processing_includes = 1;
					includeFilesNum = 0; //clear counter of includes in includeFilesTree
					continue;
				}
				else if (line.find ("Multiple ") != std::string::npos)
				{
					processing_includes = 0;
					for (int i = 0; i < includeFilesNum; i++)
					{
						if ((!includeFilesTree[i].is_system) ||
								(includeFilesTree[i].is_system && (!includeFilesTree[includeFilesTree[i].parent_num].is_system)) ||
								(includeFilesTree[i].is_system && (includeFilesTree[i].level == 1)) ||
								(includeFilesTree[i].level <= include_level_to_process)
							)
						{
							/*debug print
							result_file << includeFilesTree[i].is_system << " ";
							result_file << includeFilesTree[i].level << " ";
							result_file << includeFilesTree[i].parent_num << " ";
							*/
							result_file << includeFilesTree[i].path << " ";
							result_file << std::endl;

							cp_command_file << "cp --parent " << includeFilesTree[i].path << " compiled_sources" << std::endl;
						}
					}
				}

				if(!processing_includes)
					continue;
				else //processing include file
				{

					//get include file path
					include_file_start_pos = line.find_first_of(" ",0) + 1;
					include_file_end_pos = line.find_first_of(" ",include_file_start_pos);
					include_file_path = line.substr(include_file_start_pos, include_file_end_pos - include_file_start_pos);
					if (include_file_path.find("/") != 0) //add directory path if include file path is relative
						include_file_path = file_path + "/" + include_file_path;

					//get include file level
					include_file_level_str = line.substr(0,line.find_first_of(" ",0));
					include_file_level_num = include_file_level_str.length();
					//result_file << include_file_level_str << " " << include_file_level_num <<  std::endl;

					//define if file is in target directory
					if (include_file_path.find(target_directory,0) != std::string::npos)
					{
						include_file_is_system = 0;
					}
					else
					{
						include_file_is_system = 1;
					}

					//calculate parent_num
					if (includeFilesNum == 0)
					{
						parent_num = 0;
					}
					else
					{
						for (int i = includeFilesNum - 1; i > 0; i--)
						{
							if (includeFilesTree[i].level == include_file_level_num)
							{
								parent_num = includeFilesTree[i].parent_num;
								break;
							}
							else if (include_file_level_num == (includeFilesTree[i].level + 1))
							{
								parent_num = i;
								break;
							}
						}
					}

					includeFilesTree[includeFilesNum].level = include_file_level_num;
					includeFilesTree[includeFilesNum].path = include_file_path;
					includeFilesTree[includeFilesNum].is_system = include_file_is_system;
					includeFilesTree[includeFilesNum].parent_num = parent_num;
					includeFilesNum++;
					if (includeFilesNum > 1024)
					{
						std::cout << "Too many include files in array. Increase possible max count." << std::endl;
						exit (-1);
					}

					//result_file << include_file_path << std::endl;
					//result_file << line << std::endl;
				}
			}

			file_to_analyze.close();
			result_file.close();
		}
		else
		{
			std::cout << "Unable to open file to analyze" << std::endl;
			return 0;
		}
	}

	return 1;

}
