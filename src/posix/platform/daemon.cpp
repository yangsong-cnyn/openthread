/*
 *  Copyright (c) 2021, The OpenThread Authors.
 *  All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions are met:
 *  1. Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *  2. Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *  3. Neither the name of the copyright holder nor the
 *     names of its contributors may be used to endorse or promote products
 *     derived from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 *  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 *  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 *  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 *  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 *  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 *  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 *  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 *  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 *  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 *  POSSIBILITY OF SUCH DAMAGE.
 */

#include "posix/platform/daemon.hpp"

#if OPENTHREAD_POSIX_CONFIG_ANDROID_ENABLE
#include <cutils/sockets.h>
#endif
#include <fcntl.h>
#include <signal.h>
#include <stdarg.h>
#include <string.h>
#include <sys/file.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/un.h>
#include <unistd.h>

#include <openthread/cli.h>

#include "cli/cli_config.h"
#include "common/code_utils.hpp"
#include "posix/platform/platform-posix.h"
#include "posix/platform/utils.hpp"

#if OPENTHREAD_POSIX_CONFIG_DAEMON_ENABLE

#define OPENTHREAD_POSIX_DAEMON_SOCKET_LOCK OPENTHREAD_POSIX_CONFIG_DAEMON_SOCKET_BASENAME ".lock"
static_assert(sizeof(OPENTHREAD_POSIX_DAEMON_SOCKET_NAME) < sizeof(sockaddr_un::sun_path),
              "OpenThread daemon socket name too long!");

namespace ot {
namespace Posix {

namespace {

typedef char(Filename)[sizeof(sockaddr_un::sun_path)];

void GetFilename(Filename &aFilename, const char *aPattern)
{
    int         rval;
    const char *netIfName = strlen(gNetifName) > 0 ? gNetifName : OPENTHREAD_POSIX_CONFIG_THREAD_NETIF_DEFAULT_NAME;

    rval = snprintf(aFilename, sizeof(aFilename), aPattern, netIfName);
    if (rval < 0 && static_cast<size_t>(rval) >= sizeof(aFilename))
    {
        DieNow(OT_EXIT_INVALID_ARGUMENTS);
    }
}

} // namespace

const char Daemon::kLogModuleName[] = "Daemon";

int Daemon::OutputFormat(const char *aFormat, ...)
{
    int     ret;
    va_list ap;

    va_start(ap, aFormat);
    ret = OutputFormatV(aFormat, ap);
    va_end(ap);

    return ret;
}

int Daemon::OutputFormatV(const char *aFormat, va_list aArguments)
{
    static constexpr char truncatedMsg[] = "(truncated ...)";
    char                  buf[OPENTHREAD_CONFIG_CLI_MAX_LINE_LENGTH];
    int                   rval;

    static_assert(sizeof(truncatedMsg) < OPENTHREAD_CONFIG_CLI_MAX_LINE_LENGTH,
                  "OPENTHREAD_CONFIG_CLI_MAX_LINE_LENGTH is too short!");

    rval = vsnprintf(buf, sizeof(buf), aFormat, aArguments);
    VerifyOrExit(rval >= 0, LogWarn("Failed to format CLI output: %s", strerror(errno)));

    if (rval >= static_cast<int>(sizeof(buf)))
    {
        rval = static_cast<int>(sizeof(buf) - 1);
        memcpy(buf + sizeof(buf) - sizeof(truncatedMsg), truncatedMsg, sizeof(truncatedMsg));
    }

    VerifyOrExit(mSessionSocket != -1);

#ifdef __linux__
    // Don't die on SIGPIPE
    rval = send(mSessionSocket, buf, static_cast<size_t>(rval), MSG_NOSIGNAL);
#else
    rval = static_cast<int>(write(mSessionSocket, buf, static_cast<size_t>(rval)));
#endif

    if (rval < 0)
    {
        LogWarn("Failed to write CLI output: %s", strerror(errno));
        close(mSessionSocket);
        mSessionSocket = -1;
    }

exit:
    return rval;
}

void Daemon::InitializeSessionSocket(void)
{
    int newSessionSocket;
    int rval;

    VerifyOrExit((newSessionSocket = accept(mListenSocket, nullptr, nullptr)) != -1, rval = -1);

    VerifyOrExit((rval = fcntl(newSessionSocket, F_GETFD, 0)) != -1);

    rval |= FD_CLOEXEC;

    VerifyOrExit((rval = fcntl(newSessionSocket, F_SETFD, rval)) != -1);

#ifndef __linux__
    // some platforms (macOS, Solaris) don't have MSG_NOSIGNAL
    // SOME of those (macOS, but NOT Solaris) support SO_NOSIGPIPE
    // if we have SO_NOSIGPIPE, then set it. Otherwise, we're going
    // to simply ignore it.
#if defined(SO_NOSIGPIPE)
    rval = setsockopt(newSessionSocket, SOL_SOCKET, SO_NOSIGPIPE, &rval, sizeof(rval));
    VerifyOrExit(rval != -1);
#else
#warning "no support for MSG_NOSIGNAL or SO_NOSIGPIPE"
#endif
#endif // __linux__

    if (mSessionSocket != -1)
    {
        close(mSessionSocket);
    }
    mSessionSocket = newSessionSocket;

exit:
    if (rval == -1)
    {
        LogWarn("Failed to initialize session socket: %s", strerror(errno));
        if (newSessionSocket != -1)
        {
            close(newSessionSocket);
        }
    }
    else
    {
        LogInfo("Session socket is ready");
    }
}

#if OPENTHREAD_POSIX_CONFIG_ANDROID_ENABLE
void Daemon::createListenSocketOrDie(void)
{
    Filename socketFile;

    // Don't use OPENTHREAD_POSIX_DAEMON_SOCKET_NAME because android_get_control_socket
    // below already assumes parent /dev/socket dir
    GetFilename(socketFile, "ot-daemon/%s.sock");

    // This returns the init-managed stream socket which is already bind to
    // /dev/socket/ot-daemon/<interface-name>.sock
    mListenSocket = android_get_control_socket(socketFile);

    if (mListenSocket == -1)
    {
        DieNowWithMessage("android_get_control_socket", OT_EXIT_ERROR_ERRNO);
    }
}
#else
void Daemon::createListenSocketOrDie(void)
{
    struct sockaddr_un sockname;
    int                ret;

    class AllowAllGuard
    {
    public:
        AllowAllGuard(void)
        {
            const char *allowAll = getenv("OT_DAEMON_ALLOW_ALL");
            mAllowAll            = (allowAll != nullptr && strcmp("1", allowAll) == 0);

            if (mAllowAll)
            {
                mMode = umask(0);
            }
        }
        ~AllowAllGuard(void)
        {
            if (mAllowAll)
            {
                umask(mMode);
            }
        }

    private:
        bool   mAllowAll = false;
        mode_t mMode     = 0;
    };

    mListenSocket = SocketWithCloseExec(AF_UNIX, SOCK_STREAM, 0, kSocketNonBlock);

    if (mListenSocket == -1)
    {
        DieNow(OT_EXIT_FAILURE);
    }

    {
        static_assert(sizeof(OPENTHREAD_POSIX_DAEMON_SOCKET_LOCK) == sizeof(OPENTHREAD_POSIX_DAEMON_SOCKET_NAME),
                      "sock and lock file name pattern should have the same length!");
        Filename lockfile;

        GetFilename(lockfile, OPENTHREAD_POSIX_DAEMON_SOCKET_LOCK);

        mDaemonLock = open(lockfile, O_CREAT | O_RDONLY | O_CLOEXEC, 0600);
    }

    if (mDaemonLock == -1)
    {
        DieNowWithMessage("open", OT_EXIT_ERROR_ERRNO);
    }

    if (flock(mDaemonLock, LOCK_EX | LOCK_NB) == -1)
    {
        DieNowWithMessage("flock", OT_EXIT_ERROR_ERRNO);
    }

    memset(&sockname, 0, sizeof(struct sockaddr_un));

    sockname.sun_family = AF_UNIX;
    GetFilename(sockname.sun_path, OPENTHREAD_POSIX_DAEMON_SOCKET_NAME);
    (void)unlink(sockname.sun_path);

    {
        AllowAllGuard allowAllGuard;

        ret = bind(mListenSocket, reinterpret_cast<const struct sockaddr *>(&sockname), sizeof(struct sockaddr_un));
    }

    if (ret == -1)
    {
        DieNowWithMessage("bind", OT_EXIT_ERROR_ERRNO);
    }
}
#endif // OPENTHREAD_POSIX_CONFIG_ANDROID_ENABLE

void Daemon::SetUp(void)
{
    int ret;

    // This allows implementing pseudo reset.
    VerifyOrExit(mListenSocket == -1);
    createListenSocketOrDie();

    //
    // only accept 1 connection.
    //
    ret = listen(mListenSocket, 1);
    if (ret == -1)
    {
        DieNowWithMessage("listen", OT_EXIT_ERROR_ERRNO);
    }

exit:
#if OPENTHREAD_POSIX_CONFIG_DAEMON_CLI_ENABLE
    otSysCliInitUsingDaemon(gInstance);
#endif

    Mainloop::Manager::Get().Add(*this);

    return;
}

void Daemon::TearDown(void)
{
    Mainloop::Manager::Get().Remove(*this);

    if (mSessionSocket != -1)
    {
        close(mSessionSocket);
        mSessionSocket = -1;
    }

#if !OPENTHREAD_POSIX_CONFIG_ANDROID_ENABLE
    // The `mListenSocket` is managed by `init` on Android
    if (mListenSocket != -1)
    {
        close(mListenSocket);
        mListenSocket = -1;
    }

    if (gPlatResetReason != OT_PLAT_RESET_REASON_SOFTWARE)
    {
        Filename sockfile;

        GetFilename(sockfile, OPENTHREAD_POSIX_DAEMON_SOCKET_NAME);
        LogDebg("Removing daemon socket: %s", sockfile);
        (void)unlink(sockfile);
    }

    if (mDaemonLock != -1)
    {
        (void)flock(mDaemonLock, LOCK_UN);
        close(mDaemonLock);
        mDaemonLock = -1;
    }
#endif
}

void Daemon::Update(Mainloop::Context &aContext)
{
    Mainloop::AddToReadFdSet(mListenSocket, aContext);
    Mainloop::AddToErrorFdSet(mListenSocket, aContext);

    Mainloop::AddToReadFdSet(mSessionSocket, aContext);
    Mainloop::AddToErrorFdSet(mSessionSocket, aContext);
}

void Daemon::Process(const Mainloop::Context &aContext)
{
    ssize_t rval;

    VerifyOrExit(mListenSocket != -1);

    if (Mainloop::HasFdErrored(mListenSocket, aContext))
    {
        DieNowWithMessage("daemon socket error", OT_EXIT_FAILURE);
    }
    else if (Mainloop::IsFdReadable(mListenSocket, aContext))
    {
        InitializeSessionSocket();
    }

    VerifyOrExit(mSessionSocket != -1);

    if (Mainloop::HasFdErrored(mSessionSocket, aContext))
    {
        close(mSessionSocket);
        mSessionSocket = -1;
    }
    else if (Mainloop::IsFdReadable(mSessionSocket, aContext))
    {
        uint8_t buffer[OPENTHREAD_CONFIG_CLI_MAX_LINE_LENGTH];

        // leave 1 byte for the null terminator
        rval = read(mSessionSocket, buffer, sizeof(buffer) - 1);

        if (rval > 0)
        {
            buffer[rval] = '\0';
#if OPENTHREAD_POSIX_CONFIG_DAEMON_CLI_ENABLE
            otCliInputLine(reinterpret_cast<char *>(buffer));
#else
            OutputFormat("Error: CLI is disabled!\n");
#endif
        }
        else
        {
            if (rval < 0)
            {
                LogWarn("Daemon read: %s", strerror(errno));
            }
            close(mSessionSocket);
            mSessionSocket = -1;
        }
    }

exit:
    return;
}

Daemon &Daemon::Get(void)
{
    static Daemon sInstance;

    return sInstance;
}

} // namespace Posix
} // namespace ot
#endif // OPENTHREAD_POSIX_CONFIG_DAEMON_ENABLE
