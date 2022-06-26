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

            // Reserve memory
            m_buffer = new char[m_result.st_size];
            m_file.read(m_buffer, m_result.st_size);

            if (!m_file) {
                std::cerr << "Unable to read content of class file " << path << "\n";
                std::exit(1);
            }

            m_f_ptr = 0;
        }

        ~ClassReader() {
            if (m_buffer != nullptr) {
                delete[] m_buffer;
            }

            if (m_file) {
                m_file.close();
            }
        }

        uint8_t read_u1() {
            if (m_f_ptr < m_result.st_size) {
                return (uint8_t)m_buffer[m_f_ptr++];
            }

            return 0;
        }

        uint16_t read_u2() {
            if (m_f_ptr < m_result.st_size - 1) {
                uint8_t u1_0 = m_buffer[m_f_ptr++];
                uint8_t u1_1 = m_buffer[m_f_ptr++];

                return ((uint32_t)u1_0 << 8) | ((uint32_t)u1_1);
            }

            return 0;
        }

        uint32_t read_u4() {
            if (m_f_ptr < m_result.st_size - 3) {

                uint8_t u1_0 = m_buffer[m_f_ptr++];
                uint8_t u1_1 = m_buffer[m_f_ptr++];
                uint8_t u1_2 = m_buffer[m_f_ptr++];
                uint8_t u1_3 = m_buffer[m_f_ptr++];

                return ((uint32_t)u1_0 << 24) | ((uint32_t)u1_1 << 16) | ((uint32_t)u1_2 << 8) | ((uint32_t)u1_3);
            }

            return 0;
        }

        /*
            Returns a handle to the current position in the buffer in the
            form of an char pointer. This is makes it easier to load constants
            from the constant_pool.
        */
        char* get_handle() {
            return &m_buffer[m_f_ptr];
        }

        /*
            During reading of constant_pool elements, the ptr gets incremented
            and thus will point to a location further into the buffer.
        */
        void set_handle(char* ptr) {
            m_f_ptr = ptr - m_buffer;
        }

        /*
            Move the pointer forward in the buffer by amount.
        */
        void forward(uint32_t amount) {
            m_f_ptr += amount;
        }

    private:
        std::fstream m_file;
        struct stat m_result;
        char* m_buffer;
        uint32_t m_f_ptr;
};

#endif