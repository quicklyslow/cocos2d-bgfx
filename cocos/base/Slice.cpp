
#include "Slice.h"

namespace silly
{
    namespace slice
    {
        const std::string Slice::Empty;
        
        int Slice::compare(const Slice& rhs) const
        {
            int ret = 0;
            // It's illegal to pass nullptr to memcmp.
            if (str_ && rhs.str_)
                ret = memcmp(str_, rhs.str_, len_);
            if (ret == 0)
            {
                // Use len_ - rhs.len_ as returned value may cause overflow
                // of size_t type length. Therefore +1, -1 are returned here.
                if (len_ < rhs.len_)
                    return -1;
                else if (len_ > rhs.len_)
                    return 1;
            }
            return ret;
        }

        std::string Slice::getFilePath() const
        {
            std::string filename = toString();
            size_t pos = filename.find_last_of("/\\");
            if (pos == std::string::npos)
            {
                return Slice::Empty;
            }
            return filename.substr(0, pos) + "/";
        }

        std::string Slice::getFileName() const
        {
            std::string filename = toString();
            size_t pos = filename.find_last_of("/\\");
            if (pos == std::string::npos)
            {
                return Slice::Empty;
            }
            return filename.substr(pos + 1);
        }

        std::string Slice::getFileExtension() const
        {
            std::string filename = toString();
            size_t pos = filename.rfind('.');
            if (pos == std::string::npos)
            {
                return Slice::Empty;
            }
            return Slice(filename.substr(pos + 1)).toLower();
        }

        std::string Slice::toLower() const
        {
            std::string tmp = toString();
            for (size_t i = 0; i < tmp.length(); ++i)
            {
                tmp[i] = (char)tolower(tmp[i]);
            }
            return tmp;
        }

        std::string Slice::toUpper() const
        {
            std::string tmp = toString();
            for (size_t i = 0; i < tmp.length(); ++i)
            {
                tmp[i] = (char)toupper(tmp[i]);
            }
            return tmp;
        }

        std::list<Slice> Slice::split(const Slice& delimeter) const
        {
            std::string text = toString();
            std::string delimit = delimeter.toString();
            std::list<Slice> tokens;
            std::size_t start = text.find_first_not_of(delimit), end = 0;
            while ((end = text.find_first_of(delimit, start)) != std::string::npos)
            {
                tokens.push_back(Slice(str_ + start, end - start));
                start = text.find_first_not_of(delimeter, end);
            }
            if (start != std::string::npos)
            {
                tokens.push_back(Slice(str_ + start));
            }
            return tokens;
        }

        float Slice::stof(const Slice& str)
        {
            return static_cast<float>(std::atof(str.toString().c_str()));
        }

        int Slice::stoi(const Slice& str)
        {
            return std::atoi(str.toString().c_str());
        }
    }
}