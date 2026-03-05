#include "zip.h"

bool Zip::init(const std::string& zip_path){
    {
        if (s_is_initialized) {
            mz_zip_reader_end(&s_archive);
        }

        memset(&s_archive, 0, sizeof(s_archive));
        
        if (!mz_zip_reader_init_file(&s_archive, zip_path.c_str(), 0)) {
            s_is_initialized = false;
            return false;
        }

        s_is_initialized = true;
        return true;
    }
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

mz_zip_archive ResourceManager::s_archive = {};
bool ResourceManager::s_is_initialized = false;