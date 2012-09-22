using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using WorldEditorFramework;
using WorldEditor.SceneModule.Widget;


namespace WorldEditor.SceneModule.Window
{
    public partial class TerrainForm : WeifenLuo.WinFormsUI.Docking.DockContent
    {
        // 操作接口
        MWorldEditorFSM mWorldEditorFSM;

        //
        private TerrainDeformWidget mTerrainDeformWidget;
        private TerrainSurfaceWidget mTerrainSurfaceWidget;

        public TerrainForm()
        {
            InitializeComponent();

            // 地表高度面板
            mTerrainDeformWidget = new TerrainDeformWidget();
            mTerrainDeformWidget.Dock = DockStyle.Fill;
            mTerrainDeformWidget.Location = new Point(5, 13);
            mTerrainDeformWidget.Visible = false;
            this.groupBoxProperty.Controls.Add(mTerrainDeformWidget);

            // 地表纹理面板
            mTerrainSurfaceWidget = new TerrainSurfaceWidget();
            mTerrainSurfaceWidget.Dock = DockStyle.Fill;
            mTerrainSurfaceWidget.Location = new Point(5, 13);
            mTerrainSurfaceWidget.Visible = false;
            this.groupBoxProperty.Controls.Add(mTerrainSurfaceWidget);
        }

        // 初始化
        public void Initialize()
        {

        }

        // @{
        //*********************************************************************
        // 面板,用于处理操作接口与显示数据
        //*********************************************************************
        // 初始化
        public void OnSceneLoad()
        {
            // 操作接口
            mWorldEditorFSM = new MWorldEditorFSM();

            // 
            mTerrainSurfaceWidget.OnSceneLoad();
            mTerrainDeformWidget.OnSceneLoad();
        }

        public void OnSceneClose()
        {

        }

        // 
        public void OnOpenPanel()
        {

        }

        //
        public void OnClosePanel()
        {

        }
        // @}

        // @{
        //*********************************************************************
        // 窗口消息响应
        //*********************************************************************
        private void TerrainForm_Enter(object sender, EventArgs e)
        {

        }

        private void TerrainForm_Leave(object sender, EventArgs e)
        {

        }
        // @}

        // @{
        //*********************************************************************
        // 控件消息响应
        //*********************************************************************
        private void buttonTerrainDeform_Click(object sender, EventArgs e)
        {
            mTerrainSurfaceWidget.Visible = false;
            mTerrainSurfaceWidget.OnClosePanel();

            mTerrainDeformWidget.Visible = true;
            mTerrainDeformWidget.OnOpenPanel();

            this.groupBoxProperty.Size = new Size(8, 100) + mTerrainDeformWidget.Size;

            mWorldEditorFSM.SetEditState(6);
        }

        private void buttonTerrainSurface_Click(object sender, EventArgs e)
        {
            mTerrainDeformWidget.Visible = false;
            mTerrainDeformWidget.OnClosePanel();

            mTerrainSurfaceWidget.Visible = true;
            mTerrainSurfaceWidget.OnOpenPanel();

            this.groupBoxProperty.Size = new Size(8, 100) + mTerrainSurfaceWidget.Size;

            mWorldEditorFSM.SetEditState(7);
        }
        // @}
    }
}
