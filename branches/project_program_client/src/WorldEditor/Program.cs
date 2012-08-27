using System;
using System.Collections.Generic;
using System.Linq;
using System.Windows.Forms;

namespace WorldEditor
{
    static class Program
    {
        /// <summary>
        /// The main entry point for the application.
        /// </summary>
        [STAThread]
        static void Main()
        {
            // 设置
            Application.EnableVisualStyles();
            //Application.SetCompatibleTextRenderingDefault(false);

            // 新建主窗口,运行
            Application.Run(new FormMain());
        }
    }
}
