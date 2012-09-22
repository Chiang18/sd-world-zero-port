using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using System.Windows;
using System.Windows.Forms;
using WorldEditorFramework;
using WorldEditor.Base;
using WorldEditor.SceneModule.Window;
using WeifenLuo.WinFormsUI.Docking;

namespace WorldEditor.SceneModule.Module
{
    public class SceneModule : TSingleton<SceneModule>
    {
        // 主操作接口
        private MWorldEditor mWorldEditor;
        public MWorldEditor WorldEditor
        {
            get { return mWorldEditor; }
        }


        // 窗口
        private MainForm    mMainForm;      // 主窗口
        private OutputForm  mOutputForm;    // 输出窗口
        private SceneForm   mSceneForm;     // 场景编辑窗口
        private TerrainForm mTerrainForm;   // 地形相关编辑窗口

        //
        public SceneModule()
        {
            // 新建面板
            this.mMainForm = new MainForm();
            this.mOutputForm = new OutputForm();
            this.mSceneForm = new SceneForm();
            this.mTerrainForm = new TerrainForm();
        }

        // 初始化
        public bool Initialize(DockPanel dockPanel)
        {
            // 加载配置界面
            String applicationPath = System.Diagnostics.Process.GetCurrentProcess().MainModule.FileName;
            String configFile = Path.Combine(Path.GetDirectoryName(applicationPath), "WorldEditor.DockPanel.config");
            if (File.Exists(configFile))
            {
                DeserializeDockContent mDeserializeDockContent = new DeserializeDockContent(GetDockContentFromPersistString);
                dockPanel.LoadFromXml(configFile, mDeserializeDockContent);
            }
            else
            {
                // 显示
                dockPanel.SuspendLayout(true);
                this.mTerrainForm.Show(dockPanel, DockState.DockRight);
                this.mSceneForm.Show(dockPanel, DockState.DockRight);
                this.mMainForm.Show(dockPanel, DockState.Document);
                //this.mOutputForm.Show(dockPanel, DockState.DockRight);
                dockPanel.ResumeLayout(true, true);
            }

            // 初始化操作接口
            this.mWorldEditor = new WorldEditorFramework.MWorldEditor();
            this.mWorldEditor.Initialize(this.mMainForm.Handle);

            this.mTerrainForm.OnSceneLoad();

            return true;
        }

        // 销毁
        public void Destroy(DockPanel dockPanel)
        {
            // 保存配置界面
            String applicationPath = System.Diagnostics.Process.GetCurrentProcess().MainModule.FileName;
            String szConfigFile = Path.Combine(Path.GetDirectoryName(applicationPath), "WorldEditor.DockPanel.config");
            dockPanel.SaveAsXml(szConfigFile);
        }

        // 更新
        public void Update()
        {
            if (mWorldEditor != null)
                mWorldEditor.Update();
        }

        // 输入消息
        public void WndProc(ref Message m)
        {
            if (mWorldEditor != null)
                mWorldEditor.WndProc(ref m);
        }


        // 用于DockPanel的布局保存
        private IDockContent GetDockContentFromPersistString(string szPersist)
        {
            if (typeof(MainForm).ToString() == szPersist)
                return mMainForm;
            else if (typeof(SceneForm).ToString() == szPersist)
                return mSceneForm;
            else if (typeof(TerrainForm).ToString() == szPersist)
                return mTerrainForm;
            else if (typeof(OutputForm).ToString() == szPersist)
                return mOutputForm;
            else
                return null;
        }
    }
}
