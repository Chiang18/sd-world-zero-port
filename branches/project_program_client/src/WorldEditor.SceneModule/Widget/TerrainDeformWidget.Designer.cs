namespace WorldEditor.SceneModule.Widget
{
    partial class TerrainDeformWidget
    {
        /// <summary> 
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary> 
        /// Clean up any resources being used.
        /// </summary>
        /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Component Designer generated code

        /// <summary> 
        /// Required method for Designer support - do not modify 
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            this.components = new System.ComponentModel.Container();
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(TerrainDeformWidget));
            this.toolBarBrushType = new System.Windows.Forms.ToolBar();
            this.toolBarButtonPull = new System.Windows.Forms.ToolBarButton();
            this.toolBarButtonSmooth = new System.Windows.Forms.ToolBarButton();
            this.toolBarButtonNoise = new System.Windows.Forms.ToolBarButton();
            this.toolBarButtonAverage = new System.Windows.Forms.ToolBarButton();
            this.toolBarButtonFlat = new System.Windows.Forms.ToolBarButton();
            this.imageListBrushType = new System.Windows.Forms.ImageList(this.components);
            this.trackerBarWidgetOutterRadius = new WorldEditor.Common.CustomWidget.TrackerBarWidget();
            this.trackerBarWidgetInnerRadius = new WorldEditor.Common.CustomWidget.TrackerBarWidget();
            this.trackerBarWidgetBrushPower = new WorldEditor.Common.CustomWidget.TrackerBarWidget();
            this.trackerBarWidgetHeight = new WorldEditor.Common.CustomWidget.TrackerBarWidget();
            this.SuspendLayout();
            // 
            // toolBarBrushType
            // 
            this.toolBarBrushType.Buttons.AddRange(new System.Windows.Forms.ToolBarButton[] {
            this.toolBarButtonPull,
            this.toolBarButtonSmooth,
            this.toolBarButtonNoise,
            this.toolBarButtonAverage,
            this.toolBarButtonFlat});
            this.toolBarBrushType.ButtonSize = new System.Drawing.Size(32, 32);
            this.toolBarBrushType.Cursor = System.Windows.Forms.Cursors.Arrow;
            this.toolBarBrushType.Divider = false;
            this.toolBarBrushType.DropDownArrows = true;
            this.toolBarBrushType.ImageList = this.imageListBrushType;
            this.toolBarBrushType.Location = new System.Drawing.Point(0, 0);
            this.toolBarBrushType.Name = "toolBarBrushType";
            this.toolBarBrushType.ShowToolTips = true;
            this.toolBarBrushType.Size = new System.Drawing.Size(250, 39);
            this.toolBarBrushType.TabIndex = 2;
            // 
            // toolBarButtonPull
            // 
            this.toolBarButtonPull.ImageIndex = 3;
            this.toolBarButtonPull.Name = "toolBarButtonPull";
            // 
            // toolBarButtonSmooth
            // 
            this.toolBarButtonSmooth.ImageIndex = 4;
            this.toolBarButtonSmooth.Name = "toolBarButtonSmooth";
            // 
            // toolBarButtonNoise
            // 
            this.toolBarButtonNoise.ImageIndex = 2;
            this.toolBarButtonNoise.Name = "toolBarButtonNoise";
            // 
            // toolBarButtonAverage
            // 
            this.toolBarButtonAverage.ImageIndex = 0;
            this.toolBarButtonAverage.Name = "toolBarButtonAverage";
            // 
            // toolBarButtonFlat
            // 
            this.toolBarButtonFlat.ImageIndex = 1;
            this.toolBarButtonFlat.Name = "toolBarButtonFlat";
            // 
            // imageListBrushType
            // 
            this.imageListBrushType.ImageStream = ((System.Windows.Forms.ImageListStreamer)(resources.GetObject("imageListBrushType.ImageStream")));
            this.imageListBrushType.TransparentColor = System.Drawing.SystemColors.Control;
            this.imageListBrushType.Images.SetKeyName(0, "TerrainAverageBrush.png");
            this.imageListBrushType.Images.SetKeyName(1, "TerrainFlatBrush.png");
            this.imageListBrushType.Images.SetKeyName(2, "TerrainNoiseBrush.png");
            this.imageListBrushType.Images.SetKeyName(3, "TerrainPullBrush.png");
            this.imageListBrushType.Images.SetKeyName(4, "TerrainSmoothBrush.png");
            // 
            // trackerBarWidgetOutterRadius
            // 
            this.trackerBarWidgetOutterRadius.DecimalPlaces = 0;
            this.trackerBarWidgetOutterRadius.Increment = new decimal(new int[] {
            1,
            0,
            0,
            65536});
            this.trackerBarWidgetOutterRadius.InnerScale = new decimal(new int[] {
            10,
            0,
            0,
            0});
            this.trackerBarWidgetOutterRadius.LabelCaption = "外圈半径";
            this.trackerBarWidgetOutterRadius.Location = new System.Drawing.Point(3, 45);
            this.trackerBarWidgetOutterRadius.MaxValue = new decimal(new int[] {
            20,
            0,
            0,
            0});
            this.trackerBarWidgetOutterRadius.MinValue = new decimal(new int[] {
            0,
            0,
            0,
            0});
            this.trackerBarWidgetOutterRadius.Name = "trackerBarWidgetOutterRadius";
            this.trackerBarWidgetOutterRadius.Size = new System.Drawing.Size(241, 55);
            this.trackerBarWidgetOutterRadius.TabIndex = 3;
            this.trackerBarWidgetOutterRadius.Value = new decimal(new int[] {
            5,
            0,
            0,
            0});
            // 
            // trackerBarWidgetInnerRadius
            // 
            this.trackerBarWidgetInnerRadius.DecimalPlaces = 0;
            this.trackerBarWidgetInnerRadius.Increment = new decimal(new int[] {
            1,
            0,
            0,
            65536});
            this.trackerBarWidgetInnerRadius.InnerScale = new decimal(new int[] {
            10,
            0,
            0,
            0});
            this.trackerBarWidgetInnerRadius.LabelCaption = "内圈半径";
            this.trackerBarWidgetInnerRadius.Location = new System.Drawing.Point(3, 106);
            this.trackerBarWidgetInnerRadius.MaxValue = new decimal(new int[] {
            20,
            0,
            0,
            0});
            this.trackerBarWidgetInnerRadius.MinValue = new decimal(new int[] {
            0,
            0,
            0,
            0});
            this.trackerBarWidgetInnerRadius.Name = "trackerBarWidgetInnerRadius";
            this.trackerBarWidgetInnerRadius.Size = new System.Drawing.Size(241, 55);
            this.trackerBarWidgetInnerRadius.TabIndex = 4;
            this.trackerBarWidgetInnerRadius.Value = new decimal(new int[] {
            5,
            0,
            0,
            0});
            // 
            // trackerBarWidgetBrushPower
            // 
            this.trackerBarWidgetBrushPower.DecimalPlaces = 0;
            this.trackerBarWidgetBrushPower.Increment = new decimal(new int[] {
            1,
            0,
            0,
            65536});
            this.trackerBarWidgetBrushPower.InnerScale = new decimal(new int[] {
            10,
            0,
            0,
            0});
            this.trackerBarWidgetBrushPower.LabelCaption = "强度";
            this.trackerBarWidgetBrushPower.Location = new System.Drawing.Point(6, 167);
            this.trackerBarWidgetBrushPower.MaxValue = new decimal(new int[] {
            20,
            0,
            0,
            0});
            this.trackerBarWidgetBrushPower.MinValue = new decimal(new int[] {
            0,
            0,
            0,
            0});
            this.trackerBarWidgetBrushPower.Name = "trackerBarWidgetBrushPower";
            this.trackerBarWidgetBrushPower.Size = new System.Drawing.Size(238, 55);
            this.trackerBarWidgetBrushPower.TabIndex = 5;
            this.trackerBarWidgetBrushPower.Value = new decimal(new int[] {
            5,
            0,
            0,
            0});
            // 
            // trackerBarWidgetHeight
            // 
            this.trackerBarWidgetHeight.DecimalPlaces = 0;
            this.trackerBarWidgetHeight.Increment = new decimal(new int[] {
            1,
            0,
            0,
            65536});
            this.trackerBarWidgetHeight.InnerScale = new decimal(new int[] {
            10,
            0,
            0,
            0});
            this.trackerBarWidgetHeight.LabelCaption = "高度";
            this.trackerBarWidgetHeight.Location = new System.Drawing.Point(6, 228);
            this.trackerBarWidgetHeight.MaxValue = new decimal(new int[] {
            20,
            0,
            0,
            0});
            this.trackerBarWidgetHeight.MinValue = new decimal(new int[] {
            0,
            0,
            0,
            0});
            this.trackerBarWidgetHeight.Name = "trackerBarWidgetHeight";
            this.trackerBarWidgetHeight.Size = new System.Drawing.Size(238, 54);
            this.trackerBarWidgetHeight.TabIndex = 6;
            this.trackerBarWidgetHeight.Value = new decimal(new int[] {
            5,
            0,
            0,
            0});
            // 
            // TerrainDeformWidget
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 12F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.Controls.Add(this.trackerBarWidgetHeight);
            this.Controls.Add(this.trackerBarWidgetBrushPower);
            this.Controls.Add(this.trackerBarWidgetInnerRadius);
            this.Controls.Add(this.trackerBarWidgetOutterRadius);
            this.Controls.Add(this.toolBarBrushType);
            this.Name = "TerrainDeformWidget";
            this.Size = new System.Drawing.Size(250, 342);
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.ToolBar toolBarBrushType;
        private System.Windows.Forms.ImageList imageListBrushType;
        private System.Windows.Forms.ToolBarButton toolBarButtonPull;
        private System.Windows.Forms.ToolBarButton toolBarButtonSmooth;
        private System.Windows.Forms.ToolBarButton toolBarButtonNoise;
        private System.Windows.Forms.ToolBarButton toolBarButtonAverage;
        private System.Windows.Forms.ToolBarButton toolBarButtonFlat;
        private WorldEditor.Common.CustomWidget.TrackerBarWidget trackerBarWidgetOutterRadius;
        private WorldEditor.Common.CustomWidget.TrackerBarWidget trackerBarWidgetInnerRadius;
        private WorldEditor.Common.CustomWidget.TrackerBarWidget trackerBarWidgetBrushPower;
        private WorldEditor.Common.CustomWidget.TrackerBarWidget trackerBarWidgetHeight;
    }
}
