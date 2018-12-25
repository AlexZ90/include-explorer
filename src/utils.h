#ifndef INCLUDE_EXPLORER_UTILS_H_
#define INCLUDE_EXPLORER_UTILS_H_

enum langtype {CLANG,CXXLANG, UNKNOWN};

std::string getFileName(const std::string& filename);
std::string getFileExtension(const std::string& filename);
langtype getFileLang(const std::string& filename);
bool checkLibForHeader(const std::string& filename, langtype parent_file_type);


#endif //INCLUDE_EXPLORER_UTILS_H_
