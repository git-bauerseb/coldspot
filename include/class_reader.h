#ifndef CLASS_READER_H
#define CLASS_READER_H

#include <iostream>
#include <fstream>
#include <string>

#include <cstdint>
#include <sys/stat.h>

#include "definitions.h"

class ClassReader {
    public:
        ClassReader(std::string path) {            
            if (stat(path.c_str(), &m_result) != 0) {
                std::cerr << "Could not read stats of class file " << path << "\n";
                std::exit(1); 
            }

            m_file.open(path);


            if (!m_file) {
                std::cerr << "Unable to open class file " << path << "\n";
                std::exit(1);
            }

            // Reserve memory in vector
            m_buffer.resize(m_result.st_size);

            if (!m_file.read((char*)m_buffer.data(), m_result.st_size)) {
                std::cerr << "Unable to read class file " << path << "\n";
                std::exit(1);
            }
        }

        ~ClassReader() {
        }


        std::vector<u1>& get_bytecode() {
            return m_buffer;
        }

    private:
        std::fstream m_file;
        struct stat m_result;
        std::vector<u1> m_buffer;
};

#endif