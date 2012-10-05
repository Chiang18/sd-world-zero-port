using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace WorldEditor.Base.Interfaces.Services
{
    // 日志等级
    public enum LogLevel
    {
        Error,
        Warnning,
        Message,
        Info,
        Debug,
        Status,
    }

    // 编辑器日志接口
    interface ILoggingService : IService
    {
        // 记录日志
        void Log(String msg, LogLevel level);
    }
}
