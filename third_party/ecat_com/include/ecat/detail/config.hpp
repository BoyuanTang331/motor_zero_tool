#ifndef ECAT_CONFIG_HPP
#define ECAT_CONFIG_HPP

#define ECAT_DECL __attribute__((visibility("default")))

#ifndef ECAT_USE_BOOST_FILESYSTEM
#define ECAT_USE_BOOST_FILESYSTEM 1
#endif

#ifndef ECAT_FILESYSTEM_HEADER
#if ECAT_USE_BOOST_FILESYSTEM
#define ECAT_FILESYSTEM_HEADER <boost/filesystem.hpp>
#define ECAT_FILESYSTEM_NAMESPACE ::boost::filesystem;
#elif defined(__GNUC__) && __GNUC__ < 8
#define ECAT_FILESYSTEM_HEADER <experimental/filesystem>
#define ECAT_FILESYSTEM_NAMESPACE ::std::experimental::filesystem
#else
#define ECAT_FILESYSTEM_HEADER <filesystem>
#endif
#endif

#ifndef ECAT_FILESYSTEM_NAMESPACE
#define ECAT_FILESYSTEM_NAMESPACE ::std::filesystem
#endif

#ifndef NECRO_RT
#ifdef __QIUNIU__
#define NECRO_RT __QIUNIU
#else
#define NECRO_RT
#endif
#endif

#ifndef NECRO_NRT
#ifdef __QIUNIU_WRAP__
#define NECRO_NRT __STD
#else
#define NECRO_NRT
#endif
#endif

#ifndef NECRO_LIKELY
#ifdef __glibc_likely
#define NECRO_LIKELY __glibc_likely
#else
#define NECRO_LIKELY
#endif
#endif

#ifndef NECRO_UNLIKELY
#ifdef __glibc_unlikely
#define NECRO_UNLIKELY __glibc_unlikely
#else
#define NECRO_UNLIKELY
#endif
#endif

#endif // ECAT_CONFIG_HPP
