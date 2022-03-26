#include "filesystem.hpp"
#include "text/text.hpp"

fs::File fs::openFile(String path) {
    if(path.getSize() == 0)
        return File(nullptr, nullptr);
    
    for(int i = 0; i < path.getSize() - 1; i++)
        if(path[i] == '/' && path[i + 1] == '/') {
            path.erase(i);
            i--;
        }
    
    if(path[0] == '/')
        path.erase(0);
    
    bool has_to_be_a_directory = false;
    if(path[path.getSize() - 1] == '/') {
        has_to_be_a_directory = true;
        path.pop();
    }
    
    
    Directory curr_dir = getFileSystem()->getRootDirectory();
    String curr_str;
    for(int i = 0; i < path.getSize(); i++) {
        if(path[i] == '/') {
            File curr_file = curr_dir.getFile(curr_str);
            if(!curr_file.exists() || !curr_file.isDirectory())
                return File(nullptr, nullptr);
            curr_dir = (Directory)curr_file;
            curr_str.clear();
        } else
            curr_str.push(path[i]);
    }
    
    File result = curr_dir.getFile(curr_str);
    
    if(!result.exists() || (!result.isDirectory() && has_to_be_a_directory))
        return File(nullptr, nullptr);
    
    return result;
}


void fs::deleteFile(const String& path) {
    File file = openFile(path);
    if(!file.exists())
        return;
    
    Directory parent_directory = file.getParentDirectory();
    String file_name;
    for(int i = path.getSize() - 1; i >= 0 && path[i] != '/'; i--) {
        file_name.insert(path[i], 0);
    }
    
    parent_directory.removeFile(file_name);
}

void fs::createFile(const String& path) {
    
}
