// SPDX-License-Identifier: GPL-3.0-only
/*
 *  Prism Launcher - Minecraft Launcher
 *  Copyright (C) 2022 Sefa Eyeoglu <contact@scrumplex.net>
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 3.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *
 * This file incorporates work covered by the following copyright and
 * permission notice:
 *
 *      Copyright 2013-2021 MultiMC Contributors
 *
 *      Licensed under the Apache License, Version 2.0 (the "License");
 *      you may not use this file except in compliance with the License.
 *      You may obtain a copy of the License at
 *
 *          http://www.apache.org/licenses/LICENSE-2.0
 *
 *      Unless required by applicable law or agreed to in writing, software
 *      distributed under the License is distributed on an "AS IS" BASIS,
 *      WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *      See the License for the specific language governing permissions and
 *      limitations under the License.
 */

#include <qstringview.h>
#include <signal.h>
#include <sys/wait.h>
#include <unistd.h>
#include <QCommandLineParser>
#include <cpptrace/utils.hpp>
#include <csignal>
#include <cstring>
#include <iostream>
#include "Application.h"
#include "FileSystem.h"

// #define BREAK_INFINITE_LOOP
// #define BREAK_EXCEPTION
// #define BREAK_RETURN

#ifdef BREAK_INFINITE_LOOP
#include <chrono>
#include <thread>
#endif

// This is just a utility I like, it makes the pipe API more expressive.
struct pipe_t {
    union {
        struct {
            int read_end;
            int write_end;
        };
        int data[2];
    };
};

char* applicationPath;

void do_signal_safe_trace(cpptrace::frame_ptr* buffer, std::size_t count)
{
    // Setup pipe and spawn child
    pipe_t input_pipe;
    pipe(input_pipe.data);
    const pid_t pid = fork();
    if (pid == -1) {
        return; /* Some error occurred */
    }
    if (pid == 0) {  // child
        dup2(input_pipe.read_end, STDIN_FILENO);
        close(input_pipe.read_end);
        close(input_pipe.write_end);
        execl(applicationPath, "signal_tracer", nullptr);
        const char* exec_failure_message =
            "exec(signal_tracer) failed: Make sure the signal_tracer executable is in "
            "the current working directory and the binary's permissions are correct.\n";
        write(STDERR_FILENO, exec_failure_message, strlen(exec_failure_message));
        _exit(1);
    }
    // Resolve to safe_object_frames and write those to the pipe
    for (std::size_t i = 0; i < count; i++) {
        cpptrace::safe_object_frame frame;
        cpptrace::get_safe_object_frame(buffer[i], &frame);
        write(input_pipe.write_end, &frame, sizeof(frame));
    }
    close(input_pipe.read_end);
    close(input_pipe.write_end);
    // Wait for child
    waitpid(pid, nullptr, 0);
}

void signal_handler(int)
{
    // Print basic message
    const char* message = "SIGSEGV occurred:\n";
    write(STDERR_FILENO, message, strlen(message));
    // Generate trace
    constexpr std::size_t N = 100;
    cpptrace::frame_ptr buffer[N];
    std::size_t count = cpptrace::safe_generate_raw_trace(buffer, N);
    do_signal_safe_trace(buffer, count);
    // Up to you if you want to exit or continue or whatever
    _exit(1);
}

#if defined Q_OS_WIN32
#include <windows.h>

LONG WINAPI CustomUnhandledExceptionFilter(EXCEPTION_POINTERS* exceptionInfo)
{
    signal_handler(0);
    return EXCEPTION_CONTINUE_SEARCH;  // Continue searching for another handler
}
#endif
void customTerminate()
{
    signal_handler(0);
}

void setup_crash_handler()
{
    // Setup signal handler for common crash signals
    std::signal(SIGSEGV, signal_handler);  // Segmentation fault
    std::signal(SIGABRT, signal_handler);  // Abort signal
    std::signal(SIGILL, signal_handler);
    std::signal(SIGFPE, signal_handler);
    std::set_terminate(customTerminate);
#if defined Q_OS_WIN32
    SetUnhandledExceptionFilter(CustomUnhandledExceptionFilter);
#endif
}

void warmup_cpptrace()
{
    cpptrace::frame_ptr buffer[10];
    cpptrace::safe_generate_raw_trace(buffer, 10);
    cpptrace::safe_object_frame frame;
    cpptrace::get_safe_object_frame(buffer[0], &frame);
}

int main(int argc, char* argv[])
{
#ifdef BREAK_INFINITE_LOOP
    while (true) {
        std::this_thread::sleep_for(std::chrono::milliseconds(250));
    }
#endif
#ifdef BREAK_EXCEPTION
    throw 42;
#endif
#ifdef BREAK_RETURN
    return 42;
#endif

    auto appFilePathCStr = QApplication::applicationFilePath().toUtf8().constData();
    applicationPath = new char[strlen(appFilePathCStr) + 1];
    strcpy(applicationPath, appFilePathCStr);

    QStringList arguments;
    for (int i = 0; i < argc; ++i) {
        arguments << QString::fromUtf8(argv[i]);
    }

    if (arguments.size() > 1 && arguments.at(1) == "signal_tracer") {
        cpptrace::object_trace trace;
        while (true) {
            cpptrace::safe_object_frame frame;
            // fread used over read because a read() from a pipe might not read the full frame
            std::size_t res = fread(&frame, sizeof(frame), 1, stdin);
            if (res == 0) {
                break;
            } else if (res != 1) {
                std::cerr << "Something went wrong while reading from the pipe" << res << " " << std::endl;
                break;
            } else {
                trace.frames.push_back(frame.resolve());
            }
        }
        FS::write("crash.log", QByteArray::fromStdString(trace.resolve().to_string()));
        return 1;
    }

    warmup_cpptrace();
    setup_crash_handler();

#if QT_VERSION <= QT_VERSION_CHECK(6, 0, 0)
    QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QGuiApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);
#endif

    // initialize Qt
    Application app(argc, argv);

    switch (app.status()) {
        case Application::StartingUp:
        case Application::Initialized: {
            Q_INIT_RESOURCE(multimc);
            Q_INIT_RESOURCE(backgrounds);
            Q_INIT_RESOURCE(documents);
            Q_INIT_RESOURCE(prismlauncher);

            Q_INIT_RESOURCE(pe_dark);
            Q_INIT_RESOURCE(pe_light);
            Q_INIT_RESOURCE(pe_blue);
            Q_INIT_RESOURCE(pe_colored);
            Q_INIT_RESOURCE(breeze_dark);
            Q_INIT_RESOURCE(breeze_light);
            Q_INIT_RESOURCE(OSX);
            Q_INIT_RESOURCE(iOS);
            Q_INIT_RESOURCE(flat);
            Q_INIT_RESOURCE(flat_white);
            return app.exec();
        }
        case Application::Failed:
            return 1;
        case Application::Succeeded:
            return 0;
        default:
            return -1;
    }
}
