using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using WorldEditorFramework;

namespace WorldEditor.SceneModule.Widget
{
    public partial class TerrainSurfacePanel : UserControl
    {
        // 操作接口
        MEditTerrainHelper mEditTerrainHelper;

        public TerrainSurfacePanel()
        {
            InitializeComponent();
        }

        // @{
        //*********************************************************************
        // 面板,用于处理操作接口与显示数据
        //*********************************************************************
        public void OnSceneLoad()
        {
            // 操作接口
            mEditTerrainHelper = new MEditTerrainHelper();
        }

        public void OnSceneClose()
        {
           
        }

        public void OnOpenPanel()
        {
            // 
            mEditTerrainHelper.GetLayerMapCount();

            // 刷新图层列表
            RefreshLayerMapList();
        }

        public void OnClosePanel()
        {

        }
        // @}


        // @{
        //*********************************************************************
        // 消息响应/UI回调函数
        //*********************************************************************
        private void listBoxLayer_SelectedIndexChanged(object sender, EventArgs e)
        {
            int iIndex = listBoxLayer.SelectedIndex;
            if (iIndex < 0)
                return;

            mEditTerrainHelper.SetActiveLayerMap((uint)iIndex);
        }
        
        // @}

        // 刷新图层列表
        private void RefreshLayerMapList()
        {
            listBoxLayer.Items.Clear();

            uint uiNum = mEditTerrainHelper.GetLayerMapCount();
            for (uint i = 0; i < uiNum; ++i)
            {
                listBoxLayer.Items.Add(mEditTerrainHelper.GetLayerMapName(i));
            }
        }
    }
}
