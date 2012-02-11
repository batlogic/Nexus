using System;
using System.Drawing;
using System.Collections;
using System.ComponentModel;
using System.Windows.Forms;
using System.Data;
using Bin;
using System.Diagnostics;

namespace HexDlg
{
	/// <summary>
	/// Summary description for Form1.
	/// </summary>
	public class HexDlgForm : System.Windows.Forms.Form
	{
		private float resFontSize;
		private int resWidth;
		private int resHeight;
		private System.Windows.Forms.GroupBox groupBox1;
		private HexGrid grdData;
		private System.Windows.Forms.Label label1;
		private System.Windows.Forms.Label label2;
		private System.Windows.Forms.Label label3;
		private System.Windows.Forms.TextBox txtBytePosition;
		private System.Windows.Forms.TextBox txtBitPosition;
		private System.Windows.Forms.TextBox txtNumberOfBits;
		private System.Windows.Forms.GroupBox groupBox2;
		private System.Windows.Forms.GroupBox groupBox3;
		private System.Windows.Forms.GroupBox groupBox4;
		private System.Windows.Forms.Button btnExit;
		private System.Windows.Forms.RadioButton rbViewBits;
		private System.Windows.Forms.RadioButton rbViewDecimal;
		private System.Windows.Forms.RadioButton rbViewHex;
		private System.Windows.Forms.RadioButton rbUnsigned;
		private System.Windows.Forms.RadioButton rbSigned;
		private HexTextBox txtValueInEditMode;
		private System.Windows.Forms.Button btnWriteData;
		private System.Windows.Forms.Label label4;
		private System.Windows.Forms.TextBox txtInitialData;
		private System.Windows.Forms.Button btnFillGrid;
		/// <summary>
		/// Required designer variable.
		/// </summary>
		private System.ComponentModel.Container components = null;

		public HexDlgForm()
		{
			Application.EnableVisualStyles();  //to enable win xp visual style

			//
			// Required for Windows Form Designer support
			//
			InitializeComponent();

			//
			// TODO: Add any constructor code after InitializeComponent call
			//
			this.resFontSize = this.txtValueInEditMode.Font.Size;
			this.resWidth = this.txtValueInEditMode.Width;
			this.resHeight = this.txtValueInEditMode.Height;
		}

		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		protected override void Dispose( bool disposing )
		{
			if( disposing )
			{
				if (components != null) 
				{
					components.Dispose();
				}
			}
			base.Dispose( disposing );
		}

		#region Windows Form Designer generated code
		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		private void InitializeComponent()
		{
			this.grdData = new Bin.HexGrid();
			this.groupBox1 = new System.Windows.Forms.GroupBox();
			this.txtNumberOfBits = new System.Windows.Forms.TextBox();
			this.txtBitPosition = new System.Windows.Forms.TextBox();
			this.txtBytePosition = new System.Windows.Forms.TextBox();
			this.label3 = new System.Windows.Forms.Label();
			this.label2 = new System.Windows.Forms.Label();
			this.label1 = new System.Windows.Forms.Label();
			this.groupBox2 = new System.Windows.Forms.GroupBox();
			this.rbViewBits = new System.Windows.Forms.RadioButton();
			this.rbViewDecimal = new System.Windows.Forms.RadioButton();
			this.rbViewHex = new System.Windows.Forms.RadioButton();
			this.groupBox3 = new System.Windows.Forms.GroupBox();
			this.rbUnsigned = new System.Windows.Forms.RadioButton();
			this.rbSigned = new System.Windows.Forms.RadioButton();
			this.groupBox4 = new System.Windows.Forms.GroupBox();
			this.txtValueInEditMode = new Bin.HexTextBox();
			this.btnWriteData = new System.Windows.Forms.Button();
			this.btnExit = new System.Windows.Forms.Button();
			this.label4 = new System.Windows.Forms.Label();
			this.txtInitialData = new System.Windows.Forms.TextBox();
			this.btnFillGrid = new System.Windows.Forms.Button();
			((System.ComponentModel.ISupportInitialize)(this.grdData)).BeginInit();
			this.groupBox1.SuspendLayout();
			this.groupBox2.SuspendLayout();
			this.groupBox3.SuspendLayout();
			this.groupBox4.SuspendLayout();
			this.SuspendLayout();
			// 
			// grdData
			// 
			this.grdData.AccessibleName = "DataGrid";
			this.grdData.AccessibleRole = System.Windows.Forms.AccessibleRole.Table;
			this.grdData.CaptionVisible = false;
			this.grdData.ColumnHeadersVisible = false;
			this.grdData.DataMember = "";
			this.grdData.HeaderForeColor = System.Drawing.SystemColors.ControlText;
			this.grdData.Location = new System.Drawing.Point(0, 72);
			this.grdData.Name = "grdData";
			this.grdData.NumColumns = 16;
			this.grdData.PreferredColumnWidth = 25;
			this.grdData.RowHeadersVisible = false;
			this.grdData.RowHeaderWidth = 25;
			this.grdData.Size = new System.Drawing.Size(408, 144);
			this.grdData.TabIndex = 22;
			this.grdData.CurrentCellChanged += new System.EventHandler(this.grdData_CurrentCellChanged);
			// 
			// groupBox1
			// 
			this.groupBox1.Controls.Add(this.txtNumberOfBits);
			this.groupBox1.Controls.Add(this.txtBitPosition);
			this.groupBox1.Controls.Add(this.txtBytePosition);
			this.groupBox1.Controls.Add(this.label3);
			this.groupBox1.Controls.Add(this.label2);
			this.groupBox1.Controls.Add(this.label1);
			this.groupBox1.Location = new System.Drawing.Point(8, 224);
			this.groupBox1.Name = "groupBox1";
			this.groupBox1.Size = new System.Drawing.Size(128, 104);
			this.groupBox1.TabIndex = 17;
			this.groupBox1.TabStop = false;
			this.groupBox1.Text = "Byte Location";
			// 
			// txtNumberOfBits
			// 
			this.txtNumberOfBits.Location = new System.Drawing.Point(88, 72);
			this.txtNumberOfBits.Name = "txtNumberOfBits";
			this.txtNumberOfBits.Size = new System.Drawing.Size(24, 20);
			this.txtNumberOfBits.TabIndex = 5;
			this.txtNumberOfBits.Text = "8";
			this.txtNumberOfBits.TextChanged += new System.EventHandler(this.txtNumberOfBits_TextChanged);
			// 
			// txtBitPosition
			// 
			this.txtBitPosition.Location = new System.Drawing.Point(88, 48);
			this.txtBitPosition.Name = "txtBitPosition";
			this.txtBitPosition.Size = new System.Drawing.Size(24, 20);
			this.txtBitPosition.TabIndex = 4;
			this.txtBitPosition.Text = "0";
			this.txtBitPosition.TextChanged += new System.EventHandler(this.txtBitPosition_TextChanged);
			// 
			// txtBytePosition
			// 
			this.txtBytePosition.Location = new System.Drawing.Point(88, 24);
			this.txtBytePosition.Name = "txtBytePosition";
			this.txtBytePosition.Size = new System.Drawing.Size(24, 20);
			this.txtBytePosition.TabIndex = 3;
			this.txtBytePosition.Text = "00";
			this.txtBytePosition.KeyPress += new System.Windows.Forms.KeyPressEventHandler(this.txtBytePosition_KeyPress);
			// 
			// label3
			// 
			this.label3.Location = new System.Drawing.Point(8, 72);
			this.label3.Name = "label3";
			this.label3.Size = new System.Drawing.Size(88, 16);
			this.label3.TabIndex = 2;
			this.label3.Text = "Number of Bits:";
			// 
			// label2
			// 
			this.label2.Location = new System.Drawing.Point(8, 48);
			this.label2.Name = "label2";
			this.label2.Size = new System.Drawing.Size(64, 16);
			this.label2.TabIndex = 1;
			this.label2.Text = "Bit Position:";
			// 
			// label1
			// 
			this.label1.Location = new System.Drawing.Point(8, 24);
			this.label1.Name = "label1";
			this.label1.Size = new System.Drawing.Size(80, 16);
			this.label1.TabIndex = 0;
			this.label1.Text = "Byte Position:";
			// 
			// groupBox2
			// 
			this.groupBox2.Controls.Add(this.rbViewBits);
			this.groupBox2.Controls.Add(this.rbViewDecimal);
			this.groupBox2.Controls.Add(this.rbViewHex);
			this.groupBox2.Location = new System.Drawing.Point(144, 224);
			this.groupBox2.Name = "groupBox2";
			this.groupBox2.Size = new System.Drawing.Size(112, 104);
			this.groupBox2.TabIndex = 18;
			this.groupBox2.TabStop = false;
			this.groupBox2.Text = "View Mode";
			// 
			// rbViewBits
			// 
			this.rbViewBits.Location = new System.Drawing.Point(8, 72);
			this.rbViewBits.Name = "rbViewBits";
			this.rbViewBits.Size = new System.Drawing.Size(88, 24);
			this.rbViewBits.TabIndex = 15;
			this.rbViewBits.Text = "View Bits";
			this.rbViewBits.CheckedChanged += new System.EventHandler(this.rbViewBits_CheckedChanged);
			// 
			// rbViewDecimal
			// 
			this.rbViewDecimal.Location = new System.Drawing.Point(8, 48);
			this.rbViewDecimal.Name = "rbViewDecimal";
			this.rbViewDecimal.Size = new System.Drawing.Size(96, 24);
			this.rbViewDecimal.TabIndex = 14;
			this.rbViewDecimal.Text = "View Decimal";
			this.rbViewDecimal.CheckedChanged += new System.EventHandler(this.rbViewDecimal_CheckedChanged);
			// 
			// rbViewHex
			// 
			this.rbViewHex.Checked = true;
			this.rbViewHex.Location = new System.Drawing.Point(8, 24);
			this.rbViewHex.Name = "rbViewHex";
			this.rbViewHex.Size = new System.Drawing.Size(88, 24);
			this.rbViewHex.TabIndex = 13;
			this.rbViewHex.TabStop = true;
			this.rbViewHex.Text = "View Hex";
			this.rbViewHex.CheckedChanged += new System.EventHandler(this.rbViewHex_CheckedChanged);
			// 
			// groupBox3
			// 
			this.groupBox3.Controls.Add(this.rbUnsigned);
			this.groupBox3.Controls.Add(this.rbSigned);
			this.groupBox3.Location = new System.Drawing.Point(264, 224);
			this.groupBox3.Name = "groupBox3";
			this.groupBox3.Size = new System.Drawing.Size(136, 104);
			this.groupBox3.TabIndex = 19;
			this.groupBox3.TabStop = false;
			this.groupBox3.Text = "Number";
			// 
			// rbUnsigned
			// 
			this.rbUnsigned.Checked = true;
			this.rbUnsigned.Enabled = false;
			this.rbUnsigned.Location = new System.Drawing.Point(16, 32);
			this.rbUnsigned.Name = "rbUnsigned";
			this.rbUnsigned.TabIndex = 18;
			this.rbUnsigned.TabStop = true;
			this.rbUnsigned.Text = "Unsigned Value";
			this.rbUnsigned.CheckedChanged += new System.EventHandler(this.rbUnsigned_CheckedChanged);
			// 
			// rbSigned
			// 
			this.rbSigned.Enabled = false;
			this.rbSigned.Location = new System.Drawing.Point(16, 64);
			this.rbSigned.Name = "rbSigned";
			this.rbSigned.TabIndex = 17;
			this.rbSigned.Text = "Signed Value";
			this.rbSigned.CheckedChanged += new System.EventHandler(this.rbSigned_CheckedChanged);
			// 
			// groupBox4
			// 
			this.groupBox4.Controls.Add(this.txtValueInEditMode);
			this.groupBox4.Location = new System.Drawing.Point(8, 336);
			this.groupBox4.Name = "groupBox4";
			this.groupBox4.Size = new System.Drawing.Size(392, 72);
			this.groupBox4.TabIndex = 20;
			this.groupBox4.TabStop = false;
			this.groupBox4.Text = "Bits in Edit Mode";
			// 
			// txtValueInEditMode
			// 
			this.txtValueInEditMode.CurrentNumberMode = Bin.HexTextBox.NumberMode.Hex;
			this.txtValueInEditMode.Font = new System.Drawing.Font("Courier New", 12F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((System.Byte)(0)));
			this.txtValueInEditMode.Location = new System.Drawing.Point(136, 16);
			this.txtValueInEditMode.Multiline = false;
			this.txtValueInEditMode.Name = "txtValueInEditMode";
			this.txtValueInEditMode.NumberOfBits = 8;
			this.txtValueInEditMode.SignedNumber = false;
			this.txtValueInEditMode.Size = new System.Drawing.Size(240, 26);
			this.txtValueInEditMode.TabIndex = 0;
			this.txtValueInEditMode.Text = "";
			// 
			// btnWriteData
			// 
			this.btnWriteData.Location = new System.Drawing.Point(0, 0);
			this.btnWriteData.Name = "btnWriteData";
			this.btnWriteData.TabIndex = 0;
			// 
			// btnExit
			// 
			this.btnExit.FlatStyle = System.Windows.Forms.FlatStyle.System;
			this.btnExit.Location = new System.Drawing.Point(152, 416);
			this.btnExit.Name = "btnExit";
			this.btnExit.TabIndex = 21;
			this.btnExit.Text = "EXIT";
			this.btnExit.Click += new System.EventHandler(this.btnExit_Click);
			// 
			// label4
			// 
			this.label4.Font = new System.Drawing.Font("Microsoft Sans Serif", 9.75F, ((System.Drawing.FontStyle)((System.Drawing.FontStyle.Bold | System.Drawing.FontStyle.Italic))), System.Drawing.GraphicsUnit.Point, ((System.Byte)(0)));
			this.label4.Location = new System.Drawing.Point(0, 4);
			this.label4.Name = "label4";
			this.label4.Size = new System.Drawing.Size(288, 16);
			this.label4.TabIndex = 23;
			this.label4.Text = "Enter Hexadecimal string value to be edited:";
			// 
			// txtInitialData
			// 
			this.txtInitialData.Location = new System.Drawing.Point(0, 24);
			this.txtInitialData.Name = "txtInitialData";
			this.txtInitialData.Size = new System.Drawing.Size(300, 20);
			this.txtInitialData.TabIndex = 24;
			this.txtInitialData.Text = "";
			// 
			// btnFillGrid
			// 
			this.btnFillGrid.Location = new System.Drawing.Point(328, 16);
			this.btnFillGrid.Name = "btnFillGrid";
			this.btnFillGrid.Size = new System.Drawing.Size(75, 32);
			this.btnFillGrid.TabIndex = 25;
			this.btnFillGrid.Text = "Fill Grid";
			this.btnFillGrid.Click += new System.EventHandler(this.btnFillGrid_Click);
			// 
			// HexDlgForm
			// 
			this.AutoScaleBaseSize = new System.Drawing.Size(5, 13);
			this.ClientSize = new System.Drawing.Size(408, 446);
			this.Controls.Add(this.btnFillGrid);
			this.Controls.Add(this.txtInitialData);
			this.Controls.Add(this.label4);
			this.Controls.Add(this.btnExit);
			this.Controls.Add(this.groupBox4);
			this.Controls.Add(this.groupBox3);
			this.Controls.Add(this.groupBox2);
			this.Controls.Add(this.groupBox1);
			this.Controls.Add(this.grdData);
			this.Name = "HexDlgForm";
			this.Load += new System.EventHandler(this.HexDlgForm_Load);
			((System.ComponentModel.ISupportInitialize)(this.grdData)).EndInit();
			this.groupBox1.ResumeLayout(false);
			this.groupBox2.ResumeLayout(false);
			this.groupBox3.ResumeLayout(false);
			this.groupBox4.ResumeLayout(false);
			this.ResumeLayout(false);

		}
		#endregion

		/// <summary>
		/// The main entry point for the application.
		/// </summary>
		[STAThread]
		static void Main() 
		{
			Application.Run(new HexDlgForm());
		}

		public void UpdateCurrentBytePosition()
		{
			int r = grdData.CurrentCell.RowNumber;
			int c = grdData.CurrentCell.ColumnNumber;
			int numCols = grdData.NumColumns;

			int pos = r * numCols + c;
//			if (pos < grdData.NumBytes)
			{
				this.txtBytePosition.Text = pos.ToString();
			}
//			else
//				this.txtBytePosition.Text = "----";
		}

		private void UpdateValueToEdit()
		{
			if (this.txtBytePosition.Text.Length > 0 && this.txtBitPosition.Text.Length > 0 && 
				this.txtNumberOfBits.Text.Length > 0)
			{
				try
				{
					int bytePos = int.Parse(this.txtBytePosition.Text);
					int bitPos = int.Parse(this.txtBitPosition.Text);
					int numBits = int.Parse(this.txtNumberOfBits.Text);
					this.txtValueInEditMode.NumberOfBits = numBits;
					this.txtValueInEditMode.BinaryText = grdData.GetValueFrom(bytePos, bitPos, numBits);
					this.txtValueInEditMode.Font = new Font(this.txtValueInEditMode.Font.FontFamily, this.resFontSize);
					this.txtValueInEditMode.Multiline = true;
					this.txtValueInEditMode.ForeColor = Color.Black;
					this.txtValueInEditMode.Size = new Size(this.resWidth, this.resHeight);
				}
				catch (ApplicationException ex)
				{
					this.txtValueInEditMode.Multiline = true;
					this.txtValueInEditMode.Font = new Font(this.txtValueInEditMode.Font.FontFamily, 8);
					this.txtValueInEditMode.ForeColor = Color.Red;
					this.txtValueInEditMode.Text = ex.Message;
					this.txtValueInEditMode.Size = new Size(resWidth, resHeight * 2);
				}
			}
		}

		private void HexDlgForm_Load(object sender, System.EventArgs e)
		{
			txtInitialData.Text = "0BFE2030405060708090A0B0C0D0E0F10111213141516";
		}

		private void grdData_CurrentCellChanged(object sender, System.EventArgs e)
		{
			UpdateCurrentBytePosition();
			UpdateValueToEdit();
		}

		private void btnExit_Click(object sender, System.EventArgs e)
		{
//			string methodName = "";
//			StackTrace st = new StackTrace(0);
//			methodName = st.GetFrame(0).GetMethod().Name;

//			MessageBox.Show("Method Name = " + methodName);
			this.Close();
		}

		private void rbViewDecimal_CheckedChanged(object sender, System.EventArgs e)
		{
			if (rbViewDecimal.Checked)
			{
				this.rbSigned.Enabled = true;
				this.rbUnsigned.Enabled = true;
				this.txtValueInEditMode.CurrentNumberMode = HexTextBox.NumberMode.Decimal;
			}
		}

		private void rbViewHex_CheckedChanged(object sender, System.EventArgs e)
		{
			if (rbViewHex.Checked)
			{
				this.rbSigned.Enabled = false;
				this.rbUnsigned.Enabled = false;
				this.txtValueInEditMode.CurrentNumberMode = HexTextBox.NumberMode.Hex;
			}
		}

		private void rbViewBits_CheckedChanged(object sender, System.EventArgs e)
		{
			if (rbViewBits.Checked)
			{
				this.rbSigned.Enabled = false;
				this.rbUnsigned.Enabled = false;
				this.txtValueInEditMode.CurrentNumberMode = HexTextBox.NumberMode.Bin;
			}
		}

		private void txtNumberOfBits_TextChanged(object sender, System.EventArgs e)
		{
			UpdateValueToEdit();
		}

		private void txtBitPosition_TextChanged(object sender, System.EventArgs e)
		{
			UpdateValueToEdit();
		}

		private void txtBytePosition_KeyPress(object sender, System.Windows.Forms.KeyPressEventArgs e)
		{
			UpdateValueToEdit();
		}

		private void rbUnsigned_CheckedChanged(object sender, System.EventArgs e)
		{
			if (rbUnsigned.Checked)
			{
				this.txtValueInEditMode.SignedNumber = false;
			}
		}

		private void rbSigned_CheckedChanged(object sender, System.EventArgs e)
		{
			if (rbSigned.Checked)
			{
				this.txtValueInEditMode.SignedNumber = true;
			}
		}

		private void btnFillGrid_Click(object sender, System.EventArgs e)
		{
			try
			{
				grdData.FillGrid(this.txtInitialData.Text);
			}
			catch (Exception ex)
			{
				MessageBox.Show(ex.Message);
			}
		}
	}
}
