//@	{
//@	 "targets":[{"name":"directorylister.hpp","type":"include"}]
//@	,"dependencies_extra":[{"ref":"directorylister.o","rel":"implementation"}]
//@	}

#ifndef MAIKE_DIRECTORYLISTER_HPP
#define MAIKE_DIRECTORYLISTER_HPP

#include "visibility.hpp"
#include <utility>

namespace Maike
    {
    class PRIVATE DirectoryLister
        {
        public:
			DirectoryLister(const DirectoryLister&)=delete;
			DirectoryLister& operator=(const DirectoryLister&)=delete;

			DirectoryLister(DirectoryLister&& dir) noexcept:m_impl(dir.m_impl)
				{dir.m_impl=nullptr;}

			DirectoryLister& operator=(DirectoryLister&& dir) noexcept
				{
				std::swap(dir.m_impl,m_impl);
				return *this;
				}

            explicit DirectoryLister(const char* dirname);
            ~DirectoryLister() noexcept;

            const char* read();

        private:
            struct Impl;
            Impl* m_impl;
        };
    }

#endif
