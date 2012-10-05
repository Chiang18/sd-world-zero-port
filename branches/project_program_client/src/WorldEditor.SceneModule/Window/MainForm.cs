using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using WorldEditor.SceneModule.Module;
using WorldEditor.Common;
using WorldEditor.Common.Common;

namespace WorldEditor.SceneModule.Window
{
    public partial class MainForm : WeifenLuo.WinFormsUI.Docking.DockContent
    {
        public MainForm()
        {
            InitializeComponent();
        }

        // @{
        //*****************************************************************************************
        // 窗口消息重载,主要是输入
        //*****************************************************************************************
        protected override void WndProc(ref System.Windows.Forms.Message e)
        {
            if (e.Msg == (int)(Win32.WinMsg.WM_LBUTTONDOWN))
            {
                Win32.SetForegroundWindow(this.Handle);
            }

            if (Module.SceneModule.Instance.WorldEditor != null)
            {
                Module.SceneModule.Instance.WorldEditor.WndProc(ref e);
            }

            base.WndProc(ref e);
        }
        // @}
    }
}
