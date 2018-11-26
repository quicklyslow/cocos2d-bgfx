#pragma once

#include <cassert>
#include <cstring>
#include <string>
#include <list>

namespace silly 
{    
    namespace slice 
    {
        class Slice 
        {
        private:
            struct TrustedInitTag {};
            constexpr Slice(const char* s, size_t n, TrustedInitTag) : str_(s), len_(n) {}
        public:
            Slice(const char* s, size_t n) : str_(s), len_(n) {}
            Slice(const char* s) : Slice(s, s ? strlen(s) : 0) {}
            Slice(const std::string& s): Slice(s.data(), s.size()) {}
            Slice(std::pair<const char*, size_t> sp) : Slice(sp.first, sp.second) {}

            constexpr Slice(std::nullptr_t p = nullptr) : str_(nullptr), len_(0) {}

            operator std::string() const
            {
                return std::string(str_, len_);
            }

            const char &operator[](size_t n) const 
            {
                return str_[n];
            }

            size_t size() const 
            {
                return len_;
            }

            const char* rawData() const 
            {
                return str_;
            }

            std::string toString() const
            {
                return std::string(str_, len_);
            }

            bool empty() const
            {
                return len_ == 0;
            }

            int compare(const Slice& rhs) const;

            void skip(size_t n)
            {
                assert(n <= len_);
                str_ += n;
                len_ -= n;
            }

            void skipRight(size_t n) {
                assert(n <= len_);
                len_ -= n;
            }

            void copyTo(char* dest, bool appendEndingNull = true) const
            {
                memcpy(dest, str_, len_);
                if (appendEndingNull)
                {
                    dest[len_] = '\0';
                }
            }

            Slice& trimSpace()
            {
                assert(len_ > 0);
                size_t start = 0, end = len_ - 1;
                while (start < end && isspace(str_[start]))
                {
                    start++;
                }
                while (start < end && isspace(str_[end]))
                {
                    end--;
                }
                str_ += start;
                len_ = end - start + 1;
                return *this;
            }

            Slice& trimZero()
            {
                assert(len_ > 0);
                size_t end = len_ - 1;
                while (0 < end && (str_[end] == '0' || str_[end] == '.'))
                {
                    if (str_[end--] == '.')
                        break;
                }
                len_ = end + 1;
                return *this;
            }

            using const_iterator = const char*;
            
            const_iterator begin() const
            {
                return str_;
            }

            const_iterator end() const
            {
                return str_ + len_;
            }

            std::string getFilePath() const;
            std::string getFileName() const;
            std::string getFileExtension() const;
            std::string toLower() const;
            std::string toUpper() const;
            std::list<Slice> split(const Slice& delimeter) const;

            static const std::string Empty;
            static float stof(const Slice& str);
            static int stoi(const Slice& str);

            constexpr friend Slice operator"" _slice(const char* s, size_t n);
        private:
            const char* str_;
            size_t len_;
        };

        inline Slice trimSpace(const Slice& s)
        {
            Slice tmp = s;
            return tmp.trimSpace();
        }

        inline bool operator==(const Slice& lhs, const Slice& rhs)
        {
            return lhs.compare(rhs) == 0;
        }

        inline bool operator!=(const Slice& lhs, const Slice& rhs)
        {
            return !(lhs == rhs);
        }
        
        inline std::string operator+(const std::string& lhs, const Slice& rhs)
        {
            return lhs + rhs.toString();
        }

        constexpr Slice operator"" _slice(const char* s, size_t n)
        {
            return Slice(s, n, Slice::TrustedInitTag{});
        }
    }
}

using String = const silly::slice::Slice&;