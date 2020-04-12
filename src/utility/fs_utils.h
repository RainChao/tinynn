#pragma once
#include <string>
#include <functional>
#include <type_traits>
#include <iostream>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <memory>
#include <sys/syscall.h>
#include <utility/string_utils.h>

namespace tinynn {

class LogStream {
public:
   LogStream(const bool doabort = false) : _doabort(doabort) {}
   LogStream(const LogStream& other) {
       _ss << other._ss.str();
       _doabort = other._doabort;
   }   
   LogStream(LogStream&& other) {
       _ss << other._ss.str();
       _doabort = other._doabort;
   }   
   ~LogStream() {
       if (_doabort and _ss.str().size() > 0) {
           ::std::cerr << "[Fatal]=" << _ss.str();
           abort();
       }   
   }   
   template<typename T>
   LogStream&& operator<<(T&& val) {
       _ss << val;
       return ::std::move(*this);
   }   
   friend LogStream&& operator&&(bool flag, LogStream&& self) {
       return ::std::move(self);
   }   
private:
   bool _doabort;
   ::std::stringstream _ss;
};

#define PCHECK(flag) \
    (flag) == false? LogStream(true) : false && LogStream(false) 

class FsUtils {
public:
    static ::std::shared_ptr<FILE> open(const ::std::string& path,
                                        const ::std::string& chmod,
                                        const ::std::vector<::std::string>& converters) {
        ::std::string mutable_path = path;
        if (chmod == "r") {
            return _open_for_read(mutable_path, converters);
        } else if (chmod == "w") {
            return _open_for_write(mutable_path, converters);
        }
        return nullptr;
    }
    
    template <typename FUNC>
    static void read(::std::shared_ptr<FILE> filefd,
                        FUNC&& func,
                        const char delimer = '\n') {
        char* buffer = NULL;
        size_t buf_size = 0;
        int read_size = 0;
        while ((read_size = ::getdelim(&buffer, &buf_size, delimer, &*filefd)) >= 0) {
            if (read_size >= 1) {
                if (buffer[read_size-1] == delimer) {
                    buffer[--read_size] = '\0';
                }   
                func(buffer, read_size);
            }   
        }   
        if (buffer) { free(buffer); };
        PCHECK(feof(&*filefd));
    }

    static size_t write(::std::shared_ptr<FILE> filefd,
                        const ::std::string& val) {
        PCHECK(fwrite(val.c_str(), val.size(), 1, filefd.get()) == 1);
        fputc('\n', filefd.get());
        return val.size();
    }
    template <typename T>
    static size_t write(::std::shared_ptr<FILE> filefd,
                        const T& val) {
        abort();
        return 1;                
    }

private:
    static ::std::shared_ptr<FILE> _open_for_read(::std::string& path,
                                                  const ::std::vector<::std::string>& converters) {
        if (path.find("hdfs") != ::std::string::npos or  
            path.find("afs") != ::std::string::npos) {
            //hdfs open todo
            return nullptr;
        } else {
            bool ispipe = false;
            if (StringUtils::ends_with(path, ".gz")) {
                ispipe = true;
                path = StringUtils::string_format("zcat %s", path.c_str());
            }
            if (!converters.empty()) {
                _fs_add_read_converter(path, converters, ispipe);
            }
            return _fs_open(path, ispipe, "r"); 
        }
        return nullptr;
    }

    static ::std::shared_ptr<FILE> _open_for_write(::std::string& path,
                                                   const ::std::vector<::std::string>& converters) {
        if (path.find("hdfs") != ::std::string::npos or  
            path.find("afs") != ::std::string::npos) {
            //hdfs open todo
            return nullptr;
        } else {
            bool ispipe = false;
            if (StringUtils::ends_with(path, ".gz")) {
                ispipe = true;
                path = StringUtils::string_format("gzip | %s", path.c_str());
            }
            if (!converters.empty()) {
                _fs_add_write_converter(path, converters, ispipe);
            }
            return _fs_open(path, ispipe, "w"); 
        }
        return nullptr;
    }

    static void _fs_add_read_converter(::std::string& path, 
                                  const ::std::vector<::std::string>& converters,
                                  bool& ispipe) {
        if (converters.size() != 0) {
            if (!ispipe) {
                ispipe = true;
                path = StringUtils::string_format("cat %s", path.c_str());
            }
            for (auto& converter : converters) {
                path = StringUtils::string_format("%s | %s", path.c_str(), converter.c_str());
            }
        }    
    }

    static void _fs_add_write_converter(::std::string& path, 
                                        const ::std::vector<::std::string>& converters,
                                        bool& ispipe) {
        if (converters.size() != 0) {
            if (!ispipe) {
                ::std::string shell_cmd = converters[0];
                for (int i = 1; i < converters.size(); ++i) {
                    path = StringUtils::string_format("%s | %s", shell_cmd.c_str(), converters[i].c_str());
                }
                ispipe = true;
                path = StringUtils::string_format("(%s) > %s", shell_cmd.c_str(), path.c_str());
            } else {
                for (auto& converter : converters) {
                    path = StringUtils::string_format("%s | %s", converter.c_str(), path.c_str());
                }
            }
            ::std::cout << "path = " << path << ::std::endl;
        }    
    }

    static ::std::shared_ptr<FILE> _file_open(const ::std::string& path,
                                              const ::std::string& chmod) {
        FILE* fp; 
        fp = fopen(path.c_str(), chmod.c_str());
        return {fp, [](void* p) {
            if (p != nullptr) {
                fclose((FILE*)p);
            }
        }}; 
    }
    static void _run_shell_cmd(const ::std::string& cmd) {
        execl("/bin/sh", "sh", "-c", cmd.c_str(), nullptr);
    }

    static ::std::shared_ptr<FILE> _shell_open_fork(const ::std::string& cmd,
                                                   const ::std::string& mode,
                                                   const int parent_fd,
                                                   const int child_fd,
                                                   bool doread) {
        int child_pid = vfork();
        //parent process
        if (child_pid > 0) {
            close(child_fd);
            FILE* fp = fdopen(parent_fd, mode.c_str());
            return {fp, [child_pid](void* fp) {
                if (fp != nullptr) {
                    fclose((FILE*)fp);
                }   
                int wstatus = -1; 
                int ret = -1; 

                do {
                    PCHECK((ret = waitpid(child_pid, &wstatus, 0)) >= 0 || ret == -1 && errno == EINTR);
                } while (ret == -1 && errno == EINTR);

                PCHECK(wstatus == 0 || wstatus == (128 + SIGPIPE) * 256 || wstatus == -1 && errno == ECHILD);
            }}; 
        }   
        //child process
        close(parent_fd);
        int child_std_end = doread ? 1 : 0;
        if (child_fd != child_std_end) {
            dup2(child_fd, child_std_end);
            close(child_fd);
        }   
        _run_shell_cmd(cmd); 
        exit(127);
    }

    static ::std::shared_ptr<FILE> _shell_open(const ::std::string& cmd,
                                              const ::std::string& chmod) {
        bool isread = (chmod == "r");
        bool iswrite = (chmod == "w");
        int pipe_fds[2];
        int parent_fd;
        int child_fd;
        pipe(pipe_fds) == 0;
        if (isread) {
            parent_fd = pipe_fds[0];
            child_fd = pipe_fds[1];
        } else if (iswrite) {
            parent_fd = pipe_fds[1];
            child_fd = pipe_fds[0];
        }   
        return _shell_open_fork(cmd, chmod, parent_fd, child_fd, isread);
    }
    static ::std::shared_ptr<FILE> _fs_open(::std::string& path, 
                                            const bool ispipe,
                                            const ::std::string& chmod) {
        if (!ispipe) {
            return _file_open(path, chmod);
        }  else {
            return _shell_open(path, chmod);
        }
    }
};
}

