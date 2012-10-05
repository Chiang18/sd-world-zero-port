namespace WorldEditor.SceneModule.Widget
{
    partial class TerrainSurfacePanel
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
            this.trackerBarWidgetInnerRadius = new WorldEditor.Common.CustomWidget.TrackerBarWidget();
            this.trackerBarWidgetOutterRadius = new WorldEditor.Common.CustomWidget.TrackerBarWidget();
            this.trackerBarWidgetBrushPower = new WorldEditor.Common.CustomWidget.TrackerBarWidget();
            this.groupBox1 = new System.Windows.Forms.GroupBox();
            this.trackerBarWidget2 = new WorldEditor.Common.CustomWidget.TrackerBarWidget();
            this.trackerBarWidget1 = new WorldEditor.Common.CustomWidget.TrackerBarWidget();
            this.groupBox2 = new System.Windows.Forms.GroupBox();
            this.pictureBox2 = new System.Windows.Forms.PictureBox();
            this.groupBoxLayerDiffuse = new System.Windows.Forms.GroupBox();
            this.pictureBox1 = new System.Windows.Forms.PictureBox();
            this.listBoxLayer = new System.Windows.Forms.ListBox();
            this.buttonLayerAdd = new System.Windows.Forms.Button();
            this.buttonLayerRemove = new System.Windows.Forms.Button();
            this.buttonLayerUp = new System.Windows.Forms.Button();
            this.buttonLayerDown = new System.Windows.Forms.Button();
            this.groupBoxLayer = new System.Windows.Forms.GroupBox();
            this.groupBox1.SuspendLayout();
            this.groupBox2.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.pictureBox2)).BeginInit();
            this.groupBoxLayerDiffuse.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.pictureBox1)).BeginInit();
            this.groupBoxLayer.SuspendLayout();
            this.SuspendLayout();
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
            this.trackerBarWidgetInnerRadius.Location = new System.Drawing.Point(3, 64);
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
            this.trackerBarWidgetInnerRadius.TabIndex = 6;
            this.trackerBarWidgetInnerRadius.Value = new decimal(new int[] {
            5,
            0,
            0,
            0});
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
            this.trackerBarWidgetOutterRadius.Location = new System.Drawing.Point(3, 3);
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
            this.trackerBarWidgetOutterRadius.TabIndex = 5;
            this.trackerBarWidgetOutterRadius.Value = new decimal(new int[] {
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
            this.trackerBarWidgetBrushPower.Location = new System.Drawing.Point(6, 125);
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
            this.trackerBarWidgetBrushPower.TabIndex = 7;
            this.trackerBarWidgetBrushPower.Value = new decimal(new int[] {
            5,
            0,
            0,
            0});
            // 
            // groupBox1
            // 
            this.groupBox1.Controls.Add(this.trackerBarWidget2);
            this.groupBox1.Controls.Add(this.trackerBarWidget1);
            this.groupBox1.Controls.Add(this.groupBox2);
            this.groupBox1.Controls.Add(this.groupBoxLayerDiffuse);
            this.groupBox1.Location = new System.Drawing.Point(6, 376);
            this.groupBox1.Name = "groupBox1";
            this.groupBox1.Size = new System.Drawing.Size(238, 257);
            this.groupBox1.TabIndex = 9;
            this.groupBox1.TabStop = false;
            this.groupBox1.Text = "当前图层";
            // 
            // trackerBarWidget2
            // 
            this.trackerBarWidget2.DecimalPlaces = 0;
            this.trackerBarWidget2.Increment = new decimal(new int[] {
            1,
            0,
            0,
            65536});
            this.trackerBarWidget2.InnerScale = new decimal(new int[] {
            10,
            0,
            0,
            0});
            this.trackerBarWidget2.LabelCaption = "Mipmap-Bias";
            this.trackerBarWidget2.Location = new System.Drawing.Point(7, 196);
            this.trackerBarWidget2.MaxValue = new decimal(new int[] {
            20,
            0,
            0,
            0});
            this.trackerBarWidget2.MinValue = new decimal(new int[] {
            0,
            0,
            0,
            0});
            this.trackerBarWidget2.Name = "trackerBarWidget2";
            this.trackerBarWidget2.Size = new System.Drawing.Size(225, 55);
            this.trackerBarWidget2.TabIndex = 9;
            this.trackerBarWidget2.Value = new decimal(new int[] {
            5,
            0,
            0,
            0});
            // 
            // trackerBarWidget1
            // 
            this.trackerBarWidget1.DecimalPlaces = 0;
            this.trackerBarWidget1.Increment = new decimal(new int[] {
            1,
            0,
            0,
            65536});
            this.trackerBarWidget1.InnerScale = new decimal(new int[] {
            10,
            0,
            0,
            0});
            this.trackerBarWidget1.LabelCaption = "UV-Scale";
            this.trackerBarWidget1.Location = new System.Drawing.Point(7, 136);
            this.trackerBarWidget1.MaxValue = new decimal(new int[] {
            20,
            0,
            0,
            0});
            this.trackerBarWidget1.MinValue = new decimal(new int[] {
            0,
            0,
            0,
            0});
            this.trackerBarWidget1.Name = "trackerBarWidget1";
            this.trackerBarWidget1.Size = new System.Drawing.Size(225, 55);
            this.trackerBarWidget1.TabIndex = 8;
            this.trackerBarWidget1.Value = new decimal(new int[] {
            5,
            0,
            0,
            0});
            // 
            // groupBox2
            // 
            this.groupBox2.Controls.Add(this.pictureBox2);
            this.groupBox2.Location = new System.Drawing.Point(122, 20);
            this.groupBox2.Name = "groupBox2";
            this.groupBox2.Size = new System.Drawing.Size(110, 110);
            this.groupBox2.TabIndex = 1;
            this.groupBox2.TabStop = false;
            this.groupBox2.Text = "漫反射贴图";
            // 
            // pictureBox2
            // 
            this.pictureBox2.Location = new System.Drawing.Point(7, 21);
            this.pictureBox2.Name = "pictureBox2";
            this.pictureBox2.Size = new System.Drawing.Size(100, 83);
            this.pictureBox2.TabIndex = 0;
            this.pictureBox2.TabStop = false;
            // 
            // groupBoxLayerDiffuse
            // 
            this.groupBoxLayerDiffuse.Controls.Add(this.pictureBox1);
            this.groupBoxLayerDiffuse.Location = new System.Drawing.Point(7, 20);
            this.groupBoxLayerDiffuse.Name = "groupBoxLayerDiffuse";
            this.groupBoxLayerDiffuse.Size = new System.Drawing.Size(110, 110);
            this.groupBoxLayerDiffuse.TabIndex = 0;
            this.groupBoxLayerDiffuse.TabStop = false;
            this.groupBoxLayerDiffuse.Text = "漫反射贴图";
            // 
            // pictureBox1
            // 
            this.pictureBox1.Location = new System.Drawing.Point(7, 21);
            this.pictureBox1.Name = "pictureBox1";
            this.pictureBox1.Size = new System.Drawing.Size(100, 83);
            this.pictureBox1.TabIndex = 0;
            this.pictureBox1.TabStop = false;
            // 
            // listBoxLayer
            // 
            this.listBoxLayer.FormattingEnabled = true;
            this.listBoxLayer.ItemHeight = 12;
            this.listBoxLayer.Location = new System.Drawing.Point(7, 21);
            this.listBoxLayer.Name = "listBoxLayer";
            this.listBoxLayer.Size = new System.Drawing.Size(187, 148);
            this.listBoxLayer.TabIndex = 0;
            this.listBoxLayer.SelectedIndexChanged += new System.EventHandler(this.listBoxLayer_SelectedIndexChanged);
            // 
            // buttonLayerAdd
            // 
            this.buttonLayerAdd.Location = new System.Drawing.Point(200, 21);
            this.buttonLayerAdd.Name = "buttonLayerAdd";
            this.buttonLayerAdd.Size = new System.Drawing.Size(32, 32);
            this.buttonLayerAdd.TabIndex = 1;
            this.buttonLayerAdd.Text = "+";
            this.buttonLayerAdd.UseVisualStyleBackColor = true;
            // 
            // buttonLayerRemove
            // 
            this.buttonLayerRemove.Location = new System.Drawing.Point(200, 59);
            this.buttonLayerRemove.Name = "buttonLayerRemove";
            this.buttonLayerRemove.Size = new System.Drawing.Size(32, 32);
            this.buttonLayerRemove.TabIndex = 2;
            this.buttonLayerRemove.Text = "-";
            this.buttonLayerRemove.UseVisualStyleBackColor = true;
            // 
            // buttonLayerUp
            // 
            this.buttonLayerUp.Location = new System.Drawing.Point(200, 97);
            this.buttonLayerUp.Name = "buttonLayerUp";
            this.buttonLayerUp.Size = new System.Drawing.Size(32, 32);
            this.buttonLayerUp.TabIndex = 3;
            this.buttonLayerUp.Text = "上";
            this.buttonLayerUp.UseVisualStyleBackColor = true;
            // 
            // buttonLayerDown
            // 
            this.buttonLayerDown.Location = new System.Drawing.Point(200, 135);
            this.buttonLayerDown.Name = "buttonLayerDown";
            this.buttonLayerDown.Size = new System.Drawing.Size(32, 32);
            this.buttonLayerDown.TabIndex = 4;
            this.buttonLayerDown.Text = "下";
            this.buttonLayerDown.UseVisualStyleBackColor = true;
            // 
            // groupBoxLayer
            // 
            this.groupBoxLayer.Controls.Add(this.buttonLayerDown);
            this.groupBoxLayer.Controls.Add(this.buttonLayerUp);
            this.groupBoxLayer.Controls.Add(this.buttonLayerRemove);
            this.groupBoxLayer.Controls.Add(this.buttonLayerAdd);
            this.groupBoxLayer.Controls.Add(this.listBoxLayer);
            this.groupBoxLayer.Location = new System.Drawing.Point(6, 187);
            this.groupBoxLayer.Name = "groupBoxLayer";
            this.groupBoxLayer.Size = new System.Drawing.Size(238, 182);
            this.groupBoxLayer.TabIndex = 8;
            this.groupBoxLayer.TabStop = false;
            this.groupBoxLayer.Text = "图层";
            // 
            // TerrainSurfaceWidget
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 12F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.Controls.Add(this.groupBox1);
            this.Controls.Add(this.groupBoxLayer);
            this.Controls.Add(this.trackerBarWidgetBrushPower);
            this.Controls.Add(this.trackerBarWidgetInnerRadius);
            this.Controls.Add(this.trackerBarWidgetOutterRadius);
            this.Name = "TerrainSurfaceWidget";
            this.Size = new System.Drawing.Size(250, 636);
            this.groupBox1.ResumeLayout(false);
            this.groupBox2.ResumeLayout(false);
            ((System.ComponentModel.ISupportInitialize)(this.pictureBox2)).EndInit();
            this.groupBoxLayerDiffuse.ResumeLayout(false);
            ((System.ComponentModel.ISupportInitialize)(this.pictureBox1)).EndInit();
            this.groupBoxLayer.ResumeLayout(false);
            this.ResumeLayout(false);

        }

        #endregion

        private WorldEditor.Common.CustomWidget.TrackerBarWidget trackerBarWidgetInnerRadius;
        private WorldEditor.Common.CustomWidget.TrackerBarWidget trackerBarWidgetOutterRadius;
        private WorldEditor.Common.CustomWidget.TrackerBarWidget trackerBarWidgetBrushPower;
        private System.Windows.Forms.GroupBox groupBox1;
        private System.Windows.Forms.GroupBox groupBoxLayerDiffuse;
        private System.Windows.Forms.ListBox listBoxLayer;
        private System.Windows.Forms.Button buttonLayerAdd;
        private System.Windows.Forms.Button buttonLayerRemove;
        private System.Windows.Forms.Button buttonLayerUp;
        private System.Windows.Forms.Button buttonLayerDown;
        private System.Windows.Forms.GroupBox groupBoxLayer;
        private WorldEditor.Common.CustomWidget.TrackerBarWidget trackerBarWidget2;
        private WorldEditor.Common.CustomWidget.TrackerBarWidget trackerBarWidget1;
        private System.Windows.Forms.GroupBox groupBox2;
        private System.Windows.Forms.PictureBox pictureBox2;
        private System.Windows.Forms.PictureBox pictureBox1;
    }
}
