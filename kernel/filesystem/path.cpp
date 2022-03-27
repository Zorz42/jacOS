#include "filesystem.hpp"
#include "text/text.hpp"

fs::File fs::openFile(String path) {
    if(path.getSize() == 0)
        return File();
    
    for(int i = 0; i < path.getSize() - 1; i++)
        if(path[i] == '/' && path[i + 1] == '/') {
            path.erase(i);
            i--;
        }
    
    if(path[0] == '/')
        path.erase(0);
    
    if(path.getSize() == 0)
        return getFileSystem()->getRootDirectory();
    
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
                return File();
            curr_dir = (Directory)curr_file;
            curr_str.clear();
        } else
            curr_str.push(path[i]);
    }
    
    File result = curr_dir.getFile(curr_str);
    
    if(!result.exists() || (!result.isDirectory() && has_to_be_a_directory))
        return File();
    
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

fs::File fs::createFile(String path, const String& file_type) {
    if(path[0] != '/')
        path.insert('/', 0);
    
    String file_name;
    while(path[path.getSize() - 1] != '/') {
        file_name.insert(path[path.getSize() - 1], 0);
        path.pop();
    }
    
    File parent_directory_file = openFile(path);
    Directory parent_directory = (Directory)parent_directory_file;
    parent_directory.createFile(file_name, file_type);
    return parent_directory.getFile(file_name);
}

fs::File fs::copyFile(const String& original_path, const String& copy_path) {
    File original = openFile(original_path);
    File copy = createFile(copy_path, original.getType());
    copy.write(original.read());
    return copy;
}

fs::File fs::moveFile(String original_path, String new_path) {
    File original = openFile(original_path);
    
    if(!original.exists())
        return File();
    
    if(original_path[0] != '/')
        original_path.insert('/', 0);
    while(original_path[original_path.getSize() - 1] != '/')
        original_path.pop();
    
    String new_file_name;
    if(new_path[0] != '/')
        new_path.insert('/', 0);
    while(new_path[new_path.getSize() - 1] != '/') {
        new_file_name.insert(new_path[new_path.getSize() - 1], 0);
        new_path.pop();
    }
    
    File original_parent_file = openFile(original_path);
    Directory original_parent = (Directory)original_parent_file;
    
    File new_parent_file = openFile(new_path);
    Directory new_parent = (Directory)new_parent_file;
    
    __FileDescriptor* descriptor = nullptr;
    for(int i = 0; i < original_parent.getFileCount(); i++)
        if(original_parent.getFile(i) == original) {
            descriptor = original_parent.getDirectory()->files[i];
            original_parent.getDirectory()->files.erase(i);
        }
    
    new_parent.getDirectory()->files.push(descriptor);
    descriptor->parent_directory = new_parent.getDirectory();
    descriptor->name = new_file_name;
    
    original_parent.flushMetadata();
    new_parent.flushMetadata();
    
    return new_parent.getFile(new_file_name);
}
