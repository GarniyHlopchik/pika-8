#include "zip.h"
size_t find_zip_start(unsigned char* mem, size_t size) {
    // ZIP EOCD is usually in the last 22 to 65k bytes (due to comments)
    // We search backwards for the EOCD signature: 0x06054b50
    for (size_t i = size - 22; i > 0; i--) {
        if (mem[i] == 0x50 && mem[i+1] == 0x4B && mem[i+2] == 0x05 && mem[i+3] == 0x06) {
            // Found EOCD! Now read the "Offset of start of central directory"
            // This is at offset 16 within the EOCD record (4 bytes)
            uint32_t cd_size = *(uint32_t*)&mem[i + 12];
            uint32_t cd_offset = *(uint32_t*)&mem[i + 16];
            
            // The ZIP starts where the CD thinks it starts, relative to our search
            size_t eocd_pos = i;
            size_t real_zip_start = eocd_pos - cd_size - cd_offset;
            
            return real_zip_start;
        }
    }
    return SIZE_MAX;
}
bool Zip::init(const std::string& zip_path)
{
    if (s_is_initialized)
        mz_zip_reader_end(&s_archive);

    memset(&s_archive, 0, sizeof(s_archive));

    std::ifstream file(zip_path, std::ios::binary | std::ios::ate);
    size_t size = file.tellg();
    file.seekg(0);

    unsigned char* mem = (unsigned char*)malloc(size);
    file.read((char*)mem, size);

    size_t zip_start_offset = find_zip_start(mem, size);
    if (zip_start_offset == SIZE_MAX) {
        std::cout << "Could not find ZIP header\n";
        return false;
    }
    

    if (!mz_zip_reader_init_mem(&s_archive, mem + zip_start_offset, size - zip_start_offset, 0)) {
        std::cout << "Failed to init ZIP at offset " << zip_start_offset << "\n";
        mz_zip_error err = mz_zip_get_last_error(&s_archive);
        std::cout << "Miniz Error Code: " << mz_zip_get_error_string(err) << std::endl;
        return false;
    }

    s_is_initialized = true;
    return true;
}

Resource Zip::load(const std::string& internal_path){
    {
        Resource res;
        if (!s_is_initialized) return res;

        size_t size;
        void* p = mz_zip_reader_extract_file_to_heap(&s_archive, internal_path.c_str(), &size, 0);

        if (p) {
            res.data.reset(static_cast<uint8_t*>(p));
            res.size = size;
        }

        return res;
    }
}
void Zip::shutdown(){
    {
        if (s_is_initialized) {
            mz_zip_reader_end(&s_archive);
            s_is_initialized = false;
        }
    }
}

mz_zip_archive Zip::s_archive = {};
bool Zip::s_is_initialized = false;