using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using System.IO;
using WeifenLuo.WinFormsUI.Docking;
using WorldEditorFramework;

namespace WorldEditor
{
    public partial class FormMain : Form
    {
        private MWorldEditor mWorldEditor;  // 主操作接口

        private SceneForm mSceneForm;       // 主窗口
        private OperateForm mOperateForm;   // 操作窗口

        public FormMain()
        {
            InitializeComponent();

            mOperateForm = new OperateForm();
            mSceneForm = new SceneForm();

            //// 初始化操作接口
            //mWorldEditor = new MWorldEditor();
            //mWorldEditor.Initialize(this.Handle);
        }

        // 系统更新回调函数
        public void FormMain_Idle(object sender, EventArgs e)
        {
            if (this.Visible && FormWindowState.Minimized != this.WindowState)
            {
                // 窗口可见,且非最小化
                //
                // 更新操作接口
                if (mWorldEditor != null)
                    mWorldEditor.Update();
            }
        }

        //*****************************************************************************************
        // C# 中消息的工作流程：
        //  1.C#中的消息被Application类从应用程序消息队列中取出
        //  2.Application类的IMessageFilter进行预过滤,然后分发到消息对应的窗体
        //  2.窗体对象的第一个响应函数是对象中的 
        //      protected override void WndProc(ref System.Windows.Forms.Message e)
        //  3.它再根据消息的类型调用默认的消息响应函数,如
        //      protected override void OnMouseDown(MouseEventArgs e)
        //  4.默认的响应函数然后根据对象的事件字段(如this.MouseDown )中的函数指针列表，
        //    调用用户所加入的响应函数(如Form1_MouseDown1和Form1_MouseDown2)，
        //    而且调用顺序和用户添加顺序一致。 
        //*****************************************************************************************
        // 输入设备(键盘鼠标)消息
        protected override void WndProc(ref System.Windows.Forms.Message e)
        {
            if (mWorldEditor != null)
                mWorldEditor.WndProc(e);
        }


        // @{
        //*****************************************************************************************
        // 消息回调函数
        //*****************************************************************************************
        private void FormMain_Load(object sender, EventArgs e)
        {
            // 加载配置界面
            String szConfigFile = Path.Combine(Path.GetDirectoryName(Application.ExecutablePath), "WorldEditor.DockPanel.config");
            if (File.Exists(szConfigFile))
            {
                DeserializeDockContent mDeserializeDockContent = new DeserializeDockContent(GetDockContentFromPersistString);
                this.dockPanelMain.LoadFromXml(szConfigFile, mDeserializeDockContent);
            }
            else
            {
                this.mOperateForm.Show(this.dockPanelMain, DockState.DockRight);
                this.mSceneForm.Show(this.dockPanelMain, DockState.DockLeft);
            }

            // 初始化操作接口
            this.mWorldEditor = new MWorldEditor();
            this.mWorldEditor.Initialize(this.mSceneForm.Handle);
        }

        private void FormMain_FormClosing(object sender, FormClosingEventArgs e)
        {
            // 保存配置界面
            String szConfigFile = Path.Combine(Path.GetDirectoryName(Application.ExecutablePath), "WorldEditor.DockPanel.config");
            this.dockPanelMain.SaveAsXml(szConfigFile);
        }
        //*****************************************************************************************
        // @}


        // 用于DockPanel的布局保存
        private IDockContent GetDockContentFromPersistString(string szPersist)
        {
            if (typeof(SceneForm).ToString() == szPersist)
                return mSceneForm;
            else if (typeof(OperateForm).ToString() == szPersist)
                return mOperateForm;
            else
                return null;
        }
    }
}
