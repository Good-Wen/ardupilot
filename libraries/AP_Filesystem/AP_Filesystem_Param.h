/*
   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#pragma once

#include "AP_Filesystem_backend.h"

#if HAVE_FILESYSTEM_SUPPORT

#include <AP_Param/AP_Param.h>

class AP_Filesystem_Param : public AP_Filesystem_Backend
{
public:
    // functions that closely match the equivalent posix calls
    int open(const char *fname, int flags) override;
    int close(int fd) override;
    ssize_t read(int fd, void *buf, size_t count) override;
    ssize_t write(int fd, const void *buf, size_t count) override;
    int fsync(int fd) override;
    off_t lseek(int fd, off_t offset, int whence) override;
    int stat(const char *pathname, struct stat *stbuf) override;
    int unlink(const char *pathname) override;
    int mkdir(const char *pathname) override;
    void *opendir(const char *pathname) override;
    struct dirent *readdir(void *dirp) override;
    int closedir(void *dirp) override;

    // return free disk space in bytes, -1 on error
    int64_t disk_free(const char *path) override;

    // return total disk space in bytes, -1 on error
    int64_t disk_space(const char *path) override;

    // set modification time on a file
    bool set_mtime(const char *filename, const time_t mtime_sec) override;

private:
    // we maintain two cursors per open file to minimise seeking
    // when filling in gaps
    static constexpr uint8_t num_cursors = 2;

    // only allow up to 4 files at a time
    static constexpr uint8_t max_open_file = 4;

    // maximum size of one packed parameter
    static constexpr uint8_t max_pack_len = AP_MAX_NAME_SIZE + 2 + 4;

    struct cursor {
        AP_Param::ParamToken token;
        uint32_t token_ofs;
        char last_name[AP_MAX_NAME_SIZE+1];
        uint8_t trailer_len;
        uint8_t trailer[max_pack_len];
    };

    struct rfile {
        bool open;
        uint32_t file_ofs;
        struct cursor *cursors;
    } file[max_open_file];

    bool token_seek(const struct rfile &r, struct cursor &c);
    uint8_t pack_param(const AP_Param *ap, const char *pname, const char *last_name,
                       enum ap_var_type ptype, uint8_t *buf, uint8_t buflen);
};

#endif // HAVE_FILESYSTEM_SUPPORT
