namespace wiNstallerGUI
{
    partial class Form1
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
            this.consoleOutput = new System.Windows.Forms.TextBox();
            this.groupBox1 = new System.Windows.Forms.GroupBox();
            this.installButton = new System.Windows.Forms.Button();
            this.reactivateButton = new System.Windows.Forms.Button();
            this.cancelButton = new System.Windows.Forms.Button();
            this.deactivateButton = new System.Windows.Forms.Button();
            this.jailbreakButton = new System.Windows.Forms.Button();
            this.activateButton = new System.Windows.Forms.Button();
            this.groupBox2 = new System.Windows.Forms.GroupBox();
            this.returntojailButton = new System.Windows.Forms.Button();
            this.groupBox1.SuspendLayout();
            this.groupBox2.SuspendLayout();
            this.SuspendLayout();
            // 
            // consoleOutput
            // 
            this.consoleOutput.Dock = System.Windows.Forms.DockStyle.Fill;
            this.consoleOutput.Font = new System.Drawing.Font("Courier New", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.consoleOutput.Location = new System.Drawing.Point(3, 16);
            this.consoleOutput.Multiline = true;
            this.consoleOutput.Name = "consoleOutput";
            this.consoleOutput.ScrollBars = System.Windows.Forms.ScrollBars.Both;
            this.consoleOutput.Size = new System.Drawing.Size(538, 351);
            this.consoleOutput.TabIndex = 0;
            this.consoleOutput.WordWrap = false;
            // 
            // groupBox1
            // 
            this.groupBox1.Controls.Add(this.consoleOutput);
            this.groupBox1.Location = new System.Drawing.Point(12, 105);
            this.groupBox1.Name = "groupBox1";
            this.groupBox1.Size = new System.Drawing.Size(544, 370);
            this.groupBox1.TabIndex = 1;
            this.groupBox1.TabStop = false;
            this.groupBox1.Text = "Console Output";
            // 
            // installButton
            // 
            this.installButton.Location = new System.Drawing.Point(6, 19);
            this.installButton.Name = "installButton";
            this.installButton.Size = new System.Drawing.Size(104, 23);
            this.installButton.TabIndex = 2;
            this.installButton.Text = "Install Application";
            this.installButton.UseVisualStyleBackColor = true;
            this.installButton.Click += new System.EventHandler(this.installButton_Click);
            // 
            // reactivateButton
            // 
            this.reactivateButton.Location = new System.Drawing.Point(116, 48);
            this.reactivateButton.Name = "reactivateButton";
            this.reactivateButton.Size = new System.Drawing.Size(102, 23);
            this.reactivateButton.TabIndex = 3;
            this.reactivateButton.Text = "Reactivate";
            this.reactivateButton.UseVisualStyleBackColor = true;
            this.reactivateButton.Click += new System.EventHandler(this.reactivateButton_Click);
            // 
            // cancelButton
            // 
            this.cancelButton.Enabled = false;
            this.cancelButton.Location = new System.Drawing.Point(463, 30);
            this.cancelButton.Name = "cancelButton";
            this.cancelButton.Size = new System.Drawing.Size(75, 23);
            this.cancelButton.TabIndex = 4;
            this.cancelButton.Text = "Cancel";
            this.cancelButton.UseVisualStyleBackColor = true;
            this.cancelButton.Click += new System.EventHandler(this.cancelButton_Click);
            // 
            // deactivateButton
            // 
            this.deactivateButton.Location = new System.Drawing.Point(224, 48);
            this.deactivateButton.Name = "deactivateButton";
            this.deactivateButton.Size = new System.Drawing.Size(107, 23);
            this.deactivateButton.TabIndex = 5;
            this.deactivateButton.Text = "Deactivate";
            this.deactivateButton.UseVisualStyleBackColor = true;
            this.deactivateButton.Click += new System.EventHandler(this.deactivateButton_Click);
            // 
            // jailbreakButton
            // 
            this.jailbreakButton.Location = new System.Drawing.Point(116, 19);
            this.jailbreakButton.Name = "jailbreakButton";
            this.jailbreakButton.Size = new System.Drawing.Size(102, 23);
            this.jailbreakButton.TabIndex = 6;
            this.jailbreakButton.Text = "Jailbreak";
            this.jailbreakButton.UseVisualStyleBackColor = true;
            this.jailbreakButton.Click += new System.EventHandler(this.jailbreakButton_Click);
            // 
            // activateButton
            // 
            this.activateButton.Location = new System.Drawing.Point(6, 48);
            this.activateButton.Name = "activateButton";
            this.activateButton.Size = new System.Drawing.Size(104, 23);
            this.activateButton.TabIndex = 7;
            this.activateButton.Text = "Activate";
            this.activateButton.UseVisualStyleBackColor = true;
            this.activateButton.Click += new System.EventHandler(this.activateButton_Click);
            // 
            // groupBox2
            // 
            this.groupBox2.Controls.Add(this.returntojailButton);
            this.groupBox2.Controls.Add(this.cancelButton);
            this.groupBox2.Controls.Add(this.installButton);
            this.groupBox2.Controls.Add(this.jailbreakButton);
            this.groupBox2.Controls.Add(this.activateButton);
            this.groupBox2.Controls.Add(this.deactivateButton);
            this.groupBox2.Controls.Add(this.reactivateButton);
            this.groupBox2.Location = new System.Drawing.Point(12, 10);
            this.groupBox2.Name = "groupBox2";
            this.groupBox2.Size = new System.Drawing.Size(544, 81);
            this.groupBox2.TabIndex = 8;
            this.groupBox2.TabStop = false;
            this.groupBox2.Text = "Actions";
            // 
            // returntojailButton
            // 
            this.returntojailButton.Location = new System.Drawing.Point(224, 19);
            this.returntojailButton.Name = "returntojailButton";
            this.returntojailButton.Size = new System.Drawing.Size(107, 23);
            this.returntojailButton.TabIndex = 8;
            this.returntojailButton.Text = "Return To Jail";
            this.returntojailButton.UseVisualStyleBackColor = true;
            this.returntojailButton.Click += new System.EventHandler(this.returntojailButton_Click);
            // 
            // Form1
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(568, 487);
            this.Controls.Add(this.groupBox2);
            this.Controls.Add(this.groupBox1);
            this.ForeColor = System.Drawing.SystemColors.ControlText;
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedDialog;
            this.MaximizeBox = false;
            this.Name = "Form1";
            this.ShowIcon = false;
            this.SizeGripStyle = System.Windows.Forms.SizeGripStyle.Hide;
            this.Text = "wiNstaller";
            this.groupBox1.ResumeLayout(false);
            this.groupBox1.PerformLayout();
            this.groupBox2.ResumeLayout(false);
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.TextBox consoleOutput;
        private System.Windows.Forms.GroupBox groupBox1;
        private System.Windows.Forms.Button installButton;
        private System.Windows.Forms.Button reactivateButton;
        private System.Windows.Forms.Button cancelButton;
        private System.Windows.Forms.Button deactivateButton;
        private System.Windows.Forms.Button jailbreakButton;
        private System.Windows.Forms.Button activateButton;
        private System.Windows.Forms.GroupBox groupBox2;
        private System.Windows.Forms.Button returntojailButton;
    }
}

