namespace WorldEditor.SceneModule.Window
{
    partial class SceneForm
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

        #region Windows Form Designer generated code

        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            this.groupBoxType = new System.Windows.Forms.GroupBox();
            this.groupBoxProperty = new System.Windows.Forms.GroupBox();
            this.buttonLight = new System.Windows.Forms.Button();
            this.groupBoxType.SuspendLayout();
            this.SuspendLayout();
            // 
            // groupBoxType
            // 
            this.groupBoxType.Controls.Add(this.buttonLight);
            this.groupBoxType.Location = new System.Drawing.Point(12, 4);
            this.groupBoxType.Name = "groupBoxType";
            this.groupBoxType.Size = new System.Drawing.Size(268, 180);
            this.groupBoxType.TabIndex = 0;
            this.groupBoxType.TabStop = false;
            this.groupBoxType.Text = "场景";
            // 
            // groupBoxProperty
            // 
            this.groupBoxProperty.Location = new System.Drawing.Point(13, 191);
            this.groupBoxProperty.Name = "groupBoxProperty";
            this.groupBoxProperty.Size = new System.Drawing.Size(267, 282);
            this.groupBoxProperty.TabIndex = 1;
            this.groupBoxProperty.TabStop = false;
            this.groupBoxProperty.Text = "属性";
            // 
            // buttonLight
            // 
            this.buttonLight.Location = new System.Drawing.Point(7, 46);
            this.buttonLight.Name = "buttonLight";
            this.buttonLight.Size = new System.Drawing.Size(75, 23);
            this.buttonLight.TabIndex = 0;
            this.buttonLight.Text = "灯光";
            this.buttonLight.UseVisualStyleBackColor = true;
            this.buttonLight.Click += new System.EventHandler(this.buttonLight_Click);
            // 
            // SceneForm
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 12F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(292, 485);
            this.Controls.Add(this.groupBoxProperty);
            this.Controls.Add(this.groupBoxType);
            this.DockAreas = ((WeifenLuo.WinFormsUI.Docking.DockAreas)(((((WeifenLuo.WinFormsUI.Docking.DockAreas.Float | WeifenLuo.WinFormsUI.Docking.DockAreas.DockRight)
                        | WeifenLuo.WinFormsUI.Docking.DockAreas.DockTop)
                        | WeifenLuo.WinFormsUI.Docking.DockAreas.DockBottom)
                        | WeifenLuo.WinFormsUI.Docking.DockAreas.Document)));
            this.Name = "SceneForm";
            this.TabText = "SceneForm";
            this.Text = "SceneForm";
            this.groupBoxType.ResumeLayout(false);
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.GroupBox groupBoxType;
        private System.Windows.Forms.GroupBox groupBoxProperty;
        private System.Windows.Forms.Button buttonLight;
    }
}