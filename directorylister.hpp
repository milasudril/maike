//@{
//@"targets":[{"name":"directorylister.hpp","type":"include"}]
//@"dependencies_extra":[{"ref":"directorylister.o",rel:"implementation"}]
//@}

#ifndef MAIKE_DIRECTORYLISTER_HPP
#define MAIKE_DIRECTORYLISTER_HPP

namespace Maike
    {
    class DirectoryLister
        {
        public:
            explicit DirectoryLister(const char* dirname);
            ~DirectoryLister();
            const char* read();

        private:
            struct Impl;
            Impl* m_impl;
        };
    }

#endif
