namespace WorldEditor.SceneModule.Widget
{
    partial class LightPanel
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
            this.groupBoxAmbientLight = new System.Windows.Forms.GroupBox();
            this.groupBoxMainLight = new System.Windows.Forms.GroupBox();
            this.groupBoxAssistantLight = new System.Windows.Forms.GroupBox();
            this.label1 = new System.Windows.Forms.Label();
            this.label2 = new System.Windows.Forms.Label();
            this.labelAmbientSkyColor = new System.Windows.Forms.Label();
            this.labelAmbientTerrainColor = new System.Windows.Forms.Label();
            this.groupBoxAmbientLight.SuspendLayout();
            this.SuspendLayout();
            // 
            // groupBoxAmbientLight
            // 
            this.groupBoxAmbientLight.Controls.Add(this.labelAmbientTerrainColor);
            this.groupBoxAmbientLight.Controls.Add(this.labelAmbientSkyColor);
            this.groupBoxAmbientLight.Controls.Add(this.label2);
            this.groupBoxAmbientLight.Controls.Add(this.label1);
            this.groupBoxAmbientLight.Location = new System.Drawing.Point(4, 4);
            this.groupBoxAmbientLight.Name = "groupBoxAmbientLight";
            this.groupBoxAmbientLight.Size = new System.Drawing.Size(243, 171);
            this.groupBoxAmbientLight.TabIndex = 0;
            this.groupBoxAmbientLight.TabStop = false;
            this.groupBoxAmbientLight.Text = "环境光";
            // 
            // groupBoxMainLight
            // 
            this.groupBoxMainLight.Location = new System.Drawing.Point(4, 224);
            this.groupBoxMainLight.Name = "groupBoxMainLight";
            this.groupBoxMainLight.Size = new System.Drawing.Size(243, 109);
            this.groupBoxMainLight.TabIndex = 1;
            this.groupBoxMainLight.TabStop = false;
            this.groupBoxMainLight.Text = "主光源";
            // 
            // groupBoxAssistantLight
            // 
            this.groupBoxAssistantLight.Location = new System.Drawing.Point(4, 339);
            this.groupBoxAssistantLight.Name = "groupBoxAssistantLight";
            this.groupBoxAssistantLight.Size = new System.Drawing.Size(243, 150);
            this.groupBoxAssistantLight.TabIndex = 1;
            this.groupBoxAssistantLight.TabStop = false;
            this.groupBoxAssistantLight.Text = "辅助光源";
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Font = new System.Drawing.Font("SimSun", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(134)));
            this.label1.Location = new System.Drawing.Point(6, 25);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(59, 12);
            this.label1.TabIndex = 0;
            this.label1.Text = "天空颜色:";
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Location = new System.Drawing.Point(6, 55);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(59, 12);
            this.label2.TabIndex = 1;
            this.label2.Text = "地面颜色:";
            // 
            // labelAmbientSkyColor
            // 
            this.labelAmbientSkyColor.BackColor = System.Drawing.SystemColors.ActiveCaptionText;
            this.labelAmbientSkyColor.Location = new System.Drawing.Point(71, 21);
            this.labelAmbientSkyColor.Name = "labelAmbientSkyColor";
            this.labelAmbientSkyColor.Size = new System.Drawing.Size(165, 20);
            this.labelAmbientSkyColor.TabIndex = 2;
            // 
            // labelAmbientTerrainColor
            // 
            this.labelAmbientTerrainColor.BackColor = System.Drawing.SystemColors.ActiveCaptionText;
            this.labelAmbientTerrainColor.Location = new System.Drawing.Point(71, 52);
            this.labelAmbientTerrainColor.Name = "labelAmbientTerrainColor";
            this.labelAmbientTerrainColor.Size = new System.Drawing.Size(165, 20);
            this.labelAmbientTerrainColor.TabIndex = 3;
            // 
            // LightPanel
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 12F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.Controls.Add(this.groupBoxAssistantLight);
            this.Controls.Add(this.groupBoxMainLight);
            this.Controls.Add(this.groupBoxAmbientLight);
            this.Name = "LightPanel";
            this.Size = new System.Drawing.Size(250, 521);
            this.groupBoxAmbientLight.ResumeLayout(false);
            this.groupBoxAmbientLight.PerformLayout();
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.GroupBox groupBoxAmbientLight;
        private System.Windows.Forms.GroupBox groupBoxMainLight;
        private System.Windows.Forms.GroupBox groupBoxAssistantLight;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.Label labelAmbientTerrainColor;
        private System.Windows.Forms.Label labelAmbientSkyColor;
    }
}
