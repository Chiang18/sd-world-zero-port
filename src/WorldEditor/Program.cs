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
            // 新建主窗口
            FormMain formMain = new FormMain();

            // 注册更新回调函数
            Application.Idle += new EventHandler(formMain.FormMain_Idle);

            // 运行,进入主循环
            Application.EnableVisualStyles();
            //Application.SetCompatibleTextRenderingDefault(false);
            Application.Run(formMain);
        }
    }
}
