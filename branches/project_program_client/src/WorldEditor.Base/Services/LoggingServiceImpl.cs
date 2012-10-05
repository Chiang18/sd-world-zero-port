using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading;
using WorldEditor.Base.Interfaces.Services;

namespace WorldEditor.Base.Services
{
    //*****************************************************************************************
    // 关于C#数据流(Stream)
    //      Stream
    //      StreamReader/TextReader
    //      StreamWriter/TextWriter
    //      FileStream
    //      MemoryStream
    //      BufferredStream
    //      NetworkStream
    //*****************************************************************************************
    // 日志事件
    class LogEvent
    {
        private LogLevel mLevel;
        public LogLevel Level
        {
            get { return mLevel;}
        }

        private String mMessage;
        public String Message
        {
            get { return mMessage;}
        }

        private DateTime mTime;
        public DateTime Time
        {
            get { return mTime;}
        }

        public LogEvent(String msg, DateTime time, LogLevel level)
        {
            this.mMessage = msg;
            this.mTime = time;
            this.mLevel = level;
        }
    }


    // 日志服务实现
    class LoggingServiceImpl : StandardServiceBaseImpl, Interfaces.Services.ILoggingService
    {
        private bool mEnable;
        public bool Enable
        {
            get { return mEnable;}
            set { mEnable = value;}
        }

        private Queue<LogEvent> mLogQueue;  // 日志记录队列
        private StreamWriter mStreamWriter; // 日志输出流

        public LoggingServiceImpl()
        {
            // 创建文件流
            FileStream fileStream = new FileStream("WorldEditor.log",
                FileMode.Append, FileAccess.Write, FileShare.ReadWrite, 1024, false);

            // 创建输出数据流
            mStreamWriter = new StreamWriter(fileStream);
            mStreamWriter.AutoFlush = false;

            // 创建消息队列
            mLogQueue = new Queue<LogEvent>();
        }

        // 虚函数继承,将消息加入日志队列
        public void Log(String msg, LogLevel level)
        {
            // 日志消息事件
            LogEvent logEvent = new LogEvent(msg, DateTime.Now, level);

            // 加入队列
            Thread.BeginCriticalRegion();
            mLogQueue.Enqueue(logEvent);
            Thread.EndCriticalRegion();
        }

        // 将日志
        public override void Update()
        {
            
        }
    }
}
