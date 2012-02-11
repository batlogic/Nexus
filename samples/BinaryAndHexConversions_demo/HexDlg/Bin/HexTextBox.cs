using System;
using System.Windows;
using System.Windows.Forms;

namespace Bin 
{
	/// -----------------------------------------------------------------------------
	/// Project	 : Bin
	/// Class	 : HexTextBox
	/// 
	/// Use of this software is privilege of CodeProject members. Feel free to modify
	/// and report any bugs to Codeproject.com.
	/// 
	/// -----------------------------------------------------------------------------
	/// <summary>
	/// This class helps edit data Binary, Hexadecimal, and Decimal values.
	/// </summary>
	/// <remarks>
	/// The default mode of this class is for Hexadecimal number representation.  
	/// In hexadecimal mode it allows the user to enter only hex values.  In binary
	/// mode it allows user to enter only binary numbers and also shows in the tooltip
	/// the bit position based on the mouse position.
	/// </remarks>
	/// <history>
	/// 	[Rudy Guzman]	4/10/2005	Created
	/// </history>
	/// -----------------------------------------------------------------------------
	public class HexTextBox : RichTextBox  //use rtb instead of textbox because it has capability to show char position
	{
		public enum NumberMode{Hex, Decimal, Bin};

		private NumberMode currentMode, previousMode;
		private int numBits, numBytes;
		private bool signedNumber;
		private System.Int64 minValue;
		private System.Int64 maxValue;
		private System.Windows.Forms.ToolTip tooltips;

		/// <summary>
		/// Initialize a new instance of this class with defaults set to Hex, 8 bits.
		/// </summary>
		/// <param name="hexData">Hex value.</param>
		public HexTextBox()
		{
			this.Multiline = false;
			previousMode = NumberMode.Hex;
			currentMode = NumberMode.Hex;
			//default to 8 bits
			numBits = 8;
			numBytes = 1;
			signedNumber = false;
			minValue = 0;
			maxValue = 256;

			//add handler to check key presses so user enters only valid data
			this.KeyPress += new KeyPressEventHandler(HexTextBox_KeyPress);
			this.MouseMove += new MouseEventHandler(HexTextBox_MouseMove);
			this.tooltips = new ToolTip();
		}

		//---------------------------------------------------------------------------------------------------------
		//		Properties
		//---------------------------------------------------------------------------------------------------------
		/// <summary>
		/// Sets/Gets the number of bits allowed by this textbox editor.  The user
		/// will not be able to enter values that would exceed the max value for
		/// the specified number of bits.
		/// </summary>
		public int NumberOfBits
		{
			get { return numBits; }
			set 
			{
				numBits = value;
				numBytes = (numBits - 1) / 8 + 1;
				updateMaxMinValues();
				updateVaue();
			}
		}

		/// <summary>
		/// This property is only valid when "NumberMode" is set to Decimal.
		/// </summary>
		/// <remarks>
		/// Changing this value from SignedNumber to Unsigned number automatically
		/// converts the value of this text box using two's complement.
		/// </remarks>
		public bool SignedNumber
		{
			get { return signedNumber; }
			set 
			{ 
				bool previousValue = signedNumber;
				signedNumber = value;
				updateMaxMinValues();
				if (currentMode == NumberMode.Decimal && previousValue != signedNumber && this.Text != "")
				{
					if (signedNumber)
					{
						uint data = uint.Parse(this.Text);
						this.Text = Conversion.UIntToInt(data, numBits).ToString();
					}
					else
					{
						int data = int.Parse(this.Text);
						this.Text = Conversion.IntToUInt(data, numBits).ToString();
					}
				}
			}
		}
		
		/// <summary>
		/// Set the valid editor mode for this textbox.
		/// </summary>
		public NumberMode CurrentNumberMode
		{
			get { return currentMode; }
			set
			{
				this.previousMode = this.currentMode;
				this.currentMode = value;
				updateVaue();
			}
		}

		/// <summary>
		/// The value passed must be in binary format
		/// </summary>
		public string BinaryText
		{
			set
			{
				this.previousMode = NumberMode.Bin;
				this.Text = value;
				updateVaue();
			}
		}

		//---------------------------------------------------------------------------------------------------------
		//		Public Methods
		//---------------------------------------------------------------------------------------------------------
		/// <summary>
		/// Sets the contents of this editor to the value passed in the parameter.
		/// </summary>
		/// <remarks>
		/// Even though the contents contents of the textbox are in Hexadecimal or Binary,
		/// you can use this method to set the contents by passing a number.
		/// </remarks>
		/// <param name="number"></param>
		public void SetValue(uint number)
		{
			if (number < minValue || number > maxValue)
				throw new Exception("Value is out of range");

			switch (currentMode)
			{
				case NumberMode.Hex:
					this.Text = Bin.Conversion.UIntToHex(number, numBytes);
					break;
				case NumberMode.Bin:
					this.Text = Bin.Conversion.UIntToBin(number, numBits);
					break;
				case NumberMode.Decimal:
					this.Text = number.ToString();
					break;
				default:
					throw new Exception("Unable to set value in textbox because current mode is unknown");
			}
		}

		/// <summary>
		/// Gets the value of the contents in this textbox.
		/// </summary>
		/// <returns>Unsigned integer that represents the value of the contents in this textbox.</returns>
		public uint GetValue()
		{
			return GetDecimalValue(this.Text);
		}

		//---------------------------------------------------------------------------------------------------------
		//		Private Methods
		//---------------------------------------------------------------------------------------------------------
		protected uint GetDecimalValue(string strNumber)
		{
			uint number = 0;
			if (strNumber != "")
			{
				switch (currentMode)
				{
					case NumberMode.Hex:
						number = Bin.Conversion.HexToUInt(strNumber);
						break;
					case NumberMode.Bin:
						number = Bin.Conversion.BinToUInt(strNumber);
						break;
					case NumberMode.Decimal:
						number = uint.Parse(strNumber);
						break;
				}
			}
			return number;
		}
		protected void updateMaxMinValues()
		{
			if (signedNumber)
			{
				maxValue = Convert.ToInt32(Math.Pow(2, numBits - 1) - 1);
				minValue = Convert.ToInt32(-Math.Pow(2, numBits - 1));
			}
			else
			{
				maxValue = Convert.ToUInt32(Math.Pow(2, numBits) - 1);
				minValue = 0;	
			}
		}

		protected void updateVaue()
		{
			if (previousMode == currentMode)
				return;
			if (this.Text == "")
				return;

			int startIndex  = 0;
			string conversion = previousMode.ToString() + "To" + currentMode.ToString();
			switch (conversion)
			{
				case "BinToHex":
					this.Text = Bin.Conversion.BinToHex(this.Text);
					break;
				case "BinToDecimal":
					if (signedNumber)
						this.Text = Bin.Conversion.BinToInt(this.Text).ToString();
					else
						this.Text = Bin.Conversion.BinToUInt(this.Text).ToString();
					break;
				case "HexToBin":
					this.Text = Bin.Conversion.HexToBin(this.Text);
					startIndex = this.Text.Length - numBits;
					this.Text = this.Text.Substring(startIndex, numBits);
					break;
				case"HexToDecimal":
					if (signedNumber)
						this.Text =Bin.Conversion.HexToInt(this.Text).ToString();
					else
						this.Text = Bin.Conversion.HexToUInt(this.Text).ToString();
					break;
				case "DecimalToBin":
					if (signedNumber)
						this.Text = Bin.Conversion.IntToBin(int.Parse(this.Text), numBits);
					else
						this.Text = Bin.Conversion.UIntToBin(uint.Parse(this.Text), numBits);
					startIndex = this.Text.Length - numBits;
					this.Text = this.Text.Substring(startIndex, numBits);
					break;
				case "DecimalToHex":
					if (signedNumber)
						this.Text = Bin.Conversion.IntToHex(int.Parse(this.Text), numBytes);
					else
						this.Text = Bin.Conversion.UIntToHex(uint.Parse(this.Text), numBytes);
					break;
				default:
					MessageBox.Show("There is  no capability to convert: " + conversion);
					break;
			}
			this.previousMode = this.currentMode;
		}

		private void InitializeComponent()
		{

		}

		private void HexTextBox_KeyPress(object sender, KeyPressEventArgs e)
		{
			if (e.KeyChar == Convert.ToChar(Keys.Back))
				return;

			string validKeys = "";
			switch (currentMode)
			{
				case NumberMode.Decimal:
					validKeys = "0123456789.-";
					break;
				case NumberMode.Hex:
					validKeys = "0123456789abcdefABCDEF";
					break;
				case NumberMode.Bin:
					validKeys="01";
					break;
			}
			bool keyFound = false;
			for (int i = 0; i < validKeys.Length; i++)
			{
				if (e.KeyChar == validKeys[i])
				{
					keyFound = true;
					break;
				}
			}

			if (!keyFound)
				e.Handled = true;

			string selText = this.SelectedText;
			int numSel = 0;
			if (selText != "" ) numSel = selText.Length;
			int numCharacters = this.Text.Length - numSel + 1;

			if (currentMode == NumberMode.Bin && numCharacters > this.numBits)
				e.Handled = true;

			if (currentMode == NumberMode.Hex)
			{
				int numBytes = 19 / 8;
				if (numBytes * 8 < numBits) numBytes++;
				if (numCharacters > numBytes * 2)
					e.Handled = true;
			}
		}

		private void HexTextBox_MouseMove(object sender, MouseEventArgs e)
		{
			if (this.Text == "")
			{
				this.tooltips.SetToolTip(this, "");
				return;
			}

			if (this.currentMode == NumberMode.Bin)
			{
				System.Drawing.Point pt = new System.Drawing.Point(e.X, e.Y);
				int index = this.GetCharIndexFromPosition(pt);
				int bitPosition = this.Text.Length - index - 1;
				this.tooltips.SetToolTip(this, "bit "+bitPosition.ToString());
			}
			else
				this.tooltips.SetToolTip(this, "");
		}
	}

}
