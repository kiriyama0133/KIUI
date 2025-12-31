#ifndef LOGGER_HPP
#define LOGGER_HPP
#pragma once

#ifdef _WIN32
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#ifndef NOMINMAX
#define NOMINMAX
#endif
#endif

#include <string>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <chrono>
#include <ctime>
#include <vector>

#ifdef _WIN32
#include <windows.h>
#endif

namespace KiUI {
namespace foundation {

/**
 * @brief 日志级别枚举
 */
enum class LogLevel {
    Debug,  // 调试信息（蓝色）
    Info,   // 信息（绿色）
    Error   // 错误（红色）
};

/**
 * @brief 日志工具类
 * 支持三种日志级别：Debug（蓝色）、Info（绿色）、Error（红色）
 * 使用 fmt 风格的格式化字符串
 */
class Logger {
public:
    /**
     * @brief 格式化并输出日志
     * @tparam Args 参数类型
     * @param level 日志级别
     * @param format 格式化字符串（fmt 风格）
     * @param args 参数
     */
    template<typename... Args>
    static void Log(LogLevel level, const std::string& format, Args&&... args) {
        // 格式化消息
        std::string message = FormatMessage(format, std::forward<Args>(args)...);
        
        // 获取时间戳
        std::string timestamp = GetTimestamp();
        
        // 获取级别字符串
        std::string levelStr = GetLevelString(level);
        
        // 设置颜色并输出
        SetConsoleColor(level);
        std::cout << "[" << timestamp << "] [" << levelStr << "] " << message << std::endl;
        ResetConsoleColor();
    }

    /**
     * @brief Debug 日志（蓝色）
     */
    template<typename... Args>
    static void Debug(const std::string& format, Args&&... args) {
        Log(LogLevel::Debug, format, std::forward<Args>(args)...);
    }

    /**
     * @brief Info 日志（绿色）
     */
    template<typename... Args>
    static void Info(const std::string& format, Args&&... args) {
        Log(LogLevel::Info, format, std::forward<Args>(args)...);
    }

    /**
     * @brief Error 日志（红色）
     */
    template<typename... Args>
    static void Error(const std::string& format, Args&&... args) {
        Log(LogLevel::Error, format, std::forward<Args>(args)...);
    }

private:
    /**
     * @brief 格式化消息（fmt 风格实现）
     * 支持 {0}, {1}, {2}... 占位符（类似 fmt::format）
     * 如果项目中有 fmt 库，可以使用 fmt::format 替代此实现
     */
    template<typename... Args>
    static std::string FormatMessage(const std::string& format, Args&&... args) {
        std::string result = format;
        std::vector<std::string> argStrings;
        (argStrings.push_back(ToString(std::forward<Args>(args))), ...);
        
        for (size_t i = 0; i < argStrings.size(); ++i) {
            std::string placeholder = "{" + std::to_string(i) + "}";
            size_t pos = 0;
            while ((pos = result.find(placeholder, pos)) != std::string::npos) {
                result.replace(pos, placeholder.length(), argStrings[i]);
                pos += argStrings[i].length();
            }
        }
        
        return result;
    }

    /**
     * @brief 将参数转换为字符串
     */
    template<typename T>
    static std::string ToString(const T& value) {
        std::ostringstream oss;
        oss << value;
        return oss.str();
    }

    /**
     * @brief 获取时间戳字符串
     */
    static std::string GetTimestamp() {
        auto now = std::chrono::system_clock::now();
        auto time = std::chrono::system_clock::to_time_t(now);
        auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
            now.time_since_epoch()) % 1000;
        
        std::tm tm_buf;
#ifdef _WIN32
        localtime_s(&tm_buf, &time);
#else
        localtime_r(&time, &tm_buf);
#endif
        
        std::ostringstream oss;
        oss << std::put_time(&tm_buf, "%Y-%m-%d %H:%M:%S");
        oss << "." << std::setfill('0') << std::setw(3) << ms.count();
        return oss.str();
    }

    /**
     * @brief 获取日志级别字符串
     */
    static std::string GetLevelString(LogLevel level) {
        switch (level) {
            case LogLevel::Debug: return "DEBUG";
            case LogLevel::Info:  return "INFO ";
            case LogLevel::Error: return "ERROR";
            default: return "UNKNOWN";
        }
    }

    /**
     * @brief 设置控制台颜色
     */
    static void SetConsoleColor(LogLevel level) {
#ifdef _WIN32
        HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
        if (hConsole == INVALID_HANDLE_VALUE) return;
        
        WORD color = FOREGROUND_INTENSITY;
        switch (level) {
            case LogLevel::Debug:
                color |= FOREGROUND_BLUE;  // 蓝色
                break;
            case LogLevel::Info:
                color |= FOREGROUND_GREEN; // 绿色
                break;
            case LogLevel::Error:
                color |= FOREGROUND_RED;   // 红色
                break;
        }
        SetConsoleTextAttribute(hConsole, color);
#else
        // Linux/Mac 使用 ANSI 颜色代码
        switch (level) {
            case LogLevel::Debug:
                std::cout << "\033[34m";  // 蓝色
                break;
            case LogLevel::Info:
                std::cout << "\033[32m";  // 绿色
                break;
            case LogLevel::Error:
                std::cout << "\033[31m";  // 红色
                break;
        }
#endif
    }

    /**
     * @brief 重置控制台颜色
     */
    static void ResetConsoleColor() {
#ifdef _WIN32
        HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
        if (hConsole != INVALID_HANDLE_VALUE) {
            SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
        }
#else
        std::cout << "\033[0m";  // 重置颜色
#endif
    }
};

} // namespace foundation
} // namespace KiUI

#endif // LOGGER_HPP

